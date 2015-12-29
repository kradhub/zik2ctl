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

#include "zik2info.h"

static Zik2AnswerInfo *zik2_answer_info_copy (Zik2AnswerInfo * info);
static Zik2AudioInfo *zik2_audio_info_copy (Zik2AudioInfo * info);
static Zik2SoftwareInfo *zik2_software_info_copy (Zik2SoftwareInfo * info);
static Zik2SystemInfo *zik2_system_info_copy (Zik2SystemInfo * info);
static Zik2NoiseControlInfo *zik2_noise_control_info_copy (Zik2NoiseControlInfo * info);
static Zik2SourceInfo *zik2_source_info_copy (Zik2SourceInfo * info);
static Zik2BatteryInfo *zik2_battery_info_copy (Zik2BatteryInfo * info);
static Zik2VolumeInfo *zik2_volume_info_copy (Zik2VolumeInfo * info);
static Zik2HeadDetectionInfo *zik2_head_detection_info_copy (Zik2HeadDetectionInfo * info);
static Zik2ColorInfo *zik2_color_info_copy (Zik2ColorInfo * info);
static Zik2FlightModeInfo *zik2_flight_mode_info_copy (Zik2FlightModeInfo * info);
static Zik2BluetoothInfo *zik2_bluetooth_info_copy (Zik2BluetoothInfo * info);
static Zik2SoundEffectInfo *zik2_sound_effect_info_copy (Zik2SoundEffectInfo * info);
static Zik2AutoConnectionInfo *zik2_auto_connection_info_copy (Zik2AutoConnectionInfo * info);
static Zik2TrackInfo *zik2_track_info_copy (Zik2TrackInfo * info);
static Zik2MetadataInfo *zik2_metadata_info_copy (Zik2MetadataInfo * info);
static Zik2EqualizerInfo *zik2_equalizer_info_copy (Zik2EqualizerInfo * info);
static Zik2SmartAudioTuneInfo *zik2_smart_audio_tune_info_copy (Zik2SmartAudioTuneInfo * info);

#define ZIK2_DEFINE_BOXED_TYPE(TypeName, type_name) \
  G_DEFINE_BOXED_TYPE (TypeName, type_name, type_name##_copy, type_name##_free)

ZIK2_DEFINE_BOXED_TYPE (Zik2AnswerInfo, zik2_answer_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2AudioInfo, zik2_audio_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2SoftwareInfo, zik2_software_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2SystemInfo, zik2_system_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2NoiseControlInfo, zik2_noise_control_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2SourceInfo, zik2_source_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2BatteryInfo, zik2_battery_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2VolumeInfo, zik2_volume_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2HeadDetectionInfo, zik2_head_detection_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2ColorInfo, zik2_color_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2FlightModeInfo, zik2_flight_mode_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2BluetoothInfo, zik2_bluetooth_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2SoundEffectInfo, zik2_sound_effect_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2AutoConnectionInfo, zik2_auto_connection_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2TrackInfo, zik2_track_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2MetadataInfo, zik2_metadata_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2EqualizerInfo, zik2_equalizer_info);
ZIK2_DEFINE_BOXED_TYPE (Zik2SmartAudioTuneInfo, zik2_smart_audio_tune_info);

Zik2AnswerInfo *
zik2_answer_info_new (const gchar * path, gboolean error)
{
  Zik2AnswerInfo *info;

  info = g_slice_new0 (Zik2AnswerInfo);
  info->itype = ZIK2_ANSWER_INFO_TYPE;
  info->path = g_strdup (path);
  info->error = error;
  return info;
}

static Zik2AnswerInfo *
zik2_answer_info_copy (Zik2AnswerInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_ANSWER_INFO_TYPE, NULL);

  return zik2_answer_info_new (info->path, info->error);
}

void
zik2_answer_info_free (Zik2AnswerInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_ANSWER_INFO_TYPE);

  g_free (info->path);
  g_slice_free (Zik2AnswerInfo, info);
}

Zik2AudioInfo *
zik2_audio_info_new (void)
{
  Zik2AudioInfo *info;

  info = g_slice_new0 (Zik2AudioInfo);
  info->itype = ZIK2_AUDIO_INFO_TYPE;
  return info;
}

static Zik2AudioInfo *
zik2_audio_info_copy (Zik2AudioInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_AUDIO_INFO_TYPE, NULL);

  return zik2_audio_info_new ();
}

void
zik2_audio_info_free (Zik2AudioInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_AUDIO_INFO_TYPE);

  g_slice_free (Zik2AudioInfo, info);
}

Zik2SoftwareInfo *
zik2_software_info_new (const gchar * sip6, const gchar * pic,
    const gchar * tts)
{
  Zik2SoftwareInfo *info;

  info = g_slice_new0 (Zik2SoftwareInfo);
  info->itype = ZIK2_SOFTWARE_INFO_TYPE;
  info->sip6 = g_strdup (sip6);
  info->pic = g_strdup (pic);
  info->tts = g_strdup (tts);
  return info;
}

static Zik2SoftwareInfo *
zik2_software_info_copy (Zik2SoftwareInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_SOFTWARE_INFO_TYPE, NULL);

  return zik2_software_info_new (info->sip6, info->pic, info->tts);
}

void
zik2_software_info_free (Zik2SoftwareInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_SOFTWARE_INFO_TYPE);

  g_free (info->sip6);
  g_free (info->pic);
  g_free (info->tts);
  g_slice_free (Zik2SoftwareInfo, info);
}

Zik2SystemInfo *
zik2_system_info_new (const gchar * pi)
{
  Zik2SystemInfo *info;

  info = g_slice_new0 (Zik2SystemInfo);
  info->itype = ZIK2_SYSTEM_INFO_TYPE;
  info->pi = g_strdup (pi);
  return info;
}

static Zik2SystemInfo *
zik2_system_info_copy (Zik2SystemInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_SYSTEM_INFO_TYPE, NULL);

  return zik2_system_info_new (info->pi);
}

void
zik2_system_info_free (Zik2SystemInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_SYSTEM_INFO_TYPE);

  g_free (info->pi);
  g_slice_free (Zik2SystemInfo, info);
}

Zik2NoiseControlInfo *
zik2_noise_control_info_new (gboolean enabled, gchar * type, guint value)
{
  Zik2NoiseControlInfo *info;

  info = g_slice_new0 (Zik2NoiseControlInfo);
  info->itype = ZIK2_NOISE_CONTROL_INFO_TYPE;
  info->enabled = enabled;
  info->type = g_strdup (type);
  info->value = value;
  return info;
}

static Zik2NoiseControlInfo *
zik2_noise_control_info_copy (Zik2NoiseControlInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_NOISE_CONTROL_INFO_TYPE, NULL);

  return zik2_noise_control_info_new (info->enabled, info->type, info->value);
}

void
zik2_noise_control_info_free (Zik2NoiseControlInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_NOISE_CONTROL_INFO_TYPE);

  g_free (info->type);
  g_slice_free (Zik2NoiseControlInfo, info);
}

Zik2SourceInfo *
zik2_source_info_new (const gchar * type)
{
  Zik2SourceInfo *info;

  info = g_slice_new0 (Zik2SourceInfo);
  info->itype = ZIK2_SOURCE_INFO_TYPE;
  info->type = g_strdup (type);
  return info;
}

static Zik2SourceInfo *
zik2_source_info_copy (Zik2SourceInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_SOURCE_INFO_TYPE, NULL);

  return zik2_source_info_new (info->type);
}

void
zik2_source_info_free (Zik2SourceInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_SOURCE_INFO_TYPE);

  g_free (info->type);
  g_slice_free (Zik2SourceInfo, info);
}

Zik2BatteryInfo *
zik2_battery_info_new (const gchar * state, guint percent)
{
  Zik2BatteryInfo *info;

  info = g_slice_new0 (Zik2BatteryInfo);
  info->itype = ZIK2_BATTERY_INFO_TYPE;
  info->state = g_strdup (state);
  info->percent = percent;
  return info;
}

static Zik2BatteryInfo *
zik2_battery_info_copy (Zik2BatteryInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_BATTERY_INFO_TYPE, NULL);

  return zik2_battery_info_new (info->state, info->percent);
}

void
zik2_battery_info_free (Zik2BatteryInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_BATTERY_INFO_TYPE);

  g_free (info->state);
  g_slice_free (Zik2BatteryInfo, info);
}

Zik2VolumeInfo *
zik2_volume_info_new (guint volume)
{
  Zik2VolumeInfo *info;

  info = g_slice_new0 (Zik2VolumeInfo);
  info->itype = ZIK2_VOLUME_INFO_TYPE;
  info->volume = volume;
  return info;
}

static Zik2VolumeInfo *
zik2_volume_info_copy (Zik2VolumeInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_VOLUME_INFO_TYPE, NULL);

  return zik2_volume_info_new (info->volume);
}

void
zik2_volume_info_free (Zik2VolumeInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_VOLUME_INFO_TYPE);

  g_slice_free (Zik2VolumeInfo, info);
}

Zik2HeadDetectionInfo *
zik2_head_detection_info_new (gboolean enabled)
{
  Zik2HeadDetectionInfo *info;

  info = g_slice_new0 (Zik2HeadDetectionInfo);
  info->itype = ZIK2_HEAD_DETECTION_INFO_TYPE;
  info->enabled = enabled;
  return info;
}

static Zik2HeadDetectionInfo *
zik2_head_detection_info_copy (Zik2HeadDetectionInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_HEAD_DETECTION_INFO_TYPE, NULL);

  return zik2_head_detection_info_new (info->enabled);
}

void
zik2_head_detection_info_free (Zik2HeadDetectionInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_HEAD_DETECTION_INFO_TYPE);

  g_slice_free (Zik2HeadDetectionInfo, info);
}

Zik2ColorInfo *
zik2_color_info_new (guint value)
{
  Zik2ColorInfo *info;

  info = g_slice_new0 (Zik2ColorInfo);
  info->itype = ZIK2_COLOR_INFO_TYPE;
  info->value = value;
  return info;
}

static Zik2ColorInfo *
zik2_color_info_copy (Zik2ColorInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_COLOR_INFO_TYPE, NULL);

  return zik2_color_info_new (info->value);
}

void
zik2_color_info_free (Zik2ColorInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_COLOR_INFO_TYPE);

  g_slice_free (Zik2ColorInfo, info);
}

Zik2FlightModeInfo *
zik2_flight_mode_info_new (gboolean enabled)
{
  Zik2FlightModeInfo *info;

  info = g_slice_new0 (Zik2FlightModeInfo);
  info->itype = ZIK2_FLIGHT_MODE_INFO_TYPE;
  info->enabled = enabled;
  return info;
}

static Zik2FlightModeInfo *
zik2_flight_mode_info_copy (Zik2FlightModeInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_FLIGHT_MODE_INFO_TYPE, NULL);

  return zik2_flight_mode_info_new (info->enabled);
}

void
zik2_flight_mode_info_free (Zik2FlightModeInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_FLIGHT_MODE_INFO_TYPE);

  g_slice_free (Zik2FlightModeInfo, info);
}

Zik2BluetoothInfo *
zik2_bluetooth_info_new (const gchar * friendlyname)
{
  Zik2BluetoothInfo *info;

  info = g_slice_new0 (Zik2BluetoothInfo);
  info->itype = ZIK2_BLUETOOTH_INFO_TYPE;
  info->friendlyname = g_strdup (friendlyname);
  return info;
}

static Zik2BluetoothInfo *
zik2_bluetooth_info_copy (Zik2BluetoothInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_BLUETOOTH_INFO_TYPE, NULL);

  return zik2_bluetooth_info_new (info->friendlyname);
}

void
zik2_bluetooth_info_free (Zik2BluetoothInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_BLUETOOTH_INFO_TYPE);

  g_free (info->friendlyname);
  g_slice_free (Zik2BluetoothInfo, info);
}

Zik2SoundEffectInfo *
zik2_sound_effect_info_new (gboolean enabled, const gchar * room_size,
    guint angle)
{
  Zik2SoundEffectInfo *info;

  info = g_slice_new0 (Zik2SoundEffectInfo);
  info->itype = ZIK2_SOUND_EFFECT_INFO_TYPE;
  info->enabled = enabled;
  info->room_size = g_strdup (room_size);
  info->angle = angle;
  return info;
}

static Zik2SoundEffectInfo *
zik2_sound_effect_info_copy (Zik2SoundEffectInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_SOUND_EFFECT_INFO_TYPE, NULL);

  return zik2_sound_effect_info_new (info->enabled, info->room_size,
      info->angle);
}

void
zik2_sound_effect_info_free (Zik2SoundEffectInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_SOUND_EFFECT_INFO_TYPE);

  g_free (info->room_size);
  g_slice_free (Zik2SoundEffectInfo, info);
}

Zik2AutoConnectionInfo *
zik2_auto_connection_info_new (gboolean enabled)
{
  Zik2AutoConnectionInfo *info;

  info = g_slice_new0 (Zik2AutoConnectionInfo);
  info->itype = ZIK2_AUTO_CONNECTION_INFO_TYPE;
  info->enabled = enabled;
  return info;
}

static Zik2AutoConnectionInfo *
zik2_auto_connection_info_copy (Zik2AutoConnectionInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_AUTO_CONNECTION_INFO_TYPE, NULL);

  return zik2_auto_connection_info_new (info->enabled);
}

void
zik2_auto_connection_info_free (Zik2AutoConnectionInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_AUTO_CONNECTION_INFO_TYPE);

  g_slice_free (Zik2AutoConnectionInfo, info);
}

Zik2TrackInfo *
zik2_track_info_new (void)
{
  Zik2TrackInfo *info;

  info = g_slice_new0 (Zik2TrackInfo);
  info->itype = ZIK2_TRACK_INFO_TYPE;
  return info;
}

Zik2TrackInfo *
zik2_track_info_copy (Zik2TrackInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_TRACK_INFO_TYPE, NULL);

  return zik2_track_info_new ();
}

void
zik2_track_info_free (Zik2TrackInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_TRACK_INFO_TYPE);

  g_slice_free (Zik2TrackInfo, info);
}

Zik2MetadataInfo *
zik2_metadata_info_new (gboolean playing, const gchar * title,
    const gchar * artist, const gchar * album, const gchar * genre)
{
  Zik2MetadataInfo *info;

  info = g_slice_new0 (Zik2MetadataInfo);
  info->itype = ZIK2_METADATA_INFO_TYPE;
  info->playing = playing;
  info->title = g_strdup (title);
  info->artist = g_strdup (artist);
  info->album = g_strdup (album);
  info->genre = g_strdup (genre);
  return info;
}

static Zik2MetadataInfo *
zik2_metadata_info_copy (Zik2MetadataInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_METADATA_INFO_TYPE, NULL);

  return zik2_metadata_info_new (info->playing, info->title, info->artist,
      info->album, info->genre);
}

void
zik2_metadata_info_free (Zik2MetadataInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_METADATA_INFO_TYPE);

  g_free (info->title);
  g_free (info->artist);
  g_free (info->album);
  g_free (info->genre);
  g_slice_free (Zik2MetadataInfo, info);
}

Zik2EqualizerInfo *
zik2_equalizer_info_new (gboolean enabled)
{
  Zik2EqualizerInfo *info;

  info = g_slice_new0 (Zik2EqualizerInfo);
  info->itype = ZIK2_EQUALIZER_INFO_TYPE;
  info->enabled = enabled;
  return info;
}

static Zik2EqualizerInfo *
zik2_equalizer_info_copy (Zik2EqualizerInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_EQUALIZER_INFO_TYPE, NULL);

  return zik2_equalizer_info_new (info->enabled);
}

void
zik2_equalizer_info_free (Zik2EqualizerInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_EQUALIZER_INFO_TYPE);

  g_slice_free (Zik2EqualizerInfo, info);
}

Zik2SmartAudioTuneInfo *
zik2_smart_audio_tune_info_new (gboolean enabled)
{
  Zik2SmartAudioTuneInfo *info;

  info = g_slice_new0 (Zik2SmartAudioTuneInfo);
  info->itype = ZIK2_SMART_AUDIO_TUNE_INFO_TYPE;
  info->enabled = enabled;
  return info;
}

static Zik2SmartAudioTuneInfo *
zik2_smart_audio_tune_info_copy (Zik2SmartAudioTuneInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK2_SMART_AUDIO_TUNE_INFO_TYPE, NULL);

  return zik2_smart_audio_tune_info_new (info->enabled);
}

void
zik2_smart_audio_tune_info_free (Zik2SmartAudioTuneInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK2_SMART_AUDIO_TUNE_INFO_TYPE);

  g_slice_free (Zik2SmartAudioTuneInfo, info);
}
