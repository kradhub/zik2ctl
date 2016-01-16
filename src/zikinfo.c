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

#include "zikinfo.h"

#define ZIK_DEFINE_BOXED_TYPE(TypeName, type_name) \
  G_DEFINE_BOXED_TYPE (TypeName, type_name, type_name##_ref, type_name##_unref)

ZIK_DEFINE_BOXED_TYPE (ZikAnswerInfo, zik_answer_info);
ZIK_DEFINE_BOXED_TYPE (ZikAudioInfo, zik_audio_info);
ZIK_DEFINE_BOXED_TYPE (ZikSoftwareInfo, zik_software_info);
ZIK_DEFINE_BOXED_TYPE (ZikSystemInfo, zik_system_info);
ZIK_DEFINE_BOXED_TYPE (ZikNoiseControlInfo, zik_noise_control_info);
ZIK_DEFINE_BOXED_TYPE (ZikSourceInfo, zik_source_info);
ZIK_DEFINE_BOXED_TYPE (ZikBatteryInfo, zik_battery_info);
ZIK_DEFINE_BOXED_TYPE (ZikVolumeInfo, zik_volume_info);
ZIK_DEFINE_BOXED_TYPE (ZikHeadDetectionInfo, zik_head_detection_info);
ZIK_DEFINE_BOXED_TYPE (ZikColorInfo, zik_color_info);
ZIK_DEFINE_BOXED_TYPE (ZikFlightModeInfo, zik_flight_mode_info);
ZIK_DEFINE_BOXED_TYPE (ZikBluetoothInfo, zik_bluetooth_info);
ZIK_DEFINE_BOXED_TYPE (ZikSoundEffectInfo, zik_sound_effect_info);
ZIK_DEFINE_BOXED_TYPE (ZikAutoConnectionInfo, zik_auto_connection_info);
ZIK_DEFINE_BOXED_TYPE (ZikTrackInfo, zik_track_info);
ZIK_DEFINE_BOXED_TYPE (ZikMetadataInfo, zik_metadata_info);
ZIK_DEFINE_BOXED_TYPE (ZikEqualizerInfo, zik_equalizer_info);
ZIK_DEFINE_BOXED_TYPE (ZikSmartAudioTuneInfo, zik_smart_audio_tune_info);
ZIK_DEFINE_BOXED_TYPE (ZikAutoPowerOffInfo, zik_auto_power_off_info);
ZIK_DEFINE_BOXED_TYPE (ZikTTSInfo, zik_tts_info);

ZikAnswerInfo *
zik_answer_info_new (const gchar * path, gboolean error)
{
  ZikAnswerInfo *info;

  info = g_slice_new0 (ZikAnswerInfo);
  info->itype = ZIK_ANSWER_INFO_TYPE;
  info->ref_count = 1;
  info->path = g_strdup (path);
  info->error = error;
  return info;
}

ZikAnswerInfo *
zik_answer_info_ref (ZikAnswerInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_ANSWER_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_answer_info_unref (ZikAnswerInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_ANSWER_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count)) {
    g_free (info->path);
    g_slice_free (ZikAnswerInfo, info);
  }
}

ZikAudioInfo *
zik_audio_info_new (void)
{
  ZikAudioInfo *info;

  info = g_slice_new0 (ZikAudioInfo);
  info->itype = ZIK_AUDIO_INFO_TYPE;
  info->ref_count = 1;
  return info;
}

ZikAudioInfo *
zik_audio_info_ref (ZikAudioInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_AUDIO_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_audio_info_unref (ZikAudioInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_AUDIO_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count))
    g_slice_free (ZikAudioInfo, info);
}

ZikSoftwareInfo *
zik_software_info_new (const gchar * sip6, const gchar * pic,
    const gchar * tts)
{
  ZikSoftwareInfo *info;

  info = g_slice_new0 (ZikSoftwareInfo);
  info->itype = ZIK_SOFTWARE_INFO_TYPE;
  info->ref_count = 1;
  info->sip6 = g_strdup (sip6);
  info->pic = g_strdup (pic);
  info->tts = g_strdup (tts);
  return info;
}

ZikSoftwareInfo *
zik_software_info_ref (ZikSoftwareInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_SOFTWARE_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_software_info_unref (ZikSoftwareInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_SOFTWARE_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count)) {
    g_free (info->sip6);
    g_free (info->pic);
    g_free (info->tts);
    g_slice_free (ZikSoftwareInfo, info);
  }
}

ZikSystemInfo *
zik_system_info_new (const gchar * pi)
{
  ZikSystemInfo *info;

  info = g_slice_new0 (ZikSystemInfo);
  info->itype = ZIK_SYSTEM_INFO_TYPE;
  info->ref_count = 1;
  info->pi = g_strdup (pi);
  return info;
}

ZikSystemInfo *
zik_system_info_ref (ZikSystemInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_SYSTEM_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_system_info_unref (ZikSystemInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_SYSTEM_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count)) {
    g_free (info->pi);
    g_slice_free (ZikSystemInfo, info);
  }
}

ZikNoiseControlInfo *
zik_noise_control_info_new (gboolean enabled, gchar * type, guint value,
    gboolean auto_nc)
{
  ZikNoiseControlInfo *info;

  info = g_slice_new0 (ZikNoiseControlInfo);
  info->itype = ZIK_NOISE_CONTROL_INFO_TYPE;
  info->ref_count = 1;
  info->enabled = enabled;
  info->type = g_strdup (type);
  info->value = value;
  info->auto_nc = auto_nc;
  return info;
}

ZikNoiseControlInfo *
zik_noise_control_info_ref (ZikNoiseControlInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_NOISE_CONTROL_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_noise_control_info_unref (ZikNoiseControlInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_NOISE_CONTROL_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count)) {
    g_free (info->type);
    g_slice_free (ZikNoiseControlInfo, info);
  }
}

ZikSourceInfo *
zik_source_info_new (const gchar * type)
{
  ZikSourceInfo *info;

  info = g_slice_new0 (ZikSourceInfo);
  info->itype = ZIK_SOURCE_INFO_TYPE;
  info->ref_count = 1;
  info->type = g_strdup (type);
  return info;
}

ZikSourceInfo *
zik_source_info_ref (ZikSourceInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_SOURCE_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_source_info_unref (ZikSourceInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_SOURCE_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count)) {
    g_free (info->type);
    g_slice_free (ZikSourceInfo, info);
  }
}

ZikBatteryInfo *
zik_battery_info_new (const gchar * state, guint percent)
{
  ZikBatteryInfo *info;

  info = g_slice_new0 (ZikBatteryInfo);
  info->itype = ZIK_BATTERY_INFO_TYPE;
  info->ref_count = 1;
  info->state = g_strdup (state);
  info->percent = percent;
  return info;
}

ZikBatteryInfo *
zik_battery_info_ref (ZikBatteryInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_BATTERY_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_battery_info_unref (ZikBatteryInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_BATTERY_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count)) {
    g_free (info->state);
    g_slice_free (ZikBatteryInfo, info);
  }
}

ZikVolumeInfo *
zik_volume_info_new (guint volume)
{
  ZikVolumeInfo *info;

  info = g_slice_new0 (ZikVolumeInfo);
  info->itype = ZIK_VOLUME_INFO_TYPE;
  info->ref_count = 1;
  info->volume = volume;
  return info;
}

ZikVolumeInfo *
zik_volume_info_ref (ZikVolumeInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_VOLUME_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_volume_info_unref (ZikVolumeInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_VOLUME_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count))
    g_slice_free (ZikVolumeInfo, info);
}

ZikHeadDetectionInfo *
zik_head_detection_info_new (gboolean enabled)
{
  ZikHeadDetectionInfo *info;

  info = g_slice_new0 (ZikHeadDetectionInfo);
  info->itype = ZIK_HEAD_DETECTION_INFO_TYPE;
  info->ref_count = 1;
  info->enabled = enabled;
  return info;
}

ZikHeadDetectionInfo *
zik_head_detection_info_ref (ZikHeadDetectionInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_HEAD_DETECTION_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_head_detection_info_unref (ZikHeadDetectionInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_HEAD_DETECTION_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count))
    g_slice_free (ZikHeadDetectionInfo, info);
}

ZikColorInfo *
zik_color_info_new (guint value)
{
  ZikColorInfo *info;

  info = g_slice_new0 (ZikColorInfo);
  info->itype = ZIK_COLOR_INFO_TYPE;
  info->ref_count = 1;
  info->value = value;
  return info;
}

ZikColorInfo *
zik_color_info_ref (ZikColorInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_COLOR_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_color_info_unref (ZikColorInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_COLOR_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count))
    g_slice_free (ZikColorInfo, info);
}

ZikFlightModeInfo *
zik_flight_mode_info_new (gboolean enabled)
{
  ZikFlightModeInfo *info;

  info = g_slice_new0 (ZikFlightModeInfo);
  info->itype = ZIK_FLIGHT_MODE_INFO_TYPE;
  info->ref_count = 1;
  info->enabled = enabled;
  return info;
}

ZikFlightModeInfo *
zik_flight_mode_info_ref (ZikFlightModeInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_FLIGHT_MODE_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_flight_mode_info_unref (ZikFlightModeInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_FLIGHT_MODE_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count))
    g_slice_free (ZikFlightModeInfo, info);
}

ZikBluetoothInfo *
zik_bluetooth_info_new (const gchar * friendlyname)
{
  ZikBluetoothInfo *info;

  info = g_slice_new0 (ZikBluetoothInfo);
  info->itype = ZIK_BLUETOOTH_INFO_TYPE;
  info->ref_count = 1;
  info->friendlyname = g_strdup (friendlyname);
  return info;
}

ZikBluetoothInfo *
zik_bluetooth_info_ref (ZikBluetoothInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_BLUETOOTH_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_bluetooth_info_unref (ZikBluetoothInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_BLUETOOTH_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count)) {
    g_free (info->friendlyname);
    g_slice_free (ZikBluetoothInfo, info);
  }
}

ZikSoundEffectInfo *
zik_sound_effect_info_new (gboolean enabled, const gchar * room_size,
    guint angle)
{
  ZikSoundEffectInfo *info;

  info = g_slice_new0 (ZikSoundEffectInfo);
  info->itype = ZIK_SOUND_EFFECT_INFO_TYPE;
  info->ref_count = 1;
  info->enabled = enabled;
  info->room_size = g_strdup (room_size);
  info->angle = angle;
  return info;
}

ZikSoundEffectInfo *
zik_sound_effect_info_ref (ZikSoundEffectInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_SOUND_EFFECT_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_sound_effect_info_unref (ZikSoundEffectInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_SOUND_EFFECT_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count)) {
    g_free (info->room_size);
    g_slice_free (ZikSoundEffectInfo, info);
  }
}

ZikAutoConnectionInfo *
zik_auto_connection_info_new (gboolean enabled)
{
  ZikAutoConnectionInfo *info;

  info = g_slice_new0 (ZikAutoConnectionInfo);
  info->itype = ZIK_AUTO_CONNECTION_INFO_TYPE;
  info->ref_count = 1;
  info->enabled = enabled;
  return info;
}

ZikAutoConnectionInfo *
zik_auto_connection_info_ref (ZikAutoConnectionInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_AUTO_CONNECTION_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_auto_connection_info_unref (ZikAutoConnectionInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_AUTO_CONNECTION_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count))
    g_slice_free (ZikAutoConnectionInfo, info);
}

ZikTrackInfo *
zik_track_info_new (void)
{
  ZikTrackInfo *info;

  info = g_slice_new0 (ZikTrackInfo);
  info->itype = ZIK_TRACK_INFO_TYPE;
  info->ref_count = 1;
  return info;
}

ZikTrackInfo *
zik_track_info_ref (ZikTrackInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_TRACK_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_track_info_unref (ZikTrackInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_TRACK_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count))
    g_slice_free (ZikTrackInfo, info);
}

ZikMetadataInfo *
zik_metadata_info_new (gboolean playing, const gchar * title,
    const gchar * artist, const gchar * album, const gchar * genre)
{
  ZikMetadataInfo *info;

  info = g_slice_new0 (ZikMetadataInfo);
  info->itype = ZIK_METADATA_INFO_TYPE;
  info->ref_count = 1;
  info->playing = playing;
  info->title = g_strdup (title);
  info->artist = g_strdup (artist);
  info->album = g_strdup (album);
  info->genre = g_strdup (genre);
  return info;
}

ZikMetadataInfo *
zik_metadata_info_ref (ZikMetadataInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_METADATA_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_metadata_info_unref (ZikMetadataInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_METADATA_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count)) {
    g_free (info->title);
    g_free (info->artist);
    g_free (info->album);
    g_free (info->genre);
    g_slice_free (ZikMetadataInfo, info);
  }
}

ZikEqualizerInfo *
zik_equalizer_info_new (gboolean enabled)
{
  ZikEqualizerInfo *info;

  info = g_slice_new0 (ZikEqualizerInfo);
  info->itype = ZIK_EQUALIZER_INFO_TYPE;
  info->ref_count = 1;
  info->enabled = enabled;
  return info;
}

ZikEqualizerInfo *
zik_equalizer_info_ref (ZikEqualizerInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_EQUALIZER_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_equalizer_info_unref (ZikEqualizerInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_EQUALIZER_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count))
    g_slice_free (ZikEqualizerInfo, info);
}

ZikSmartAudioTuneInfo *
zik_smart_audio_tune_info_new (gboolean enabled)
{
  ZikSmartAudioTuneInfo *info;

  info = g_slice_new0 (ZikSmartAudioTuneInfo);
  info->itype = ZIK_SMART_AUDIO_TUNE_INFO_TYPE;
  info->ref_count = 1;
  info->enabled = enabled;
  return info;
}

ZikSmartAudioTuneInfo *
zik_smart_audio_tune_info_ref (ZikSmartAudioTuneInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_SMART_AUDIO_TUNE_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_smart_audio_tune_info_unref (ZikSmartAudioTuneInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_SMART_AUDIO_TUNE_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count))
    g_slice_free (ZikSmartAudioTuneInfo, info);
}

ZikAutoPowerOffInfo *
zik_auto_power_off_info_new (guint value)
{
  ZikAutoPowerOffInfo *info;

  info = g_slice_new0 (ZikAutoPowerOffInfo);
  info->itype = ZIK_AUTO_POWER_OFF_INFO_TYPE;
  info->ref_count = 1;
  info->value = value;
  return info;
}

ZikAutoPowerOffInfo *
zik_auto_power_off_info_ref (ZikAutoPowerOffInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_AUTO_POWER_OFF_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_auto_power_off_info_unref (ZikAutoPowerOffInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_AUTO_POWER_OFF_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count))
    g_slice_free (ZikAutoPowerOffInfo, info);
}

ZikTTSInfo *
zik_tts_info_new (gboolean enabled)
{
  ZikTTSInfo *info;

  info = g_slice_new0 (ZikTTSInfo);
  info->itype = ZIK_TTS_INFO_TYPE;
  info->ref_count = 1;
  info->enabled = enabled;
  return info;
}

ZikTTSInfo *
zik_tts_info_ref (ZikTTSInfo * info)
{
  g_return_val_if_fail (info != NULL, NULL);
  g_return_val_if_fail (info->itype == ZIK_TTS_INFO_TYPE, NULL);
  g_return_val_if_fail (info->ref_count > 0, NULL);

  g_atomic_int_inc (&info->ref_count);
  return info;
}

void
zik_tts_info_unref (ZikTTSInfo * info)
{
  g_return_if_fail (info != NULL);
  g_return_if_fail (info->itype == ZIK_TTS_INFO_TYPE);
  g_return_if_fail (info->ref_count > 0);

  if (g_atomic_int_dec_and_test (&info->ref_count))
    g_slice_free (ZikTTSInfo, info);
}
