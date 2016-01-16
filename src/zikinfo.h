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

#ifndef ZIK_INFO_H
#define ZIK_INFO_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

#define ZIK_ANSWER_INFO_TYPE (zik_answer_info_get_type ())
#define ZIK_AUDIO_INFO_TYPE (zik_audio_info_get_type ())
#define ZIK_SOFTWARE_INFO_TYPE (zik_software_info_get_type ())
#define ZIK_SYSTEM_INFO_TYPE (zik_system_info_get_type ())
#define ZIK_NOISE_CONTROL_INFO_TYPE (zik_noise_control_info_get_type ())
#define ZIK_SOURCE_INFO_TYPE (zik_source_info_get_type ())
#define ZIK_BATTERY_INFO_TYPE (zik_battery_info_get_type ())
#define ZIK_VOLUME_INFO_TYPE (zik_volume_info_get_type ())
#define ZIK_HEAD_DETECTION_INFO_TYPE (zik_head_detection_info_get_type ())
#define ZIK_COLOR_INFO_TYPE (zik_color_info_get_type ())
#define ZIK_FLIGHT_MODE_INFO_TYPE (zik_flight_mode_info_get_type ())
#define ZIK_BLUETOOTH_INFO_TYPE (zik_bluetooth_info_get_type ())
#define ZIK_SOUND_EFFECT_INFO_TYPE (zik_sound_effect_info_get_type ())
#define ZIK_AUTO_CONNECTION_INFO_TYPE (zik_auto_connection_info_get_type ())
#define ZIK_TRACK_INFO_TYPE (zik_track_info_get_type ())
#define ZIK_METADATA_INFO_TYPE (zik_metadata_info_get_type ())
#define ZIK_EQUALIZER_INFO_TYPE (zik_equalizer_info_get_type ())
#define ZIK_SMART_AUDIO_TUNE_INFO_TYPE (zik_smart_audio_tune_info_get_type ())
#define ZIK_AUTO_POWER_OFF_INFO_TYPE (zik_auto_power_off_info_get_type ())
#define ZIK_TTS_INFO_TYPE (zik_tts_info_get_type ())

typedef struct _ZikAnswerInfo ZikAnswerInfo;
typedef struct _ZikAudioInfo ZikAudioInfo;
typedef struct _ZikSoftwareInfo ZikSoftwareInfo;
typedef struct _ZikSystemInfo ZikSystemInfo;
typedef struct _ZikNoiseControlInfo ZikNoiseControlInfo;
typedef struct _ZikSourceInfo ZikSourceInfo;
typedef struct _ZikBatteryInfo ZikBatteryInfo;
typedef struct _ZikVolumeInfo ZikVolumeInfo;
typedef struct _ZikHeadDetectionInfo ZikHeadDetectionInfo;
typedef struct _ZikColorInfo ZikColorInfo;
typedef struct _ZikFlightModeInfo ZikFlightModeInfo;
typedef struct _ZikBluetoothInfo ZikBluetoothInfo;
typedef struct _ZikSoundEffectInfo ZikSoundEffectInfo;
typedef struct _ZikAutoConnectionInfo ZikAutoConnectionInfo;
typedef struct _ZikTrackInfo ZikTrackInfo;
typedef struct _ZikMetadataInfo ZikMetadataInfo;
typedef struct _ZikEqualizerInfo ZikEqualizerInfo;
typedef struct _ZikSmartAudioTuneInfo ZikSmartAudioTuneInfo;
typedef struct _ZikAutoPowerOffInfo ZikAutoPowerOffInfo;
typedef struct _ZikTTSInfo ZikTTSInfo;

/* all nodes structures shall begin with:
 * GType itype
 */
struct _ZikAnswerInfo
{
  GType itype;
  gint ref_count;

  gchar *path;
  gboolean error;
};

struct _ZikAudioInfo
{
  GType itype;
  gint ref_count;
};

struct _ZikSoftwareInfo
{
  GType itype;
  gint ref_count;

  gchar *sip6;
  gchar *pic;
  gchar *tts;
};

struct _ZikSystemInfo
{
  GType itype;
  gint ref_count;

  gchar *pi;
};

struct _ZikNoiseControlInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
  gchar *type;
  guint value;
};

struct _ZikSourceInfo
{
  GType itype;
  gint ref_count;

  gchar *type;
};

struct _ZikBatteryInfo
{
  GType itype;
  gint ref_count;

  gchar *state;
  guint percent;
};

struct _ZikVolumeInfo
{
  GType itype;
  gint ref_count;

  guint volume;
};

struct _ZikHeadDetectionInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
};

struct _ZikColorInfo
{
  GType itype;
  gint ref_count;

  guint value;
};

struct _ZikFlightModeInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
};

struct _ZikBluetoothInfo
{
  GType itype;
  gint ref_count;

  gchar *friendlyname;
};

struct _ZikSoundEffectInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
  gchar *room_size;
  guint angle;
};

struct _ZikAutoConnectionInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
};

struct _ZikTrackInfo
{
  GType itype;
  gint ref_count;
};

struct _ZikMetadataInfo
{
  GType itype;
  gint ref_count;

  gboolean playing;
  gchar *title;
  gchar *artist;
  gchar *album;
  gchar *genre;
};

struct _ZikEqualizerInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
};

struct _ZikSmartAudioTuneInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
};

struct _ZikAutoPowerOffInfo
{
  GType itype;
  gint ref_count;

  guint value;
};

struct _ZikTTSInfo
{
  GType itype;
  gint ref_count;

  gboolean enabled;
};

ZikAnswerInfo *zik_answer_info_new (const gchar * path, gboolean error);
ZikAnswerInfo *zik_answer_info_ref (ZikAnswerInfo * info);
void zik_answer_info_unref (ZikAnswerInfo * info);

ZikAudioInfo *zik_audio_info_new (void);
ZikAudioInfo *zik_audio_info_ref (ZikAudioInfo * info);
void zik_audio_info_unref (ZikAudioInfo * info);

ZikSoftwareInfo *zik_software_info_new (const gchar * sip6, const gchar * pic,
    const gchar * tts);
ZikSoftwareInfo *zik_software_info_ref (ZikSoftwareInfo * info);
void zik_software_info_unref (ZikSoftwareInfo * info);

ZikSystemInfo *zik_system_info_new (const gchar * pi);
ZikSystemInfo *zik_system_info_ref (ZikSystemInfo * info);
void zik_system_info_unref (ZikSystemInfo * info);

ZikNoiseControlInfo * zik_noise_control_info_new (gboolean enabled,
    gchar * type, guint value);
ZikNoiseControlInfo *zik_noise_control_info_ref (ZikNoiseControlInfo * info);
void zik_noise_control_info_unref (ZikNoiseControlInfo * info);

ZikSourceInfo * zik_source_info_new (const gchar * type);
ZikSourceInfo *zik_source_info_ref (ZikSourceInfo * info);
void zik_source_info_unref (ZikSourceInfo * info);

ZikBatteryInfo *zik_battery_info_new (const gchar * state, guint percent);
ZikBatteryInfo *zik_battery_info_ref (ZikBatteryInfo * info);
void zik_battery_info_unref (ZikBatteryInfo * info);

ZikVolumeInfo *zik_volume_info_new (guint volume);
ZikVolumeInfo *zik_volume_info_ref (ZikVolumeInfo * info);
void zik_volume_info_unref (ZikVolumeInfo * info);

ZikHeadDetectionInfo *zik_head_detection_info_new (gboolean enabled);
ZikHeadDetectionInfo *zik_head_detection_info_ref (ZikHeadDetectionInfo * info);
void zik_head_detection_info_unref (ZikHeadDetectionInfo * info);

ZikColorInfo *zik_color_info_new (guint value);
ZikColorInfo *zik_color_info_ref (ZikColorInfo * info);
void zik_color_info_unref (ZikColorInfo * info);

ZikFlightModeInfo *zik_flight_mode_info_new (gboolean enabled);
ZikFlightModeInfo *zik_flight_mode_info_ref (ZikFlightModeInfo * info);
void zik_flight_mode_info_unref (ZikFlightModeInfo * info);

ZikBluetoothInfo *zik_bluetooth_info_new (const gchar * friendlyname);
ZikBluetoothInfo *zik_bluetooth_info_ref (ZikBluetoothInfo * info);
void zik_bluetooth_info_unref (ZikBluetoothInfo * info);

ZikSoundEffectInfo *zik_sound_effect_info_new (gboolean enabled,
    const gchar * room_size, guint angle);
ZikSoundEffectInfo *zik_sound_effect_info_ref (ZikSoundEffectInfo * info);
void zik_sound_effect_info_unref (ZikSoundEffectInfo * info);

ZikAutoConnectionInfo *zik_auto_connection_info_new (gboolean enabled);
ZikAutoConnectionInfo *zik_auto_connection_info_ref (ZikAutoConnectionInfo * info);
void zik_auto_connection_info_unref (ZikAutoConnectionInfo * info);

ZikTrackInfo *zik_track_info_new (void);
ZikTrackInfo *zik_track_info_ref (ZikTrackInfo * info);
void zik_track_info_unref (ZikTrackInfo * info);

ZikMetadataInfo *zik_metadata_info_new (gboolean playing, const gchar * title,
    const gchar * artist, const gchar * album, const gchar * genre);
ZikMetadataInfo *zik_metadata_info_ref (ZikMetadataInfo * info);
void zik_metadata_info_unref (ZikMetadataInfo * info);

ZikEqualizerInfo *zik_equalizer_info_new (gboolean enabled);
ZikEqualizerInfo *zik_equalizer_info_ref (ZikEqualizerInfo * info);
void zik_equalizer_info_unref (ZikEqualizerInfo * info);

ZikSmartAudioTuneInfo *zik_smart_audio_tune_info_new (gboolean enabled);
ZikSmartAudioTuneInfo *zik_smart_audio_tune_info_ref (ZikSmartAudioTuneInfo * info);
void zik_smart_audio_tune_info_unref (ZikSmartAudioTuneInfo * info);

ZikAutoPowerOffInfo *zik_auto_power_off_info_new (guint value);
ZikAutoPowerOffInfo *zik_auto_power_off_info_ref (ZikAutoPowerOffInfo * info);
void zik_auto_power_off_info_unref (ZikAutoPowerOffInfo * info);

ZikTTSInfo *zik_tts_info_new (gboolean enabled);
ZikTTSInfo *zik_tts_info_ref (ZikTTSInfo * info);
void zik_tts_info_unref (ZikTTSInfo * info);

GType zik_answer_info_get_type (void);
GType zik_audio_info_get_type (void);
GType zik_software_info_get_type (void);
GType zik_system_info_get_type (void);
GType zik_noise_control_info_get_type (void);
GType zik_source_info_get_type (void);
GType zik_battery_info_get_type (void);
GType zik_volume_info_get_type (void);
GType zik_head_detection_info_get_type (void);
GType zik_color_info_get_type (void);
GType zik_flight_mode_info_get_type (void);
GType zik_bluetooth_info_get_type (void);
GType zik_sound_effect_info_get_type (void);
GType zik_auto_connection_info_get_type (void);
GType zik_track_info_get_type (void);
GType zik_metadata_info_get_type (void);
GType zik_equalizer_info_get_type (void);
GType zik_smart_audio_tune_info_get_type (void);
GType zik_auto_power_off_info_get_type (void);
GType zik_tts_info_get_type (void);

G_END_DECLS

#endif
