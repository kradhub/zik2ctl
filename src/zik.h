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

#ifndef ZIK_H
#define ZIK_H

#include <glib.h>
#include <glib-object.h>
#include "zikconnection.h"

G_BEGIN_DECLS

#define ZIK_TYPE (zik_get_type ())
#define ZIK(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ZIK_TYPE, Zik))
#define ZIK_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), ZIK_TYPE, ZikClass))

typedef enum _ZikNoiseControlMode ZikNoiseControlMode;
typedef enum _ZikSoundEffectRoom ZikSoundEffectRoom;
typedef enum _ZikSoundEffectAngle ZikSoundEffectAngle;

typedef struct _ZikClass ZikClass;
typedef struct _Zik Zik;
typedef struct _ZikPrivate ZikPrivate;

enum _ZikNoiseControlMode
{
  ZIK_NOISE_CONTROL_MODE_OFF,
  ZIK_NOISE_CONTROL_MODE_ANC,
  ZIK_NOISE_CONTROL_MODE_AOC
};

enum _ZikSoundEffectRoom
{
  ZIK_SOUND_EFFECT_ROOM_UNKNOWN,
  ZIK_SOUND_EFFECT_ROOM_SILENT,
  ZIK_SOUND_EFFECT_ROOM_LIVING,
  ZIK_SOUND_EFFECT_ROOM_JAZZ,
  ZIK_SOUND_EFFECT_ROOM_CONCERT
};

enum _ZikSoundEffectAngle
{
  ZIK_SOUND_EFFECT_ANGLE_UNKNOWN = 0,
  ZIK_SOUND_EFFECT_ANGLE_30 = 30,
  ZIK_SOUND_EFFECT_ANGLE_60 = 60,
  ZIK_SOUND_EFFECT_ANGLE_90 = 90,
  ZIK_SOUND_EFFECT_ANGLE_120 = 120,
  ZIK_SOUND_EFFECT_ANGLE_150 = 150,
  ZIK_SOUND_EFFECT_ANGLE_180 = 180
};

struct _Zik
{
  GObject parent;

  ZikPrivate *priv;
};

struct _ZikClass
{
  GObjectClass parent_class;
};

ZikSoundEffectRoom zik_sound_effect_room_from_string (const gchar * str);
const gchar *zik_sound_effect_room_name (ZikSoundEffectRoom room);

GType zik_get_type (void);

const gchar *zik_get_name (Zik * zik);
const gchar *zik_get_address (Zik * zik);
ZikConnection *zik_get_connection (Zik * zik);

/* audio */
gboolean zik_is_noise_control_active (Zik * zik);
gboolean zik_set_noise_control_active (Zik * zik, gboolean active);

ZikNoiseControlMode zik_get_noise_control_mode (Zik * zik);
gboolean zik_set_noise_control_mode (Zik * zik, ZikNoiseControlMode mode);

guint zik_get_noise_control_strength (Zik * zik);
gboolean zik_set_noise_control_strength (Zik * zik, guint strength);

const gchar *zik_get_source (Zik * zik);
guint zik_get_volume (Zik * zik);

gboolean zik_is_sound_effect_active (Zik * zik);
gboolean zik_set_sound_effect_active (Zik * zik, gboolean active);

ZikSoundEffectRoom zik_get_sound_effect_room (Zik * zik);
gboolean zik_set_sound_effect_room (Zik * zik, ZikSoundEffectRoom room);

ZikSoundEffectAngle zik_get_sound_effect_angle (Zik * zik);
gboolean zik_set_sound_effect_angle (Zik * zik, ZikSoundEffectAngle angle);

/* software and system */
const gchar *zik_get_software_version (Zik * zik);
const gchar *zik_get_battery_state (Zik * zik);
guint zik_get_battery_percentage (Zik * zik);

gboolean zik_is_head_detection_active (Zik * zik);
gboolean zik_set_head_detection_active (Zik * zik, gboolean active);

const gchar *zik_get_serial (Zik *zik);

/* TODO: Remove when add support of Zik1 */
gboolean zik_is_flight_mode_active (Zik * zik);
gboolean zik_set_flight_mode_active (Zik * zik, gboolean active);

const gchar *zik_get_friendlyname (Zik * zik);
gboolean zik_set_friendlyname (Zik * zik, const gchar * name);

gboolean zik_is_auto_connection_active (Zik * zik);
gboolean zik_set_auto_connection_active (Zik * zik, gboolean active);

void zik_get_track_metadata (Zik * zik, gboolean * playing,
    const gchar ** title, const gchar ** artist, const gchar ** album,
    const gchar ** genre);

gboolean zik_is_equalizer_active (Zik * zik);
gboolean zik_set_equalizer_active (Zik * zik, gboolean active);

gboolean zik_is_smart_audio_tune_active (Zik * zik);
gboolean zik_set_smart_audio_tune_active (Zik * zik, gboolean active);

guint zik_get_auto_power_off_timeout (Zik * zik);
gboolean zik_set_auto_power_off_timeout (Zik * zik, guint timeout_min);

gboolean zik_is_tts_active (Zik * zik);
gboolean zik_set_tts_active (Zik * zik, gboolean active);

/* helpers */
gboolean zik_do_request (Zik * zik, const gchar * path, const gchar * method,
    const gchar * args, ZikRequestReplyData ** reply_data);
gpointer zik_request_info (Zik * zik, const gchar * path, GType type);
void zik_sync_static_properties (Zik * zik);

G_END_DECLS

#endif
