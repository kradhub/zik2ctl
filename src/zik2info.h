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

#ifndef ZIK2_INFO_H
#define ZIK2_INFO_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ZIK2_ANSWER_INFO_TYPE (zik2_answer_info_get_type ())
#define ZIK2_AUDIO_INFO_TYPE (zik2_audio_info_get_type ())
#define ZIK2_SOFTWARE_INFO_TYPE (zik2_software_info_get_type ())
#define ZIK2_SYSTEM_INFO_TYPE (zik2_system_info_get_type ())
#define ZIK2_NOISE_CONTROL_INFO_TYPE (zik2_noise_control_info_get_type ())
#define ZIK2_SOURCE_INFO_TYPE (zik2_source_info_get_type ())
#define ZIK2_BATTERY_INFO_TYPE (zik2_battery_info_get_type ())
#define ZIK2_VOLUME_INFO_TYPE (zik2_volume_info_get_type ())
#define ZIK2_HEAD_DETECTION_INFO_TYPE (zik2_head_detection_info_get_type ())
#define ZIK2_COLOR_INFO_TYPE (zik2_color_info_get_type ())
#define ZIK2_FLIGHT_MODE_INFO_TYPE (zik2_flight_mode_info_get_type ())
#define ZIK2_BLUETOOTH_INFO_TYPE (zik2_bluetooth_info_get_type ())
#define ZIK2_SOUND_EFFECT_INFO_TYPE (zik2_sound_effect_info_get_type ())
#define ZIK2_AUTO_CONNECTION_INFO_TYPE (zik2_auto_connection_info_get_type ())

typedef struct _Zik2AnswerInfo Zik2AnswerInfo;
typedef struct _Zik2AudioInfo Zik2AudioInfo;
typedef struct _Zik2SoftwareInfo Zik2SoftwareInfo;
typedef struct _Zik2SystemInfo Zik2SystemInfo;
typedef struct _Zik2NoiseControlInfo Zik2NoiseControlInfo;
typedef struct _Zik2SourceInfo Zik2SourceInfo;
typedef struct _Zik2BatteryInfo Zik2BatteryInfo;
typedef struct _Zik2VolumeInfo Zik2VolumeInfo;
typedef struct _Zik2HeadDetectionInfo Zik2HeadDetectionInfo;
typedef struct _Zik2ColorInfo Zik2ColorInfo;
typedef struct _Zik2FlightModeInfo Zik2FlightModeInfo;
typedef struct _Zik2BluetoothInfo Zik2BluetoothInfo;
typedef struct _Zik2SoundEffectInfo Zik2SoundEffectInfo;
typedef struct _Zik2AutoConnectionInfo Zik2AutoConnectionInfo;

/* all nodes structures shall begin with:
 * GType itype
 */
struct _Zik2AnswerInfo
{
  GType itype;

  gchar *path;
  gboolean error;
};

struct _Zik2AudioInfo
{
  GType itype;
};

struct _Zik2SoftwareInfo
{
  GType itype;

  gchar *sip6;
  gchar *pic;
  gchar *tts;
};

struct _Zik2SystemInfo
{
  GType itype;

  gchar *pi;
};

struct _Zik2NoiseControlInfo
{
  GType itype;

  gboolean enabled;
  gchar *type;
  guint value;
};

struct _Zik2SourceInfo
{
  GType itype;

  gchar *type;
};

struct _Zik2BatteryInfo
{
  GType itype;

  gchar *state;
  guint percent;
};

struct _Zik2VolumeInfo
{
  GType itype;

  guint volume;
};

struct _Zik2HeadDetectionInfo
{
  GType itype;

  gboolean enabled;
};

struct _Zik2ColorInfo
{
  GType itype;

  guint value;
};

struct _Zik2FlightModeInfo
{
  GType itype;

  gboolean enabled;
};

struct _Zik2BluetoothInfo
{
  GType itype;

  gchar *friendlyname;
};

struct _Zik2SoundEffectInfo
{
  GType itype;

  gboolean enabled;
  gchar *room_size;
  guint angle;
};

struct _Zik2AutoConnectionInfo
{
  GType itype;

  gboolean enabled;
};

Zik2AnswerInfo *zik2_answer_info_new (const gchar * path, gboolean error);
void zik2_answer_info_free (Zik2AnswerInfo * info);

Zik2AudioInfo *zik2_audio_info_new (void);
void zik2_audio_info_free (Zik2AudioInfo * info);

Zik2SoftwareInfo *zik2_software_info_new (const gchar * sip6, const gchar * pic,
    const gchar * tts);
void zik2_software_info_free (Zik2SoftwareInfo * info);

Zik2SystemInfo *zik2_system_info_new (const gchar * pi);
void zik2_system_info_free (Zik2SystemInfo * info);

Zik2NoiseControlInfo * zik2_noise_control_info_new (gboolean enabled,
    gchar * type, guint value);
void zik2_noise_control_info_free (Zik2NoiseControlInfo * info);

Zik2SourceInfo * zik2_source_info_new (const gchar * type);
void zik2_source_info_free (Zik2SourceInfo * info);

Zik2BatteryInfo *zik2_battery_info_new (const gchar * state, guint percent);
void zik2_battery_info_free (Zik2BatteryInfo * info);

Zik2VolumeInfo *zik2_volume_info_new (guint volume);
void zik2_volume_info_free (Zik2VolumeInfo * info);

Zik2HeadDetectionInfo *zik2_head_detection_info_new (gboolean enabled);
void zik2_head_detection_info_free (Zik2HeadDetectionInfo * info);

Zik2ColorInfo *zik2_color_info_new (guint value);
void zik2_color_info_free (Zik2ColorInfo * info);

Zik2FlightModeInfo *zik2_flight_mode_info_new (gboolean enabled);
void zik2_flight_mode_info_free (Zik2FlightModeInfo * info);

Zik2BluetoothInfo *zik2_bluetooth_info_new (const gchar * friendlyname);
void zik2_bluetooth_info_free (Zik2BluetoothInfo * info);

Zik2SoundEffectInfo *zik2_sound_effect_info_new (gboolean enabled,
    const gchar * room_size, guint angle);
void zik2_sound_effect_info_free (Zik2SoundEffectInfo * info);

Zik2AutoConnectionInfo *zik2_auto_connection_info_new (gboolean enabled);
void zik2_auto_connection_info_free (Zik2AutoConnectionInfo * info);

GType zik2_answer_info_get_type (void);
GType zik2_audio_info_get_type (void);
GType zik2_software_info_get_type (void);
GType zik2_system_info_get_type (void);
GType zik2_noise_control_info_get_type (void);
GType zik2_source_info_get_type (void);
GType zik2_battery_info_get_type (void);
GType zik2_volume_info_get_type (void);
GType zik2_head_detection_info_get_type (void);
GType zik2_color_info_get_type (void);
GType zik2_flight_mode_info_get_type (void);
GType zik2_bluetooth_info_get_type (void);
GType zik2_sound_effect_info_get_type (void);
GType zik2_auto_connection_info_get_type (void);

G_END_DECLS

#endif
