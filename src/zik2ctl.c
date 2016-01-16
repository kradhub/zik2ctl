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
#include "zikapi.h"
#include "zikmessage.h"
#include "zikconnection.h"
#include "zik2/zik2.h"
#include "zik2/zik2profile.h"
#include "zik3/zik3.h"
#include "zik3/zik3profile.h"


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
static gchar *auto_noise_control_switch = NULL;

static gchar *request_path = NULL;
static gchar *request_method = NULL;
static gchar *request_args = NULL;

static GOptionEntry entries[] = {
  { "list", 'l', 0, G_OPTION_ARG_NONE, &list_devices, "List Zik devices paired", NULL },
  { "device", 'd', 0, G_OPTION_ARG_STRING, &device_addr, "Specify Zik device address", "01:23:45:67:89:AB" },
  { "set-noise-control", 0, 0, G_OPTION_ARG_STRING, &noise_control_switch, "Enable the noise control", "<on|off>" },
  { "set-noise-control-mode", 0, 0, G_OPTION_ARG_STRING, &noise_control_mode, "Select noise control mode (anc: noise cancelling, aoc: street mode)", "<off|anc|aoc>" },
  { "set-noise-control-strength", 0, 0, G_OPTION_ARG_INT, &noise_control_strength, "Select noise control strength", "<1|2>" },
  { "set-auto-noise-control", 0, 0, G_OPTION_ARG_STRING, &auto_noise_control_switch, "Enable/Disable automatic noise control (Zik3 only)", "<on|off>" },
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
  { "ZIK_API_AUDIO_TRACK_METADATA_PATH", ZIK_API_AUDIO_TRACK_METADATA_PATH },
  { "ZIK_API_AUDIO_NOISE_CONTROL_ENABLED_PATH", ZIK_API_AUDIO_NOISE_CONTROL_ENABLED_PATH },
  { "ZIK_API_AUDIO_NOISE_CONTROL_PATH", ZIK_API_AUDIO_NOISE_CONTROL_PATH },
  { "ZIK_API_AUDIO_NOISE_CONTROL_PHONE_MODE_PATH", ZIK_API_AUDIO_NOISE_CONTROL_PHONE_MODE_PATH },
  { "ZIK_API_AUDIO_THUMB_EQUALIZER_VALUE_PATH", ZIK_API_AUDIO_THUMB_EQUALIZER_VALUE_PATH },
  { "ZIK_API_AUDIO_EQUALIZER_ENABLED_PATH", ZIK_API_AUDIO_EQUALIZER_ENABLED_PATH },
  { "ZIK_API_AUDIO_SMART_AUDIO_TUNE_PATH", ZIK_API_AUDIO_SMART_AUDIO_TUNE_PATH },
  { "ZIK_API_AUDIO_PRESET_BYPASS_PATH", ZIK_API_AUDIO_PRESET_BYPASS_PATH },
  { "ZIK_API_AUDIO_PRESET_CURRENT_PATH", ZIK_API_AUDIO_PRESET_CURRENT_PATH },
  { "ZIK_API_AUDIO_SOUND_EFFECT_ENABLED_PATH", ZIK_API_AUDIO_SOUND_EFFECT_ENABLED_PATH },
  { "ZIK_API_AUDIO_SOUND_EFFECT_PATH", ZIK_API_AUDIO_SOUND_EFFECT_PATH },
  { "ZIK_API_AUDIO_NOISE_PATH", ZIK_API_AUDIO_NOISE_PATH },
  { "ZIK_API_AUDIO_VOLUME_PATH", ZIK_API_AUDIO_VOLUME_PATH },
  { "ZIK_API_AUDIO_SOURCE_PATH", ZIK_API_AUDIO_SOURCE_PATH },
  { "ZIK_API_SOFTWARE_VERSION_PATH", ZIK_API_SOFTWARE_VERSION_PATH },
  { "ZIK_API_BLUETOOTH_FRIENDLY_NAME_PATH", ZIK_API_BLUETOOTH_FRIENDLY_NAME_PATH },
  { "ZIK_API_SYSTEM_BATTERY_PATH", ZIK_API_SYSTEM_BATTERY_PATH },
  { "ZIK_API_SYSTEM_BATTERY_FORECAST_PATH", ZIK_API_SYSTEM_BATTERY_FORECAST_PATH },
  { "ZIK_API_SYSTEM_AUTO_CONNECTION_ENABLED_PATH", ZIK_API_SYSTEM_AUTO_CONNECTION_ENABLED_PATH },
  { "ZIK_API_SYSTEM_ANC_PHONE_MODE_ENABLED_PATH", ZIK_API_SYSTEM_ANC_PHONE_MODE_ENABLED_PATH },
  { "ZIK_API_SYSTEM_DEVICE_TYPE_PATH", ZIK_API_SYSTEM_DEVICE_TYPE_PATH },
  { "ZIK_API_SYSTEM_COLOR_PATH", ZIK_API_SYSTEM_COLOR_PATH },
  { "ZIK_API_SYSTEM_PI_PATH", ZIK_API_SYSTEM_PI_PATH },
  { "ZIK_API_SYSTEM_HEAD_DETECTION_ENABLED_PATH", ZIK_API_SYSTEM_HEAD_DETECTION_ENABLED_PATH },
  { "ZIK_API_SYSTEM_FLIGHT_MODE_PATH", ZIK_API_SYSTEM_FLIGHT_MODE_PATH },
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

static gboolean
is_zik2_device (BluetoothDevice1 * device)
{
  return device_has_uuid (device, ZIK2_PROFILE_UUID);
}

static gboolean
is_zik3_device (BluetoothDevice1 * device)
{
  return device_has_uuid (device, ZIK3_PROFILE_UUID);
}

static GSList *
zik_device_list_new (GDBusObjectManager * manager)
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

    if (is_zik2_device (BLUETOOTH_DEVICE1 (interface)) ||
        is_zik3_device (BLUETOOTH_DEVICE1 (interface)))
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
custom_request (Zik * zik, const gchar * path, const gchar * method,
    const gchar * args)
{
  ZikMessage *msg;
  ZikMessage *reply;
  gchar *xml;

  msg = zik_message_new_request (path, method, args);
  zik_connection_send_message (zik_get_connection (zik), msg, &reply);
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
nc_mode_str (ZikNoiseControlMode mode)
{
  switch (mode) {
    case ZIK_NOISE_CONTROL_MODE_OFF:
      return "off";
    case ZIK_NOISE_CONTROL_MODE_ANC:
      return "anc (noise cancelling)";
    case ZIK_NOISE_CONTROL_MODE_AOC:
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
show_zik (Zik * zik)
{
  gboolean metadata_playing;
  const gchar *metadata_title;
  const gchar *metadata_artist;
  const gchar *metadata_album;
  const gchar *metadata_genre;
  guint auto_power_off_timeout;

  zik_get_track_metadata (zik, &metadata_playing, &metadata_title,
      &metadata_artist, &metadata_album, &metadata_genre);
  auto_power_off_timeout = zik_get_auto_power_off_timeout (zik);

  g_print ("audio:\n");
  g_print ("  noise control          : %s\n",
      zik_is_noise_control_active (zik) ? "on" : "off");
  g_print ("  noise control mode     : %s\n",
      nc_mode_str (zik_get_noise_control_mode (zik)));
  g_print ("  noise control strength : %u\n",
      zik_get_noise_control_strength (zik));

  if (IS_ZIK3 (zik))
    g_print ("  noise control auto     : %s\n",
        zik3_is_auto_noise_control_active (ZIK3_CAST (zik)) ? "on" : "off");

  g_print ("  sound effect           : %s\n",
      zik_is_sound_effect_active (zik) ? "on" : "off");
  g_print ("  sound effect room      : %s\n",
      zik_sound_effect_room_name (zik_get_sound_effect_room (zik)));
  g_print ("  sound effect angle     : %u\n",
      zik_get_sound_effect_angle (zik));
  g_print ("  equalizer              : %s\n",
      zik_is_equalizer_active (zik) ? "on" : "off");
  g_print ("  smart audio tune       : %s\n",
      zik_is_smart_audio_tune_active (zik) ? "on" : "off");
  g_print ("  source                 : %s\n", zik_get_source (zik));
  g_print ("  volume (raw)           : %u\n", zik_get_volume (zik));

  g_print ("\ntrack metadata\n");
  g_print ("  playing                : %s\n", metadata_playing ? "yes" : "no");
  g_print ("  title                  : %s\n", metadata_title);
  g_print ("  artist                 : %s\n", metadata_artist);
  g_print ("  album                  : %s\n", metadata_album);
  g_print ("  genre                  : %s\n", metadata_genre);

  g_print ("\nsoftware:\n");
  g_print ("  software version       : %s\n", zik_get_software_version (zik));

  g_print ("\nsystem:\n");
  g_print ("  battery state          : %s (remaining: %u%%)\n",
      zik_get_battery_state (zik), zik_get_battery_percentage (zik));

  if (IS_ZIK2 (zik))
    g_print ("  color                  : %s\n",
        color_str (zik2_get_color (ZIK2_CAST (zik))));

  g_print ("  flight mode            : %s\n",
      zik_is_flight_mode_active (zik) ? "on" : "off");
  g_print ("  head detection         : %s\n",
      zik_is_head_detection_active (zik) ? "on" : "off");
  g_print ("  serial-number          : %s\n", zik_get_serial (zik));
  g_print ("  friendlyname           : %s\n", zik_get_friendlyname (zik));
  g_print ("  auto-connection        : %s\n",
      zik_is_auto_connection_active (zik) ? "on" : "off");

  if (auto_power_off_timeout > 0)
    g_print ("  auto power off timeout : %u minutes\n", auto_power_off_timeout);
  else
    g_print ("  auto power off timeout : off\n");

  g_print ("  text-to-speech         : %s\n",
      zik_is_tts_active (zik) ? "on" : "off");
}

static gboolean
set_boolean_property_from_string (Zik * zik, const gchar * property,
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

  g_object_set (zik, property, req_value, NULL);
  g_object_get (zik, property, &value, NULL);

  if (value != req_value)
    return FALSE;

  return TRUE;
}

static gboolean
set_noise_control_mode (Zik * zik)
{
  ZikNoiseControlMode req_mode;

  if (g_strcmp0 (noise_control_mode, "off") == 0)
    req_mode = ZIK_NOISE_CONTROL_MODE_OFF;
  else if (g_strcmp0 (noise_control_mode, "anc") == 0)
    req_mode = ZIK_NOISE_CONTROL_MODE_ANC;
  else if (g_strcmp0 (noise_control_mode, "aoc") == 0)
    req_mode = ZIK_NOISE_CONTROL_MODE_AOC;
  else
    return FALSE;

  g_print ("Setting noise control mode to %s\n", noise_control_mode);
  if (!zik_set_noise_control_mode (zik, req_mode)) {
    g_printerr ("failed to set noise control mode to %s\n", noise_control_mode);
    return FALSE;
  }

  return TRUE;
}

static gboolean
set_noise_control_strength (Zik * zik)
{
  guint req_value = noise_control_strength;

  g_print ("Setting noise control strength to %u\n", req_value);
  if (!zik_set_noise_control_strength (zik, req_value)) {
    g_printerr ("failed to set noise control strength to %u\n", req_value);
    return FALSE;
  }

  return TRUE;
}

static gboolean
set_friendly_name (Zik * zik)
{
  gchar *req_value = friendlyname;

  g_print ("Setting friendlyname to '%s'\n", req_value);
  if (!zik_set_friendlyname (zik, req_value)) {
     g_printerr ("failed to set friendlyname to %s\n", req_value);
     return FALSE;
  }

  return TRUE;
}

static gboolean
set_sound_effect_room (Zik * zik)
{
  ZikSoundEffectRoom req_mode;

  req_mode = zik_sound_effect_room_from_string (sound_effect_room);
  if (req_mode == ZIK_SOUND_EFFECT_ROOM_UNKNOWN)
    return FALSE;

  g_print ("Setting sound effect room to %s\n", sound_effect_room);
  if (!zik_set_sound_effect_room (zik, req_mode)) {
    g_printerr ("failed to set sound effect room to %s\n", sound_effect_room);
    return FALSE;
  }

  return TRUE;
}

static gboolean
set_sound_effect_angle (Zik * zik)
{
  guint req_value = sound_effect_angle;

  g_print ("Setting sound_effect_angle to %u\n", sound_effect_angle);
  if (!zik_set_sound_effect_angle (zik, req_value)) {
    g_printerr ("failed to set sound effect angle to %u\n", sound_effect_angle);
    return FALSE;
  }

  return TRUE;
}

static void
on_zik_connected (ZikProfile * bprofile, Zik * zik, gpointer userdata)
{
  gchar *name;
  guint i;

  g_object_get (zik, "name", &name, NULL);
  g_print ("connected to %s\n", name);
  g_free (name);

  /* process set request from user */
  if (noise_control_switch) {
    g_print ("Setting noise control to %s\n", noise_control_switch);
    if (!set_boolean_property_from_string (zik, "noise-control",
          noise_control_switch))
      g_printerr ("Failed to set noise control\n");
  }

  if (noise_control_mode)
    set_noise_control_mode (zik);

  if (noise_control_strength)
    set_noise_control_strength (zik);

  if (head_detection_switch) {
    g_print ("Setting head detection to %s\n", head_detection_switch);
    if (!set_boolean_property_from_string (zik, "head-detection",
          head_detection_switch))
      g_printerr ("Failed to set head detection\n");
  }

  if (friendlyname)
    set_friendly_name (zik);

  if (sound_effect_switch) {
    g_print ("Setting sound effect to %s\n", sound_effect_switch);
    if (!set_boolean_property_from_string (zik, "sound-effect",
          sound_effect_switch))
      g_printerr ("Failed to set sound effect\n");
  }

  if (sound_effect_room)
    set_sound_effect_room (zik);

  if (sound_effect_angle > 0)
    set_sound_effect_angle (zik);

  if (auto_connection_switch) {
    g_print ("Setting auto-connection to %s\n", auto_connection_switch);
    if (!set_boolean_property_from_string (zik, "auto-connection",
          auto_connection_switch))
      g_printerr ("Failed to set auto-connection\n");
  }

  if (equalizer_switch) {
    g_print ("Setting equalizer to %s\n", equalizer_switch);
    if (!set_boolean_property_from_string (zik, "equalizer", equalizer_switch))
      g_printerr ("Failed to set equalizer\n");
  }

  if (smart_audio_tune_switch) {
    g_print ("Setting smart audio tune to %s\n", smart_audio_tune_switch);
    if (!set_boolean_property_from_string (zik, "smart-audio-tune",
          smart_audio_tune_switch))
      g_printerr ("Failed to set smart audio tune\n");
  }

  if (auto_power_off_timeout != -1)
    zik_set_auto_power_off_timeout (zik, auto_power_off_timeout);

  if (tts_switch) {
    g_print ("Setting text-to-speech to %s\n", tts_switch);
    if (!set_boolean_property_from_string (zik, "tts", tts_switch))
      g_printerr ("Failed to set text-to-speech\n");
  }

  if (auto_noise_control_switch) {
    g_print ("Setting auto noise control to %s\n", auto_noise_control_switch);
    if (!set_boolean_property_from_string (zik, "auto-noise-control",
          auto_noise_control_switch))
      g_printerr ("Failed to set automatic noise control\n");
  }

  if (flight_mode_switch) {
    /* enabling flight mode reset connection so don't send request */
    g_print ("Setting flight mode to %s\n", flight_mode_switch);
    if (!set_boolean_property_from_string (zik, "flight-mode",
          flight_mode_switch))
      g_printerr ("Failed to set flight mode\n");
  } else if (dump_api_xml) {
    for (i = 0; zik2_api[i].name != NULL; i++) {
      g_print ("- %s\n", zik2_api[i].name);
      custom_request (zik, zik2_api[i].get_uri, "get", NULL);
    }
  } else if (request_path) {
    custom_request (zik, request_path, request_method, request_args);
  } else {
    show_zik (zik);
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


static void
setup_profile (GDBusObjectManager * manager, ZikProfile * profile)
{
  g_signal_connect (profile, "zik-connected", G_CALLBACK (on_zik_connected),
      manager);

  zik_profile_install (profile, manager);
}

void
cleanup_profile (ZikProfile * profile, GDBusObjectManager * manager)
{
  zik_profile_uninstall (profile);
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

  if (auto_noise_control_switch)
    ret = check_switch_argument (auto_noise_control_switch,
        "set-auto-noise-control");

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
  GSList *zik_devices = NULL;

  context = g_option_context_new ("- control Zik2/Zik3 settings");
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

  zik_devices = zik_device_list_new (manager);
  if (zik_devices == NULL) {
    g_print ("No Zik has been paired, please pair them first\n");
    goto out;
  }

  if (list_devices) {
    /* display a list of paired Zik */
    GSList *walk;

    g_print ("List of Zik paired:\n");

    for (walk = zik_devices; walk != NULL; walk = g_slist_next (walk)) {
      BluetoothDevice1 *device = BLUETOOTH_DEVICE1 (walk->data);

      g_print ("%s: %s\n", bluetooth_device1_get_name (device),
          bluetooth_device1_get_address (device));
    }
  } else {
    BluetoothDevice1 *device;
    ZikProfile *profile;
    const gchar *profile_uuid;

    if (device_addr) {
      device = lookup_device_by_addr (zik_devices, device_addr);
      if (device == NULL) {
        g_printerr ("device '%s' not found\n", device_addr);
        goto out;
      }
    } else if (g_slist_length (zik_devices) == 1) {
      device = BLUETOOTH_DEVICE1 (zik_devices->data);
    } else {
      /* there is more than one zik and no device specified, error out */
      g_print ("more than 1 device are paired, please select one\n");
      goto out;
    }

    /* make profile according to device type */
    if (is_zik2_device (device)) {
      profile = (ZikProfile *) zik2_profile_new ();
      profile_uuid = ZIK2_PROFILE_UUID;
    } else if (is_zik3_device (device)) {
      profile = (ZikProfile *) zik3_profile_new ();
      profile_uuid = ZIK3_PROFILE_UUID;
    } else {
      g_assert_not_reached ();
    }

    setup_profile (manager, profile);

    /* connect asynchronously to profile as it is handled by this application
     * so through the main loop */
    g_print ("connecting to Zik device '%s'\n",
        bluetooth_device1_get_name (device));
    bluetooth_device1_call_connect_profile (device, profile_uuid, NULL,
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

  if (zik_devices)
    g_slist_free_full (zik_devices, g_object_unref);

  g_option_context_free (context);

  return ret;
}
