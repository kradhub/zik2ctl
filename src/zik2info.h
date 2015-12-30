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
#define ZIK2_TRACK_INFO_TYPE (zik2_track_info_get_type ())
#define ZIK2_METADATA_INFO_TYPE (zik2_metadata_info_get_type ())
#define ZIK2_EQUALIZER_INFO_TYPE (zik2_equalizer_info_get_type ())
#define ZIK2_SMART_AUDIO_TUNE_INFO_TYPE (zik2_smart_audio_tune_info_get_type ())
#define ZIK2_AUTO_POWER_OFF_INFO_TYPE (zik2_auto_power_off_info_get_type ())

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
typedef struct _Zik2TrackInfo Zik2TrackInfo;
typedef struct _Zik2MetadataInfo Zik2MetadataInfo;
typedef struct _Zik2EqualizerInfo Zik2EqualizerInfo;
typedef struct _Zik2SmartAudioTuneInfo Zik2SmartAudioTuneInfo;
typedef struct _Zik2AutoPowerOffInfo Zik2AutoPowerOffInfo;

/* all nodes structures shall begin with:
 * GType itype
 */
struct _Zik2AnswerInfo
{
  GType itype;
  gint ref_count;

  gchar *path;
  gboolean error;
};

struct _Zik2AudioInfo
{
  GType itype;
  gint ref_count;
};

struct _Zik2SoftwareInfo
{
  GType itype;
  gint ref_count;

  gchar *sip6;
  gchar *pic;
  gchar *tts;
};

struct _Zik2SystemInfo
{
  GType itype;
  gint ref_count;

  gchar *pi;
};

struct _Zik2NoiseControlInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
  gchar *type;
  guint value;
};

struct _Zik2SourceInfo
{
  GType itype;
  gint ref_count;

  gchar *type;
};

struct _Zik2BatteryInfo
{
  GType itype;
  gint ref_count;

  gchar *state;
  guint percent;
};

struct _Zik2VolumeInfo
{
  GType itype;
  gint ref_count;

  guint volume;
};

struct _Zik2HeadDetectionInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
};

struct _Zik2ColorInfo
{
  GType itype;
  gint ref_count;

  guint value;
};

struct _Zik2FlightModeInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
};

struct _Zik2BluetoothInfo
{
  GType itype;
  gint ref_count;

  gchar *friendlyname;
};

struct _Zik2SoundEffectInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
  gchar *room_size;
  guint angle;
};

struct _Zik2AutoConnectionInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
};

struct _Zik2TrackInfo
{
  GType itype;
  gint ref_count;
};

struct _Zik2MetadataInfo
{
  GType itype;
  gint ref_count;

  gboolean playing;
  gchar *title;
  gchar *artist;
  gchar *album;
  gchar *genre;
};

struct _Zik2EqualizerInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
};

struct _Zik2SmartAudioTuneInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
};

struct _Zik2AutoPowerOffInfo
{
  GType itype;
  gint ref_count;

  guint value;
};

Zik2AnswerInfo *zik2_answer_info_new (const gchar * path, gboolean error);
Zik2AnswerInfo *zik2_answer_info_ref (Zik2AnswerInfo * info);
void zik2_answer_info_unref (Zik2AnswerInfo * info);

Zik2AudioInfo *zik2_audio_info_new (void);
Zik2AudioInfo *zik2_audio_info_ref (Zik2AudioInfo * info);
void zik2_audio_info_unref (Zik2AudioInfo * info);

Zik2SoftwareInfo *zik2_software_info_new (const gchar * sip6, const gchar * pic,
    const gchar * tts);
Zik2SoftwareInfo *zik2_software_info_ref (Zik2SoftwareInfo * info);
void zik2_software_info_unref (Zik2SoftwareInfo * info);

Zik2SystemInfo *zik2_system_info_new (const gchar * pi);
Zik2SystemInfo *zik2_system_info_ref (Zik2SystemInfo * info);
void zik2_system_info_unref (Zik2SystemInfo * info);

Zik2NoiseControlInfo * zik2_noise_control_info_new (gboolean enabled,
    gchar * type, guint value);
Zik2NoiseControlInfo *zik2_noise_control_info_ref (Zik2NoiseControlInfo * info);
void zik2_noise_control_info_unref (Zik2NoiseControlInfo * info);

Zik2SourceInfo * zik2_source_info_new (const gchar * type);
Zik2SourceInfo *zik2_source_info_ref (Zik2SourceInfo * info);
void zik2_source_info_unref (Zik2SourceInfo * info);

Zik2BatteryInfo *zik2_battery_info_new (const gchar * state, guint percent);
Zik2BatteryInfo *zik2_battery_info_ref (Zik2BatteryInfo * info);
void zik2_battery_info_unref (Zik2BatteryInfo * info);

Zik2VolumeInfo *zik2_volume_info_new (guint volume);
Zik2VolumeInfo *zik2_volume_info_ref (Zik2VolumeInfo * info);
void zik2_volume_info_unref (Zik2VolumeInfo * info);

Zik2HeadDetectionInfo *zik2_head_detection_info_new (gboolean enabled);
Zik2HeadDetectionInfo *zik2_head_detection_info_ref (Zik2HeadDetectionInfo * info);
void zik2_head_detection_info_unref (Zik2HeadDetectionInfo * info);

Zik2ColorInfo *zik2_color_info_new (guint value);
Zik2ColorInfo *zik2_color_info_ref (Zik2ColorInfo * info);
void zik2_color_info_unref (Zik2ColorInfo * info);

Zik2FlightModeInfo *zik2_flight_mode_info_new (gboolean enabled);
Zik2FlightModeInfo *zik2_flight_mode_info_ref (Zik2FlightModeInfo * info);
void zik2_flight_mode_info_unref (Zik2FlightModeInfo * info);

Zik2BluetoothInfo *zik2_bluetooth_info_new (const gchar * friendlyname);
Zik2BluetoothInfo *zik2_bluetooth_info_ref (Zik2BluetoothInfo * info);
void zik2_bluetooth_info_unref (Zik2BluetoothInfo * info);

Zik2SoundEffectInfo *zik2_sound_effect_info_new (gboolean enabled,
    const gchar * room_size, guint angle);
Zik2SoundEffectInfo *zik2_sound_effect_info_ref (Zik2SoundEffectInfo * info);
void zik2_sound_effect_info_unref (Zik2SoundEffectInfo * info);

Zik2AutoConnectionInfo *zik2_auto_connection_info_new (gboolean enabled);
Zik2AutoConnectionInfo *zik2_auto_connection_info_ref (Zik2AutoConnectionInfo * info);
void zik2_auto_connection_info_unref (Zik2AutoConnectionInfo * info);

Zik2TrackInfo *zik2_track_info_new (void);
Zik2TrackInfo *zik2_track_info_ref (Zik2TrackInfo * info);
void zik2_track_info_unref (Zik2TrackInfo * info);

Zik2MetadataInfo *zik2_metadata_info_new (gboolean playing, const gchar * title,
    const gchar * artist, const gchar * album, const gchar * genre);
Zik2MetadataInfo *zik2_metadata_info_ref (Zik2MetadataInfo * info);
void zik2_metadata_info_unref (Zik2MetadataInfo * info);

Zik2EqualizerInfo *zik2_equalizer_info_new (gboolean enabled);
Zik2EqualizerInfo *zik2_equalizer_info_ref (Zik2EqualizerInfo * info);
void zik2_equalizer_info_unref (Zik2EqualizerInfo * info);

Zik2SmartAudioTuneInfo *zik2_smart_audio_tune_info_new (gboolean enabled);
Zik2SmartAudioTuneInfo *zik2_smart_audio_tune_info_ref (Zik2SmartAudioTuneInfo * info);
void zik2_smart_audio_tune_info_unref (Zik2SmartAudioTuneInfo * info);

Zik2AutoPowerOffInfo *zik2_auto_power_off_info_new (guint value);
Zik2AutoPowerOffInfo *zik2_auto_power_off_info_ref (Zik2AutoPowerOffInfo * info);
void zik2_auto_power_off_info_unref (Zik2AutoPowerOffInfo * info);

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
GType zik2_track_info_get_type (void);
GType zik2_metadata_info_get_type (void);
GType zik2_equalizer_info_get_type (void);
GType zik2_smart_audio_tune_info_get_type (void);
GType zik2_auto_power_off_info_get_type (void);

G_END_DECLS

#endif
