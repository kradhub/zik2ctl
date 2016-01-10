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
#include "zikmessage.h"
#include "zikconnection.h"
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
static gchar *head_detection_switch = NULL;
static gchar *flight_mode_switch = NULL;
static gchar *friendlyname = NULL;
static gchar *sound_effect_switch = NULL;
static gchar *sound_effect_room = NULL;
static gint sound_effect_angle = -1;
static gchar *auto_connection_switch = NULL;
static gchar *equalizer_switch = NULL;
static gchar *smart_audio_tune_switch = NULL;
static gint auto_power_off_timeout = -1;
static gchar *tts_switch = NULL;

static gchar *request_path = NULL;
static gchar *request_method = NULL;
static gchar *request_args = NULL;

static GOptionEntry entries[] = {
  { "list", 'l', 0, G_OPTION_ARG_NONE, &list_devices, "List Zik2 devices paired", NULL },
  { "device", 'd', 0, G_OPTION_ARG_STRING, &device_addr, "Specify Zik2 device address", "01:23:45:67:89:AB" },
  { "set-noise-control", 0, 0, G_OPTION_ARG_STRING, &noise_control_switch, "Enable the noise control", "<on|off>" },
  { "set-noise-control-mode", 0, 0, G_OPTION_ARG_STRING, &noise_control_mode, "Select noise control mode (anc: noise cancelling, aoc: street mode)", "<off|anc|aoc>" },
  { "set-noise-control-strength", 0, 0, G_OPTION_ARG_INT, &noise_control_strength, "Select noise control strength", "<1|2>" },
  { "set-head-detection", 0, 0, G_OPTION_ARG_STRING, &head_detection_switch, "Enable the head detection", "<on|off>" },
  { "set-flight-mode", 0, 0, G_OPTION_ARG_STRING, &flight_mode_switch, "Enable/Disable flight mode", "<on|off>" },
  { "set-friendlyname", 0, 0, G_OPTION_ARG_STRING, &friendlyname, "Set the name used to generate bluetooth name", NULL },
  { "set-sound-effect", 0, 0, G_OPTION_ARG_STRING, &sound_effect_switch, "Enable/Disable sound effect (Concert Hall)", "<on|off>" },
  { "set-sound-effect-room", 0, 0, G_OPTION_ARG_STRING, &sound_effect_room, "Select room type for Concert Hall", "<silent|living|jazz|concert>" },
  { "set-sound-effect-angle", 0, 0, G_OPTION_ARG_INT, &sound_effect_angle, "Set the angle for sound effect (Concert Hall)", "<30|60|90|120|150|180>" },
  { "set-auto-connection", 0, 0, G_OPTION_ARG_STRING, &auto_connection_switch, "Enable/Disable device auto-connection", "<on|off>" },
  { "set-equalizer", 0, 0, G_OPTION_ARG_STRING, &equalizer_switch, "Enable/Disable device equalizer", "<on|off>" },
  { "set-smart-audio-tune", 0, 0, G_OPTION_ARG_STRING, &smart_audio_tune_switch, "Enable/Disable smart audio tune", "<on|off>" },
  { "set-auto-power-off-timeout", 0, 0, G_OPTION_ARG_INT, &auto_power_off_timeout, "Set device auto-power-off timeout in minutes", "<5|10|15|30|60>" },
  { "set-tts", 0, 0, G_OPTION_ARG_STRING, &tts_switch, "Enable/Disable text-to-speech", "<on|off>" },
  { "dump-api-xml", 0, 0, G_OPTION_ARG_NONE, &dump_api_xml, "Dump answer from all known api", NULL },
  { "request-path", 0, 0, G_OPTION_ARG_STRING, &request_path, "custom request path (development/debug purpose)", "/api/..." },
  { "request-method", 0, 0, G_OPTION_ARG_STRING, &request_method, "custom method to call (development/debug purpose)", "get" },
  { "request-args", 0, 0, G_OPTION_ARG_STRING, &request_args, "custom args (development/debug purpose)", "true" },
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

static gboolean
device_has_uuid (BluetoothDevice1 * device, const gchar * req_uuid)
{
  const gchar * const *uuids;

  uuids = bluetooth_device1_get_uuids (device);
  if (uuids == NULL)
    return FALSE;

  for (; *uuids != NULL; uuids++) {
    if (g_strcmp0 (req_uuid, *uuids) == 0)
      return TRUE;
  }

  return FALSE;
}

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

    interface = g_dbus_object_get_interface (object, BLUEZ_DEVICE_IFACE);
    if (interface == NULL)
      continue;

    if (device_has_uuid (BLUETOOTH_DEVICE1 (interface), ZIK2_PROFILE_UUID))
      list = g_slist_prepend (list, interface);
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
custom_request (Zik2 * zik2, const gchar * path, const gchar * method,
    const gchar * args)
{
  ZikMessage *msg;
  ZikMessage *reply;
  gchar *xml;

  msg = zik_message_new_request (path, method, args);
  zik_connection_send_message (zik2->conn, msg, &reply);
  zik_message_free (msg);

  g_print ("custom request '%s/%s?arg=%s' reply:\n", path, method, args);
  xml = zik_message_get_request_reply_xml (reply);
  if (xml)
    g_print ("%s\n", xml);
  else
    g_print ("no reply...\n");

  g_free (xml);
  zik_message_free (reply);

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

static const gchar *
color_str (Zik2Color color)
{
  switch (color) {
    case ZIK2_COLOR_BLACK:
      return "black";
    case ZIK2_COLOR_BLUE:
      return "blue";
    default:
      break;
  }

  return "unknown";
}

static void
show_zik2 (Zik2 * zik2)
{
  gboolean metadata_playing;
  const gchar *metadata_title;
  const gchar *metadata_artist;
  const gchar *metadata_album;
  const gchar *metadata_genre;
  guint auto_power_off_timeout;

  zik2_get_track_metadata (zik2, &metadata_playing, &metadata_title,
      &metadata_artist, &metadata_album, &metadata_genre);
  auto_power_off_timeout = zik2_get_auto_power_off_timeout (zik2);

  g_print ("audio:\n");
  g_print ("  noise control          : %s\n",
      zik2_is_noise_control_active (zik2) ? "on" : "off");
  g_print ("  noise control mode     : %s\n",
      nc_mode_str (zik2_get_noise_control_mode (zik2)));
  g_print ("  noise control strength : %u\n",
      zik2_get_noise_control_strength (zik2));
  g_print ("  sound effect           : %s\n",
      zik2_is_sound_effect_active (zik2) ? "on" : "off");
  g_print ("  sound effect room      : %s\n",
      zik2_sound_effect_room_name (zik2_get_sound_effect_room (zik2)));
  g_print ("  sound effect angle     : %u\n",
      zik2_get_sound_effect_angle (zik2));
  g_print ("  equalizer              : %s\n",
      zik2_is_equalizer_active (zik2) ? "on" : "off");
  g_print ("  smart audio tune       : %s\n",
      zik2_is_smart_audio_tune_active (zik2) ? "on" : "off");
  g_print ("  source                 : %s\n", zik2_get_source (zik2));
  g_print ("  volume (raw)           : %u\n", zik2_get_volume (zik2));

  g_print ("\ntrack metadata\n");
  g_print ("  playing                : %s\n", metadata_playing ? "yes" : "no");
  g_print ("  title                  : %s\n", metadata_title);
  g_print ("  artist                 : %s\n", metadata_artist);
  g_print ("  album                  : %s\n", metadata_album);
  g_print ("  genre                  : %s\n", metadata_genre);

  g_print ("\nsoftware:\n");
  g_print ("  software version       : %s\n", zik2_get_software_version (zik2));

  g_print ("\nsystem:\n");
  g_print ("  battery state          : %s (remaining: %u%%)\n",
      zik2_get_battery_state (zik2), zik2_get_battery_percentage (zik2));
  g_print ("  color                  : %s\n",
      color_str (zik2_get_color (zik2)));
  g_print ("  flight mode            : %s\n",
      zik2_is_flight_mode_active (zik2) ? "on" : "off");
  g_print ("  head detection         : %s\n",
      zik2_is_head_detection_active (zik2) ? "on" : "off");
  g_print ("  serial-number          : %s\n", zik2_get_serial (zik2));
  g_print ("  friendlyname           : %s\n", zik2_get_friendlyname (zik2));
  g_print ("  auto-connection        : %s\n",
      zik2_is_auto_connection_active (zik2) ? "on" : "off");

  if (auto_power_off_timeout > 0)
    g_print ("  auto power off timeout : %u minutes\n", auto_power_off_timeout);
  else
    g_print ("  auto power off timeout : off\n");

  g_print ("  text-to-speech         : %s\n",
      zik2_is_tts_active (zik2) ? "on" : "off");
}

static gboolean
set_boolean_property_from_string (Zik2 * zik2, const gchar * property,
    const gchar * value_str)
{
  gboolean req_value;
  gboolean value;

  if (g_strcmp0 (value_str, "on") == 0)
    req_value = TRUE;
  else if (g_strcmp0 (value_str, "off") == 0)
    req_value = FALSE;
  else
    return FALSE;

  g_object_set (zik2, property, req_value, NULL);
  g_object_get (zik2, property, &value, NULL);

  if (value != req_value)
    return FALSE;

  return TRUE;
}

static gboolean
set_noise_control_mode (Zik2 * zik2)
{
  Zik2NoiseControlMode req_mode;

  if (g_strcmp0 (noise_control_mode, "off") == 0)
    req_mode = ZIK2_NOISE_CONTROL_MODE_OFF;
  else if (g_strcmp0 (noise_control_mode, "anc") == 0)
    req_mode = ZIK2_NOISE_CONTROL_MODE_ANC;
  else if (g_strcmp0 (noise_control_mode, "aoc") == 0)
    req_mode = ZIK2_NOISE_CONTROL_MODE_AOC;
  else
    return FALSE;

  g_print ("Setting noise control mode to %s\n", noise_control_mode);
  if (!zik2_set_noise_control_mode (zik2, req_mode)) {
    g_printerr ("failed to set noise control mode to %s\n", noise_control_mode);
    return FALSE;
  }

  return TRUE;
}

static gboolean
set_noise_control_strength (Zik2 * zik2)
{
  guint req_value = noise_control_strength;

  g_print ("Setting noise control strength to %u\n", req_value);
  if (!zik2_set_noise_control_strength (zik2, req_value)) {
    g_printerr ("failed to set noise control strength to %u\n", req_value);
    return FALSE;
  }

  return TRUE;
}

static gboolean
set_friendly_name (Zik2 * zik2)
{
  gchar *req_value = friendlyname;

  g_print ("Setting friendlyname to '%s'\n", req_value);
  if (!zik2_set_friendlyname (zik2, req_value)) {
     g_printerr ("failed to set friendlyname to %s\n", req_value);
     return FALSE;
  }

  return TRUE;
}

static gboolean
set_sound_effect_room (Zik2 * zik2)
{
  Zik2SoundEffectRoom req_mode;

  req_mode = zik2_sound_effect_room_from_string (sound_effect_room);
  if (req_mode == ZIK2_SOUND_EFFECT_ROOM_UNKNOWN)
    return FALSE;

  g_print ("Setting sound effect room to %s\n", sound_effect_room);
  if (!zik2_set_sound_effect_room (zik2, req_mode)) {
    g_printerr ("failed to set sound effect room to %s\n", sound_effect_room);
    return FALSE;
  }

  return TRUE;
}

static gboolean
set_sound_effect_angle (Zik2 * zik2)
{
  guint req_value = sound_effect_angle;

  g_print ("Setting sound_effect_angle to %u\n", sound_effect_angle);
  if (!zik2_set_sound_effect_angle (zik2, req_value)) {
    g_printerr ("failed to set sound effect angle to %u\n", sound_effect_angle);
    return FALSE;
  }

  return TRUE;
}

static void
on_zik2_connected (ZikProfile * bprofile, Zik2 * zik2, gpointer userdata)
{
  gchar *name;
  guint i;

  g_object_get (zik2, "name", &name, NULL);
  g_print ("connected to %s\n", name);
  g_free (name);

  /* process set request from user */
  if (noise_control_switch) {
    g_print ("Setting noise control to %s\n", noise_control_switch);
    if (!set_boolean_property_from_string (zik2, "noise-control",
          noise_control_switch))
      g_printerr ("Failed to set noise control\n");
  }

  if (noise_control_mode)
    set_noise_control_mode (zik2);

  if (noise_control_strength)
    set_noise_control_strength (zik2);

  if (head_detection_switch) {
    g_print ("Setting head detection to %s\n", head_detection_switch);
    if (!set_boolean_property_from_string (zik2, "head-detection",
          head_detection_switch))
      g_printerr ("Failed to set head detection\n");
  }

  if (friendlyname)
    set_friendly_name (zik2);

  if (sound_effect_switch) {
    g_print ("Setting sound effect to %s\n", sound_effect_switch);
    if (!set_boolean_property_from_string (zik2, "sound-effect",
          sound_effect_switch))
      g_printerr ("Failed to set sound effect\n");
  }

  if (sound_effect_room)
    set_sound_effect_room (zik2);

  if (sound_effect_angle > 0)
    set_sound_effect_angle (zik2);

  if (auto_connection_switch) {
    g_print ("Setting auto-connection to %s\n", auto_connection_switch);
    if (!set_boolean_property_from_string (zik2, "auto-connection",
          auto_connection_switch))
      g_printerr ("Failed to set auto-connection\n");
  }

  if (equalizer_switch) {
    g_print ("Setting equalizer to %s\n", equalizer_switch);
    if (!set_boolean_property_from_string (zik2, "equalizer", equalizer_switch))
      g_printerr ("Failed to set equalizer\n");
  }

  if (smart_audio_tune_switch) {
    g_print ("Setting smart audio tune to %s\n", smart_audio_tune_switch);
    if (!set_boolean_property_from_string (zik2, "smart-audio-tune",
          smart_audio_tune_switch))
      g_printerr ("Failed to set smart audio tune\n");
  }

  if (auto_power_off_timeout != -1)
    zik2_set_auto_power_off_timeout (zik2, auto_power_off_timeout);

  if (tts_switch) {
    g_print ("Setting text-to-speech to %s\n", tts_switch);
    if (!set_boolean_property_from_string (zik2, "tts", tts_switch))
      g_printerr ("Failed to set text-to-speech\n");
  }

  if (flight_mode_switch) {
    /* enabling flight mode reset connection so don't send request */
    g_print ("Setting flight mode to %s\n", flight_mode_switch);
    if (!set_boolean_property_from_string (zik2, "flight-mode",
          flight_mode_switch))
      g_printerr ("Failed to set flight mode\n");
  } else if (dump_api_xml) {
    for (i = 0; zik2_api[i].name != NULL; i++) {
      g_print ("- %s\n", zik2_api[i].name);
      custom_request (zik2, zik2_api[i].get_uri, "get", NULL);
    }
  } else if (request_path) {
    custom_request (zik2, request_path, request_method, request_args);
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
  g_signal_connect (profile, "zik-connected", G_CALLBACK (on_zik2_connected),
      manager);

  zik_profile_install (ZIK_PROFILE (profile), manager);

  return profile;
}

void
cleanup_profile (Zik2Profile * profile, GDBusObjectManager * manager)
{
  zik_profile_uninstall (ZIK_PROFILE (profile));
  g_object_unref (profile);
}

static gboolean
check_switch_argument (const gchar * sw, const gchar * option_name)
{
  if (g_strcmp0 (sw, "on") != 0 && g_strcmp0 (sw, "off")) {
    g_printerr ("unrecognized '%s' value\n", option_name);
    return FALSE;
  }

  return TRUE;
}

static gboolean
check_arguments (void)
{
  gboolean ret = TRUE;

  if (noise_control_switch)
    ret = check_switch_argument (noise_control_switch, "set-noise-control");

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

  if (head_detection_switch)
    ret = check_switch_argument (head_detection_switch, "set-head-detection");

  if (flight_mode_switch)
    ret = check_switch_argument (flight_mode_switch, "set-flight-mode");

  if (sound_effect_switch)
    ret = check_switch_argument (sound_effect_switch, "set-sound-effect");

  if (sound_effect_room) {
    /* valid values: silent, living, jazz, concert */
    if (g_strcmp0 (sound_effect_room, "silent") != 0 &&
        g_strcmp0 (sound_effect_room, "living") != 0 &&
        g_strcmp0 (sound_effect_room, "jazz") != 0 &&
        g_strcmp0 (sound_effect_room, "concert") != 0) {
      g_printerr ("unrecognized 'set-sound-effect-room' value\n");
      ret = FALSE;
    }
  }

  if (sound_effect_angle != -1) {
    /* valid values: 30, 60, 90, 120, 150, 180 */
    if (sound_effect_angle != 30 && sound_effect_angle != 60 &&
        sound_effect_angle != 90 && sound_effect_angle != 120 &&
        sound_effect_angle != 150 && sound_effect_angle != 180) {
      g_printerr ("unrecognized 'set-sound-effect-angle' value\n");
      ret = FALSE;
    }
  }

  if (auto_connection_switch)
    ret = check_switch_argument (auto_connection_switch, "set-auto-connection");

  if (equalizer_switch)
    ret = check_switch_argument (equalizer_switch, "set-equalizer");

  if (smart_audio_tune_switch)
    ret = check_switch_argument (smart_audio_tune_switch,
        "set-smart-audio-tune");

  if ((request_path && request_method == NULL) ||
      (request_path == NULL && request_method)) {
    g_printerr ("request-path and request-method shall be used together\n");
    ret = FALSE;
  }

  if (auto_power_off_timeout != -1) {
    /* valid value 0, 5, 10, 15, 30, 60 */
    if (auto_power_off_timeout != 0 && auto_power_off_timeout != 5 &&
        auto_power_off_timeout != 10 && auto_power_off_timeout != 15 &&
        auto_power_off_timeout != 30 && auto_power_off_timeout != 60) {
      g_printerr ("unrecognized 'auto_power_off_timeout' value");
      ret = FALSE;
    }
  }

  if (tts_switch)
    ret = check_switch_argument (tts_switch, "set-tts");

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
