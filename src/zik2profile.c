/* Zik2ctl
 * Copyright (C) 2015 Aurélien Zanelli <aurelien.zanelli@darkosphere.fr>
 *
 * Zik2ctl is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zik2ctl is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with Zik2ctl. If not, see <http://www.gnu.org/licenses/>.
 */

#include <gio/gunixfdlist.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bluetooth-client.h"
#include "zik2.h"
#include "zik2profile.h"
#include "zik2connection.h"
#include "zik2message.h"

#define ZIK2_PATH "/org/bluez/zik2"
#define ZIK2_UUID "8b6814d3-6ce7-4498-9700-9312c1711f63"

#define BLUEZ_PROFILE_MANAGER_PATH "/org/bluez"
#define BLUEZ_PROFILE_MANAGER_IFACE "org.bluez.ProfileManager1"

static const gchar introspection_xml[] =
  "<node>"
  "  <interface name='org.bluez.Profile1'>"
  "    <method name='Release'></method>"
  "    <method name='NewConnection'>"
  "      <arg name='device' type='o' direction='in'/>"
  "      <arg name='fd' type='h' direction='in'/>"
  "      <arg name='fd_properties' type='a{sv}' direction='in'/>"
  "    </method>"
  "    <method name='RequestDisconnection'>"
  "      <arg name='device' type='o' direction='in'/>"
  "    </method>"
  "  </interface>"
  "</node>";

enum {
  SIGNAL_ZIK2_CONNECTED,
  SIGNAL_ZIK2_DISCONNECTED,
  LAST_SIGNAL
};

static guint zik2_profile_signals[LAST_SIGNAL];

#define parent_class zik2_profile_parent_class
G_DEFINE_TYPE (Zik2Profile, zik2_profile, G_TYPE_OBJECT);

/* D-Bus methods */
static void zik2_profile_handle_method_call (GDBusConnection * connection,
    const gchar * sender, const gchar * object_path,
    const gchar * interface_name, const gchar * method_name,
    GVariant * parameters, GDBusMethodInvocation * invocation,
    gpointer userdata);

/* GObject methods */
static void zik2_profile_finalize (GObject * object);

static void
zik2_profile_class_init (Zik2ProfileClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = zik2_profile_finalize;

  klass->introspection_data = g_dbus_node_info_new_for_xml (introspection_xml,
      NULL);
  g_assert (klass->introspection_data);

  klass->interface_vtable.method_call = zik2_profile_handle_method_call;
  klass->interface_vtable.get_property = NULL;
  klass->interface_vtable.set_property = NULL;

  /* Zik2Profile::zik2-connected:
   * @profile: the Zik2Profile instance
   * @device: the #Zik2 object connected
   */
  zik2_profile_signals[SIGNAL_ZIK2_CONNECTED] = g_signal_new ("zik2-connected",
      G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL,
      G_TYPE_NONE, 1, G_TYPE_OBJECT);

  /* Zik2Profile::zik2-disconnected:
   * @profile: the Zik2Profile instance
   * @device: the #zik2 object disconnected
   */
  zik2_profile_signals[SIGNAL_ZIK2_DISCONNECTED] =
      g_signal_new ("zik2-disconnected", G_TYPE_FROM_CLASS (klass),
          G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 1,
          G_TYPE_OBJECT);
}

static void
zik2_profile_init (Zik2Profile * profile)
{
  profile->conn = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, NULL);
  profile->devices = g_hash_table_new_full (g_str_hash, g_str_equal,
      g_free, (GDestroyNotify) g_object_unref);
}

static void
zik2_profile_finalize (GObject * object)
{
  Zik2Profile *profile = ZIK2_PROFILE (object);

  g_object_unref (profile->conn);
  g_hash_table_unref (profile->devices);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean
notify_disconnect (gpointer key, gpointer value, gpointer userdata)
{
  Zik2 *zik2 = ZIK2 (value);
  Zik2Profile *profile = (Zik2Profile *) userdata;

  g_signal_emit (profile, zik2_profile_signals[SIGNAL_ZIK2_DISCONNECTED], 0,
      zik2);

  return TRUE;
}

static void
zik2_profile_release (Zik2Profile * profile)
{
  g_debug ("zik2_profile_release called\n");

  /* notify all device disconnection and clean devices hash table */
  g_hash_table_foreach_remove (profile->devices, notify_disconnect, profile);
}

static void
zik2_profile_new_connection (Zik2Profile * profile, const gchar * device,
    gint fd)
{
  Zik2 *zik2;
  Zik2Connection *conn = NULL;

  g_info ("zik2_profile_new_connection called with device '%s' and fd %d",
      device, fd);

  conn = zik2_connection_new (fd);
  if (conn == NULL) {
    g_critical ("failed to create zik2_connection for fd %d", fd);
    close (fd);
    return;
  }

  /* open session */
  g_debug ("opening session for device '%s'", device);
  if (!zik2_connection_open_session (conn)) {
    g_warning ("failed to open session");
    zik2_connection_free (conn);
    close(fd);
    return;
  }

  zik2 = zik2_new (conn);
  g_hash_table_insert (profile->devices, g_strdup (device), zik2);

  g_signal_emit (profile, zik2_profile_signals[SIGNAL_ZIK2_CONNECTED], 0, zik2);
}

static void
zik2_profile_request_disconnection (Zik2Profile * profile, const gchar * device)
{
  Zik2 *zik2;

  g_info ("zik2_profile_request_disconnection called with device '%s'\n",
     device);

  zik2 = g_hash_table_lookup (profile->devices, device);
  if (zik2 == NULL) {
    g_warning ("device '%s' not found", device);
    return;
  }

  g_signal_emit (profile, zik2_profile_signals[SIGNAL_ZIK2_DISCONNECTED],
      0, zik2);
  g_hash_table_remove (profile->devices, device);
}

static void
zik2_profile_handle_method_call (GDBusConnection * connection,
    const gchar * sender, const gchar * object_path,
    const gchar * interface_name, const gchar * method_name,
    GVariant * parameters, GDBusMethodInvocation * invocation,
    gpointer userdata)
{
  Zik2Profile *profile = (Zik2Profile *) userdata;

  if (g_strcmp0 (method_name, "Release") == 0) {
    zik2_profile_release (profile);

    g_dbus_method_invocation_return_value (invocation, NULL);
  } else if (g_strcmp0 (method_name, "NewConnection") == 0) {
    const gchar *device;
    GDBusMessage *message;
    GUnixFDList *fdlist;
    gint fd;
    GError *error = NULL;
    GVariant *var;

    if (g_variant_n_children (parameters) != 3) {
      g_critical ("invalid number of parameters");
      g_dbus_method_invocation_return_error_literal (invocation, G_DBUS_ERROR,
          G_DBUS_ERROR_INVALID_ARGS, "invalid number of parameters");
      goto out;
    }

    var = g_variant_get_child_value (parameters, 0);
    device = g_variant_get_string (var, NULL);
    g_variant_unref (var);

    /* retrieve fd from message */
    var = g_variant_get_child_value (parameters, 1);
    message = g_dbus_method_invocation_get_message (invocation);
    fdlist = g_dbus_message_get_unix_fd_list (message);
    if (fdlist == NULL) {
      g_critical ("failed to get fd from message");
      g_variant_unref (var);
      g_dbus_method_invocation_return_error_literal (invocation, G_DBUS_ERROR,
          G_DBUS_ERROR_INVALID_ARGS, "failed to get fd list from message");
      goto out;
    }

    fd = g_unix_fd_list_get (fdlist, g_variant_get_handle (var), &error);
    if (fd == -1) {
      g_critical ("failed to get fd from list");
      g_variant_unref (var);
      g_dbus_method_invocation_return_gerror (invocation, error);
      g_error_free (error);
      goto out;
    }

    g_variant_unref (var);

    zik2_profile_new_connection (profile, device, fd);

    g_dbus_method_invocation_return_value (invocation, NULL);
  } else if (g_strcmp0 (method_name, "RequestDisconnection")) {
    const gchar * device;

    device = g_variant_get_string (parameters, NULL);
    zik2_profile_request_disconnection (profile, device);

    g_dbus_method_invocation_return_value (invocation, NULL);
  }

out:
  return;
}

Zik2Profile *
zik2_profile_new (void)
{
  return (Zik2Profile *) g_object_new (ZIK2_PROFILE_TYPE, NULL);
}

gboolean
zik2_profile_install (Zik2Profile * profile, GDBusObjectManager * manager)
{
  Zik2ProfileClass *klass = ZIK2_PROFILE_GET_CLASS (profile);
  GDBusInterface *iface;
  BluetoothProfileManager1 *profile_manager;
  GError *error = NULL;
  GVariantDict dict;

  /* 1. get ProfileManager1 interface */
  iface = g_dbus_object_manager_get_interface (manager,
      BLUEZ_PROFILE_MANAGER_PATH, BLUEZ_PROFILE_MANAGER_IFACE);
  if (iface == NULL) {
    g_critical ("failed to get '%s' interface for node '%s'",
        BLUEZ_PROFILE_MANAGER_IFACE, BLUEZ_PROFILE_MANAGER_PATH);
    return FALSE;
  }

  /* 2. export object on the bus */
  profile->id = g_dbus_connection_register_object (profile->conn, ZIK2_PATH,
      klass->introspection_data->interfaces[0], &klass->interface_vtable,
      profile, NULL, &error);
  if (profile->id == 0) {
    g_critical ("failed to register profile object: %s", error->message);
    g_object_unref (iface);
    g_error_free (error);
    return FALSE;
  }

  /* 3. fill some info about profile */
  g_variant_dict_init (&dict, NULL);
  g_variant_dict_insert_value (&dict, "Name",
      g_variant_new_string ("Zik2 controller profile"));
  g_variant_dict_insert_value (&dict, "RequireAuthentication",
      g_variant_new_boolean (TRUE));
  g_variant_dict_insert_value (&dict, "Role", g_variant_new_string ("client"));

  /* 4. register to bluetoothd */
  profile_manager = BLUETOOTH_PROFILE_MANAGER1 (iface);
  if (!bluetooth_profile_manager1_call_register_profile_sync (profile_manager,
        ZIK2_PATH, ZIK2_UUID, g_variant_dict_end (&dict), NULL, &error)) {
    g_critical ("failed to register profile on bluetoothd: %s", error->message);
    g_dbus_connection_unregister_object (profile->conn, profile->id);
    g_object_unref (iface);
    g_error_free (error);
    return FALSE;
  }

  g_object_unref (iface);

  profile->manager = g_object_ref (manager);

  return TRUE;
}

void
zik2_profile_uninstall (Zik2Profile * profile)
{
  GDBusInterface *iface;
  BluetoothProfileManager1 *profile_manager;
  GError *error = NULL;

  /* 1. get ProfileManager1 interface */
  iface = g_dbus_object_manager_get_interface (profile->manager,
      BLUEZ_PROFILE_MANAGER_PATH, BLUEZ_PROFILE_MANAGER_IFACE);
  if (iface == NULL) {
    g_critical ("failed to get '%s' interface for node '%s'",
        BLUEZ_PROFILE_MANAGER_IFACE, BLUEZ_PROFILE_MANAGER_PATH);
    goto skip_unregister_profile;
  }

  /* 2, unregister profile from bluetooth and unregister object from bus */
  profile_manager = BLUETOOTH_PROFILE_MANAGER1 (iface);
  bluetooth_profile_manager1_call_unregister_profile_sync (profile_manager,
      ZIK2_PATH, NULL, &error);
  if (error) {
    g_warning ("failed to unregister profile from blutoothd: %s",
        error->message);
  }

skip_unregister_profile:
  g_dbus_connection_unregister_object (profile->conn, profile->id);

  g_object_unref (iface);
  g_object_unref (profile->manager);
}
