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
#include "zikprofile.h"
#include "zik.h"

#define BLUEZ_PROFILE_MANAGER_PATH "/org/bluez"
#define BLUEZ_PROFILE_MANAGER_IFACE "org.bluez.ProfileManager1"
#define BLUEZ_DEVICE_IFACE "org.bluez.Device1"

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
  SIGNAL_ZIK_CONNECTED,
  SIGNAL_ZIK_DISCONNECTED,
  LAST_SIGNAL
};

static guint zik_profile_signals[LAST_SIGNAL];

#define parent_class zik_profile_parent_class
G_DEFINE_TYPE (ZikProfile, zik_profile, G_TYPE_OBJECT);

/* D-Bus methods */
static void zik_profile_handle_method_call (GDBusConnection * connection,
    const gchar * sender, const gchar * object_path,
    const gchar * interface_name, const gchar * method_name,
    GVariant * parameters, GDBusMethodInvocation * invocation,
    gpointer userdata);

/* GObject methods */
static void zik_profile_finalize (GObject * object);

static void
zik_profile_class_init (ZikProfileClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  gobject_class->finalize = zik_profile_finalize;

  klass->introspection_data = g_dbus_node_info_new_for_xml (introspection_xml,
      NULL);
  g_assert (klass->introspection_data);

  klass->interface_vtable.method_call = zik_profile_handle_method_call;
  klass->interface_vtable.get_property = NULL;
  klass->interface_vtable.set_property = NULL;

  /* ZikProfile::zik-connected:
   * @profile: the ZikProfile instance
   * @device: the #Zik object connected
   */
  zik_profile_signals[SIGNAL_ZIK_CONNECTED] = g_signal_new ("zik-connected",
      G_TYPE_FROM_CLASS (klass), G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL,
      G_TYPE_NONE, 1, ZIK_TYPE);

  /* ZikProfile::zik-disconnected:
   * @profile: the ZikProfile instance
   * @device: the #Zik object disconnected
   */
  zik_profile_signals[SIGNAL_ZIK_DISCONNECTED] =
      g_signal_new ("zik-disconnected", G_TYPE_FROM_CLASS (klass),
          G_SIGNAL_RUN_LAST, 0, NULL, NULL, NULL, G_TYPE_NONE, 1,
          ZIK_TYPE);
}

static void
zik_profile_init (ZikProfile * profile)
{
  profile->conn = g_bus_get_sync (G_BUS_TYPE_SYSTEM, NULL, NULL);
  profile->devices = g_hash_table_new_full (g_str_hash, g_str_equal,
      g_free, (GDestroyNotify) g_object_unref);
}

static void
zik_profile_finalize (GObject * object)
{
  ZikProfile *profile = ZIK_PROFILE (object);

  g_object_unref (profile->conn);
  g_hash_table_unref (profile->devices);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

static gboolean
notify_disconnect (gpointer key, gpointer value, gpointer userdata)
{
  GObject *zik = G_OBJECT (value);
  ZikProfile *profile = (ZikProfile *) userdata;

  g_signal_emit (profile, zik_profile_signals[SIGNAL_ZIK_DISCONNECTED], 0, zik);

  return TRUE;
}

static void
zik_profile_release (ZikProfile * profile)
{
  g_debug ("zik_profile_release called\n");

  /* notify all device disconnection and clean devices hash table */
  g_hash_table_foreach_remove (profile->devices, notify_disconnect, profile);
}

static void
zik_profile_new_connection (ZikProfile * profile, const gchar * device,
    gint fd)
{
  ZikProfileClass *klass = ZIK_PROFILE_GET_CLASS (profile);
  GDBusInterface *iface = NULL;
  BluetoothDevice1 *bt_device;
  Zik *zik;

  g_info ("zik_profile_new_connection called with device '%s' and fd %d",
      device, fd);

  /* get the Device1 interface to have the name and bluetooth address of the
   * device */
  iface = g_dbus_object_manager_get_interface (profile->manager, device,
      BLUEZ_DEVICE_IFACE);
  if (iface == NULL) {
    g_critical ("failed to retrieve %s for '%s'\n", BLUEZ_DEVICE_IFACE, device);
    goto beach;
  }

  bt_device = BLUETOOTH_DEVICE1 (iface);

  /* delegate to subclass */
  zik = klass->new_connection (profile, bt_device, fd);
  if (zik == NULL) {
    g_critical ("failed to create new_connection for device '%s'", device);
    goto beach;
  }

  g_hash_table_insert (profile->devices, g_strdup (device), zik);

  g_signal_emit (profile, zik_profile_signals[SIGNAL_ZIK_CONNECTED], 0, zik);

  g_object_unref (iface);

  return;

beach:
  if (iface)
    g_object_unref (iface);

  close (fd);
}

static void
zik_profile_request_disconnection (ZikProfile * profile, const gchar * device)
{
  ZikProfileClass *klass = ZIK_PROFILE_GET_CLASS (profile);
  Zik *zik;

  g_info ("zik_profile_request_disconnection called with device '%s'\n",
     device);

  zik = g_hash_table_lookup (profile->devices, device);
  if (zik == NULL) {
    g_warning ("device '%s' not found", device);
    return;
  }

  g_signal_emit (profile, zik_profile_signals[SIGNAL_ZIK_DISCONNECTED],
      0, zik);

  if (!klass->close_connection (profile, zik))
    g_warning ("failed to close session for device '%s'", device);

  g_hash_table_remove (profile->devices, device);
}

static void
zik_profile_handle_method_call (GDBusConnection * connection,
    const gchar * sender, const gchar * object_path,
    const gchar * interface_name, const gchar * method_name,
    GVariant * parameters, GDBusMethodInvocation * invocation,
    gpointer userdata)
{
  ZikProfile *profile = (ZikProfile *) userdata;

  if (g_strcmp0 (method_name, "Release") == 0) {
    zik_profile_release (profile);

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

    zik_profile_new_connection (profile, device, fd);

    g_dbus_method_invocation_return_value (invocation, NULL);
  } else if (g_strcmp0 (method_name, "RequestDisconnection")) {
    const gchar * device;

    device = g_variant_get_string (parameters, NULL);
    zik_profile_request_disconnection (profile, device);

    g_dbus_method_invocation_return_value (invocation, NULL);
  }

out:
  return;
}

ZikProfile *
zik_profile_new (void)
{
  return (ZikProfile *) g_object_new (ZIK_PROFILE_TYPE, NULL);
}

gboolean
zik_profile_install (ZikProfile * profile, GDBusObjectManager * manager)
{
  ZikProfileClass *klass = ZIK_PROFILE_GET_CLASS (profile);
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
  profile->id = g_dbus_connection_register_object (profile->conn,
      klass->object_path, klass->introspection_data->interfaces[0],
      &klass->interface_vtable, profile, NULL, &error);
  if (profile->id == 0) {
    g_critical ("failed to register profile object: %s", error->message);
    g_object_unref (iface);
    g_error_free (error);
    return FALSE;
  }

  /* 3. fill some info about profile */
  g_variant_dict_init (&dict, NULL);
  g_variant_dict_insert_value (&dict, "Name",
      g_variant_new_string (klass->profile_name));
  g_variant_dict_insert_value (&dict, "RequireAuthentication",
      g_variant_new_boolean (TRUE));
  g_variant_dict_insert_value (&dict, "Role", g_variant_new_string ("client"));

  /* 4. register to bluetoothd */
  profile_manager = BLUETOOTH_PROFILE_MANAGER1 (iface);
  if (!bluetooth_profile_manager1_call_register_profile_sync (profile_manager,
        klass->object_path, klass->profile_uuid, g_variant_dict_end (&dict),
        NULL, &error)) {
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
zik_profile_uninstall (ZikProfile * profile)
{
  ZikProfileClass *klass = ZIK_PROFILE_GET_CLASS (profile);
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
      klass->object_path, NULL, &error);
  if (error) {
    g_warning ("failed to unregister profile from blutoothd: %s",
        error->message);
  }

skip_unregister_profile:
  g_dbus_connection_unregister_object (profile->conn, profile->id);

  g_object_unref (iface);
  g_object_unref (profile->manager);
}
