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

#include <stdlib.h>
#include <glib.h>
#include <gio/gio.h>
#include <unistd.h>
#include <errno.h>

#include "bluetooth-client.h"
#include "zik2profile.h"
#include "zik2message.h"
#include "zik2connection.h"
#include "zik2.h"
#include "zik2api.h"

#define BLUEZ_NAME "org.bluez"
#define BLUEZ_OBJECT_MANAGER_PATH "/"
#define BLUEZ_ADAPTER_IFACE "org.bluez.Adapter1"
#define BLUEZ_DEVICE_IFACE "org.bluez.Device1"

static gboolean list_devices = FALSE;
static gchar *device_addr = NULL;
static gboolean dump_api_xml = FALSE;
static gchar *noise_control_switch = NULL;
static gchar *noise_control_mode = NULL;
static gint noise_control_strength = 0;
static gchar *get_uri = NULL;

static GOptionEntry entries[] = {
  { "list", 'l', 0, G_OPTION_ARG_NONE, &list_devices, "List Zik2 devices paired", NULL },
  { "device", 'd', 0, G_OPTION_ARG_STRING, &device_addr, "Specify Zik2 device address", "01:23:45:67:89:AB" },
  { "set-noise-control", 0, 0, G_OPTION_ARG_STRING, &noise_control_switch, "Enable the noise control", "<on|off>" },
  { "set-noise-control-mode", 0, 0, G_OPTION_ARG_STRING, &noise_control_mode, "Select noise control mode (anc: noise cancelling, aoc: street mode)", "<off|anc|aoc>" },
  { "set-noise-control-strength", 0, 0, G_OPTION_ARG_INT, &noise_control_strength, "Select noise control strength", "<1|2>" },
  { "dump-api-xml", 0, 0, G_OPTION_ARG_NONE, &dump_api_xml, "Dump answer from all known api", NULL },
  { "get-uri", 0, 0, G_OPTION_ARG_STRING, &get_uri, "Get uri and print reply", "/api/..." },
  { NULL, 0, 0, 0, NULL, NULL, NULL }
};

typedef struct _Zik2Api Zik2Api;
struct _Zik2Api
{
  gchar *name;
  gchar *get_uri;
};

static const Zik2Api zik2_api[] = {
  { "ZIK2_API_AUDIO_TRACK_METADATA_PATH", ZIK2_API_AUDIO_TRACK_METADATA_PATH },
  { "ZIK2_API_AUDIO_NOISE_CONTROL_ENABLED_PATH", ZIK2_API_AUDIO_NOISE_CONTROL_ENABLED_PATH },
  { "ZIK2_API_AUDIO_NOISE_CONTROL_PATH", ZIK2_API_AUDIO_NOISE_CONTROL_PATH },
  { "ZIK2_API_AUDIO_NOISE_CONTROL_PHONE_MODE_PATH", ZIK2_API_AUDIO_NOISE_CONTROL_PHONE_MODE_PATH },
  { "ZIK2_API_AUDIO_THUMB_EQUALIZER_VALUE_PATH", ZIK2_API_AUDIO_THUMB_EQUALIZER_VALUE_PATH },
  { "ZIK2_API_AUDIO_EQUALIZER_ENABLED_PATH", ZIK2_API_AUDIO_EQUALIZER_ENABLED_PATH },
  { "ZIK2_API_AUDIO_SMART_AUDIO_TUNE_PATH", ZIK2_API_AUDIO_SMART_AUDIO_TUNE_PATH },
  { "ZIK2_API_AUDIO_PRESET_BYPASS_PATH", ZIK2_API_AUDIO_PRESET_BYPASS_PATH },
  { "ZIK2_API_AUDIO_PRESET_CURRENT_PATH", ZIK2_API_AUDIO_PRESET_CURRENT_PATH },
  { "ZIK2_API_AUDIO_SOUND_EFFECT_ENABLED_PATH", ZIK2_API_AUDIO_SOUND_EFFECT_ENABLED_PATH },
  { "ZIK2_API_AUDIO_SOUND_EFFECT_PATH", ZIK2_API_AUDIO_SOUND_EFFECT_PATH },
  { "ZIK2_API_AUDIO_NOISE_PATH", ZIK2_API_AUDIO_NOISE_PATH },
  { "ZIK2_API_AUDIO_VOLUME_PATH", ZIK2_API_AUDIO_VOLUME_PATH },
  { "ZIK2_API_AUDIO_SOURCE_PATH", ZIK2_API_AUDIO_SOURCE_PATH },
  { "ZIK2_API_SOFTWARE_VERSION_PATH", ZIK2_API_SOFTWARE_VERSION_PATH },
  { "ZIK2_API_BLUETOOTH_FRIENDLY_NAME_PATH", ZIK2_API_BLUETOOTH_FRIENDLY_NAME_PATH },
  { "ZIK2_API_SYSTEM_BATTERY_PATH", ZIK2_API_SYSTEM_BATTERY_PATH },
  { "ZIK2_API_SYSTEM_BATTERY_FORECAST_PATH", ZIK2_API_SYSTEM_BATTERY_FORECAST_PATH },
  { "ZIK2_API_SYSTEM_AUTO_CONNECTION_ENABLED_PATH", ZIK2_API_SYSTEM_AUTO_CONNECTION_ENABLED_PATH },
  { "ZIK2_API_SYSTEM_ANC_PHONE_MODE_ENABLED_PATH", ZIK2_API_SYSTEM_ANC_PHONE_MODE_ENABLED_PATH },
  { "ZIK2_API_SYSTEM_DEVICE_TYPE_PATH", ZIK2_API_SYSTEM_DEVICE_TYPE_PATH },
  { "ZIK2_API_SYSTEM_COLOR_PATH", ZIK2_API_SYSTEM_COLOR_PATH },
  { "ZIK2_API_SYSTEM_PI_PATH", ZIK2_API_SYSTEM_PI_PATH },
  { "ZIK2_API_SYSTEM_HEAD_DETECTION_ENABLED_PATH", ZIK2_API_SYSTEM_HEAD_DETECTION_ENABLED_PATH },
  { "ZIK2_API_SYSTEM_FLIGHT_MODE_PATH", ZIK2_API_SYSTEM_FLIGHT_MODE_PATH },
  { NULL, NULL },
};

static GSList *
zik2_device_list_new (GDBusObjectManager * manager)
{
  GSList *list = NULL;
  GList *objects;
  GList *walk;

  objects = g_dbus_object_manager_get_objects (manager);
  for (walk = objects; walk != NULL; walk = walk->next) {
    GDBusObject *object = G_DBUS_OBJECT (walk->data);
    GDBusInterface *interface;
    BluetoothDevice1 *device;
    const gchar * const *uuids;

    interface = g_dbus_object_get_interface (object, BLUEZ_DEVICE_IFACE);
    if (interface == NULL)
      continue;

    device = BLUETOOTH_DEVICE1 (interface);

    uuids = bluetooth_device1_get_uuids (device);
    if (g_strv_contains (uuids, ZIK2_PROFILE_UUID)) {
      list = g_slist_prepend (list, device);
    }
  }
  g_list_free_full (objects, g_object_unref);

  return list;
}

static BluetoothDevice1 *
lookup_device_by_addr (GSList * list, const gchar * addr)
{
  GSList *walk;

  for (walk = list; walk != NULL; walk = g_slist_next (walk)) {
    BluetoothDevice1 *device = BLUETOOTH_DEVICE1 (walk->data);
    const gchar *dev_addr;

    dev_addr = bluetooth_device1_get_address (device);
    if (g_strcmp0 (dev_addr, addr) == 0)
      return device;
  }

  return NULL;
}

static void
zik2_get (Zik2Connection * conn, const gchar * property)
{
  Zik2Message *msg;
  Zik2Message *answer = NULL;
  gchar *xml;

  msg = zik2_message_new_request (property, "get", NULL);
  zik2_connection_send_message (conn, msg, &answer);
  zik2_message_free (msg);

  xml = zik2_message_get_request_reply_xml (answer);
  if (xml) {
    g_print ("got answer: %s\n", xml);
  }
  g_free (xml);

  zik2_message_free (answer);
}

static const gchar *
nc_mode_str (Zik2NoiseControlMode mode)
{
  switch (mode) {
    case ZIK2_NOISE_CONTROL_MODE_OFF:
      return "off";
    case ZIK2_NOISE_CONTROL_MODE_ANC:
      return "anc (noise cancelling)";
    case ZIK2_NOISE_CONTROL_MODE_AOC:
        return "aoc (street mode)";
    default:
        break;
  }

  return "unknown";
}

static void
show_zik2 (Zik2 * zik2)
{
  gchar *serial;
  gchar *sw_version;
  gboolean nc_enabled;
  gchar *source;
  gchar *bat_state;
  guint bat_percent;
  Zik2NoiseControlMode noise_control_mode;
  guint noise_control_strength;
  guint volume;

  g_object_get (zik2, "serial", &serial,
      "noise-control-enabled", &nc_enabled,
      "source", &source,
      "software-version", &sw_version,
      "battery-state", &bat_state,
      "battery-percentage", &bat_percent,
      "noise-control-mode", &noise_control_mode,
      "noise-control-strength", &noise_control_strength,
      "volume", &volume,
      NULL);

  g_print ("audio:\n");
  g_print ("  noise control enabled  : %s\n", nc_enabled ? "true" : "false");
  g_print ("  noise control mode     : %s\n", nc_mode_str (noise_control_mode));
  g_print ("  noise control strength : %u\n", noise_control_strength);
  g_print ("  source                 : %s\n", source);
  g_print ("  volume (raw)           : %u\n", volume);

  g_print ("\nsoftware:\n");
  g_print ("  software version       : %s\n", sw_version);

  g_print ("\nsystem:\n");
  g_print ("  battery state          : %s (remaining: %u%%)\n", bat_state,
      bat_percent);
  g_print ("  serial-number          : %s\n", serial);

  g_free (serial);
  g_free (sw_version);
  g_free (source);
  g_free (bat_state);
}

static gboolean
set_noise_control (Zik2 * zik2)
{
  gboolean req_value;
  gboolean value;

  if (g_strcmp0 (noise_control_switch, "on") == 0)
    req_value = TRUE;
  else if (g_strcmp0 (noise_control_switch, "off") == 0)
    req_value = FALSE;
  else
    return FALSE;

  g_print ("%s noise control\n", req_value ? "Enabling" : "Disabling");
  g_object_set (zik2, "noise-control-enabled", req_value, NULL);
  g_object_get (zik2, "noise-control-enabled", &value, NULL);

  if (value != req_value) {
    g_printerr ("failed to %s noise control\n",
        req_value ? "enable" : "disable");
    return FALSE;
  }

  return TRUE;
}

static gboolean
set_noise_control_mode (Zik2 * zik2)
{
  Zik2NoiseControlMode req_mode;
  Zik2NoiseControlMode mode;

  if (g_strcmp0 (noise_control_mode, "off") == 0)
    req_mode = ZIK2_NOISE_CONTROL_MODE_OFF;
  else if (g_strcmp0 (noise_control_mode, "anc") == 0)
    req_mode = ZIK2_NOISE_CONTROL_MODE_ANC;
  else if (g_strcmp0 (noise_control_mode, "aoc") == 0)
    req_mode = ZIK2_NOISE_CONTROL_MODE_AOC;
  else
    return FALSE;

  g_print ("Setting noise control mode to %s\n", noise_control_mode);
  g_object_set (zik2, "noise-control-mode", req_mode, NULL);
  g_object_get (zik2, "noise-control-mode", &mode, NULL);

  if (mode != req_mode) {
    g_printerr ("failed to set noise control mode to %s\n", noise_control_mode);
    return FALSE;
  }

  return TRUE;
}

static gboolean
set_noise_control_strength (Zik2 * zik2)
{
  guint req_value = noise_control_strength;
  guint value;

  g_print ("Setting noise control strength to %u\n", noise_control_strength);
  g_object_set (zik2, "noise-control-strength", req_value, NULL);
  g_object_get (zik2, "noise-control-strength", &value, NULL);

  if (value != req_value) {
    g_printerr ("failed to set noise control strength to %u\n",
        noise_control_strength);
    return FALSE;
  }

  return TRUE;
}

static void
on_zik2_connected (Zik2Profile * profile, Zik2 * zik2, gpointer userdata)
{
  gchar *name;
  guint i;

  g_object_get (zik2, "name", &name, NULL);
  g_print ("connected to %s\n", name);
  g_free (name);

  /* process set request from user */
  if (noise_control_switch)
    set_noise_control (zik2);

  if (noise_control_mode)
    set_noise_control_mode (zik2);

  if (noise_control_strength)
    set_noise_control_strength (zik2);

  if (dump_api_xml) {
    for (i = 0; zik2_api[i].name != NULL; i++) {
      g_print ("- %s\n", zik2_api[i].name);
      zik2_get (zik2->conn, zik2_api[i].get_uri);
    }
  } else if (get_uri) {
    zik2_get (zik2->conn, get_uri);
  } else {
    show_zik2 (zik2);
  }
}

static void
on_profile_connected (GObject * object, GAsyncResult * res, gpointer userdata)
{
  BluetoothDevice1 *device = BLUETOOTH_DEVICE1 (object);
  GMainLoop *loop = (GMainLoop *) userdata;
  GError *error = NULL;

  if (!bluetooth_device1_call_connect_profile_finish (device, res, &error))
    g_printerr ("failed to connect: %s\n", error->message);

  /* work has been done when profile received a new connection
   * so we can quit the main loop */
  g_main_loop_quit (loop);
}


static Zik2Profile *
setup_profile (GDBusObjectManager * manager)
{
  Zik2Profile *profile;

  profile = zik2_profile_new ();
  g_signal_connect (profile, "zik2-connected", G_CALLBACK (on_zik2_connected),
      manager);

  zik2_profile_install (profile, manager);

  return profile;
}

void
cleanup_profile (Zik2Profile * profile, GDBusObjectManager * manager)
{
  zik2_profile_uninstall (profile);
  g_object_unref (profile);
}

static gboolean
check_arguments (void)
{
  gboolean ret = TRUE;

  if (noise_control_switch) {
    /* valid values: on, off */
    if (g_strcmp0 (noise_control_switch, "on") != 0 &&
        g_strcmp0 (noise_control_switch, "off")) {
      g_printerr ("unrecognized 'set-noise-control' value\n");
      ret = FALSE;
    }
  }

  if (noise_control_mode) {
    /* valid values: off, anc, aoc */
    if (g_strcmp0 (noise_control_mode, "off") != 0 &&
        g_strcmp0 (noise_control_mode, "anc") != 0 &&
        g_strcmp0 (noise_control_mode, "aoc") != 0) {
      g_printerr ("unrecognized 'set-noise-control-mode' value\n");
      ret = FALSE;
    }
  }

  if (noise_control_strength) {
    if (noise_control_strength < 1 && noise_control_strength > 2) {
      g_printerr ("unrecognized 'set-noise-control-strength' value\n");
      ret = FALSE;
    }
  }

  return ret;
}

int
main (int argc, char *argv[])
{
  gint ret = EXIT_FAILURE;
  GError *error = NULL;
  GOptionContext *context;
  GMainLoop *loop = NULL;
  GDBusObjectManager *manager = NULL;
  gchar *name_owner;
  GSList *zik2_devices = NULL;

  context = g_option_context_new ("- control Zik2 settings");
  g_option_context_add_main_entries (context, entries, 0);
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_printerr ("failed to parse options: %s\n", error->message);
    g_error_free (error);
    goto out;
  }

  if (!check_arguments ())
    goto out;

  loop = g_main_loop_new (NULL, FALSE);

  /* proxy bluez object manager */
  manager = bluetooth_object_manager_client_new_for_bus_sync (G_BUS_TYPE_SYSTEM,
      G_DBUS_OBJECT_MANAGER_CLIENT_FLAGS_DO_NOT_AUTO_START, BLUEZ_NAME,
      BLUEZ_OBJECT_MANAGER_PATH, NULL, &error);
  if (manager == NULL) {
    g_printerr ("failed to create bluetooth manager: %s\n", error->message);
    g_error_free (error);
    goto out;
  }

  /* check that bluetoothd is running */
  name_owner = g_dbus_object_manager_client_get_name_owner (
      G_DBUS_OBJECT_MANAGER_CLIENT (manager));
  if (name_owner == NULL) {
    g_printerr ("failed to connect to bluetoothd, check service is running\n");
    goto out;
  }
  g_free (name_owner);

  zik2_devices = zik2_device_list_new (manager);
  if (zik2_devices == NULL) {
    g_print ("No Zik2 has been paired, please pair them first\n");
    goto out;
  }

  if (list_devices) {
    /* display a list of paired Zik2 */
    GSList *walk;

    g_print ("List of Zik2 paired:\n");

    for (walk = zik2_devices; walk != NULL; walk = g_slist_next (walk)) {
      BluetoothDevice1 *device = BLUETOOTH_DEVICE1 (walk->data);

      g_print ("%s: %s\n", bluetooth_device1_get_name (device),
          bluetooth_device1_get_address (device));
    }
  } else {
    BluetoothDevice1 *device;
    Zik2Profile *profile;

    if (device_addr) {
      device = lookup_device_by_addr (zik2_devices, device_addr);
      if (device == NULL) {
        g_printerr ("device '%s' not found\n", device_addr);
        goto out;
      }
    } else if (g_slist_length (zik2_devices) == 1) {
      device = BLUETOOTH_DEVICE1 (zik2_devices->data);
    } else {
      /* there is more than one zik2 and no device specified, error out */
      g_print ("more than 1 device are paired, please select one\n");
      goto out;
    }

    profile = setup_profile (manager);

    /* connect asynchronously to profile as it is handled by this application
     * so through the main loop */
    g_print ("connecting to Zik2 device '%s'\n",
        bluetooth_device1_get_name (device));
    bluetooth_device1_call_connect_profile (device, ZIK2_PROFILE_UUID, NULL,
        on_profile_connected, loop);

    g_main_loop_run (loop);

    cleanup_profile (profile, manager);
  }

  ret = EXIT_SUCCESS;

out:
  if (loop)
    g_main_loop_unref (loop);

  if (manager)
    g_object_unref (manager);

  if (zik2_devices)
    g_slist_free_full (zik2_devices, g_object_unref);

  g_option_context_free (context);

  return ret;
}
