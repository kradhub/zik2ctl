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

#ifndef ZIK2_H
#define ZIK2_H

#include <glib.h>
#include <glib-object.h>
#include "zik2connection.h"

G_BEGIN_DECLS

#define ZIK2_TYPE (zik2_get_type ())
#define ZIK2(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ZIK2_TYPE, Zik2))
#define ZIK2_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ZIK2_TYPE, Zik2Class))

typedef enum _Zik2Color Zik2Color;
typedef enum _Zik2NoiseControlMode Zik2NoiseControlMode;
typedef enum _Zik2SoundEffectRoom Zik2SoundEffectRoom;
typedef enum _Zik2SoundEffectAngle Zik2SoundEffectAngle;

typedef struct _Zik2Class Zik2Class;
typedef struct _Zik2 Zik2;
typedef struct _Zik2Private Zik2Private;

enum _Zik2Color
{
  ZIK2_COLOR_UNKNOWN = 0x0,
  ZIK2_COLOR_BLACK = 0x1,
  ZIK2_COLOR_BLUE = 0x2,
};

enum _Zik2NoiseControlMode
{
  ZIK2_NOISE_CONTROL_MODE_OFF,
  ZIK2_NOISE_CONTROL_MODE_ANC,
  ZIK2_NOISE_CONTROL_MODE_AOC
};

enum _Zik2SoundEffectRoom
{
  ZIK2_SOUND_EFFECT_ROOM_UNKNOWN,
  ZIK2_SOUND_EFFECT_ROOM_SILENT,
  ZIK2_SOUND_EFFECT_ROOM_LIVING,
  ZIK2_SOUND_EFFECT_ROOM_JAZZ,
  ZIK2_SOUND_EFFECT_ROOM_CONCERT
};

enum _Zik2SoundEffectAngle
{
  ZIK2_SOUND_EFFECT_ANGLE_UNKNOWN = 0,
  ZIK2_SOUND_EFFECT_ANGLE_30 = 30,
  ZIK2_SOUND_EFFECT_ANGLE_60 = 60,
  ZIK2_SOUND_EFFECT_ANGLE_90 = 90,
  ZIK2_SOUND_EFFECT_ANGLE_120 = 120,
  ZIK2_SOUND_EFFECT_ANGLE_150 = 150,
  ZIK2_SOUND_EFFECT_ANGLE_180 = 180
};

struct _Zik2
{
  GObject parent;

  Zik2Connection *conn;

  Zik2Private *priv;
};

struct _Zik2Class
{
  GObjectClass parent_class;
};

Zik2SoundEffectRoom zik2_sound_effect_room_from_string (const gchar * str);
const gchar *zik2_sound_effect_room_name (Zik2SoundEffectRoom room);

GType zik2_get_type (void);
Zik2 *zik2_new (const gchar * name, const gchar * address, Zik2Connection * conn);

const gchar *zik2_get_name (Zik2 * zik2);
const gchar *zik2_get_address (Zik2 * zik2);

/* audio */
gboolean zik2_is_noise_control_active (Zik2 * zik2);
gboolean zik2_set_noise_control_active (Zik2 * zik2, gboolean active);

Zik2NoiseControlMode zik2_get_noise_control_mode (Zik2 * zik2);
gboolean zik2_set_noise_control_mode (Zik2 * zik2, Zik2NoiseControlMode mode);

guint zik2_get_noise_control_strength (Zik2 * zik2);
gboolean zik2_set_noise_control_strength (Zik2 * zik2, guint strength);

const gchar *zik2_get_source (Zik2 * zik2);
guint zik2_get_volume (Zik2 * zik2);

gboolean zik2_is_sound_effect_active (Zik2 * zik2);
gboolean zik2_set_sound_effect_active (Zik2 * zik2, gboolean active);

Zik2SoundEffectRoom zik2_get_sound_effect_room (Zik2 * zik2);
gboolean zik2_set_sound_effect_room (Zik2 * zik2, Zik2SoundEffectRoom room);

Zik2SoundEffectAngle zik2_get_sound_effect_angle (Zik2 * zik2);
gboolean zik2_set_sound_effect_angle (Zik2 * zik2, Zik2SoundEffectAngle angle);

/* software and system */
const gchar *zik2_get_software_version (Zik2 * zik2);
const gchar *zik2_get_battery_state (Zik2 * zik2);
guint zik2_get_battery_percentage (Zik2 * zik2);
Zik2Color zik2_get_color (Zik2 * zik2);

gboolean zik2_is_head_detection_active (Zik2 * zik2);
gboolean zik2_set_head_detection_active (Zik2 * zik2, gboolean active);

const gchar *zik2_get_serial (Zik2 *zik2);

gboolean zik2_is_flight_mode_active (Zik2 * zik2);
gboolean zik2_set_flight_mode_active (Zik2 * zik2, gboolean active);

const gchar *zik2_get_friendlyname (Zik2 * zik2);
gboolean zik2_set_friendlyname (Zik2 * zik2, const gchar * name);

gboolean zik2_is_auto_connection_active (Zik2 * zik2);
gboolean zik2_set_auto_connection_active (Zik2 * zik2, gboolean active);

void zik2_get_track_metadata (Zik2 * zik2, gboolean * playing,
    const gchar ** title, const gchar ** artist, const gchar ** album,
    const gchar ** genre);

gboolean zik2_is_equalizer_active (Zik2 * zik2);
gboolean zik2_set_equalizer_active (Zik2 * zik2, gboolean active);

gboolean zik2_is_smart_audio_tune_active (Zik2 * zik2);
gboolean zik2_set_smart_audio_tune_active (Zik2 * zik2, gboolean active);

guint zik2_get_auto_power_off_timeout (Zik2 * zik2);
gboolean zik2_set_auto_power_off_timeout (Zik2 * zik2, guint timeout_min);

G_END_DECLS

#endif
