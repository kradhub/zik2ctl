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

#ifndef ZIK_API_H
#define ZIK_API_H

/* Audio */
#define ZIK_API_AUDIO_EQUALIZER_ENABLED_PATH "/api/audio/equalizer/enabled"
#define ZIK_API_AUDIO_NOISE_PATH "/api/audio/noise"
#define ZIK_API_AUDIO_NOISE_CONTROL_PATH "/api/audio/noise_control"
#define ZIK_API_AUDIO_NOISE_CONTROL_ENABLED_PATH "/api/audio/noise_control/enabled"
#define ZIK_API_AUDIO_NOISE_CONTROL_AUTO_NC_PATH "/api/audio/noise_control/auto_nc"
#define ZIK_API_AUDIO_NOISE_CONTROL_PHONE_MODE_PATH "/api/audio/noise_control/phone_mode"
#define ZIK_API_AUDIO_PRESET_BYPASS_PATH "/api/audio/preset/bypass"
#define ZIK_API_AUDIO_PRESET_CURRENT_PATH "/api/audio/preset/current"
#define ZIK_API_AUDIO_SMART_AUDIO_TUNE_PATH "/api/audio/smart_audio_tune"
#define ZIK_API_AUDIO_SOUND_EFFECT_PATH "/api/audio/sound_effect"
#define ZIK_API_AUDIO_SOUND_EFFECT_ENABLED_PATH "/api/audio/sound_effect/enabled"
#define ZIK_API_AUDIO_SOUND_EFFECT_ROOM_SIZE_PATH "/api/audio/sound_effect/room_size"
#define ZIK_API_AUDIO_SOUND_EFFECT_ANGLE_PATH "/api/audio/sound_effect/angle"
#define ZIK_API_AUDIO_SOURCE_PATH "/api/audio/source"
#define ZIK_API_AUDIO_THUMB_EQUALIZER_VALUE_PATH "/api/audio/thumb_equalizer/value"
#define ZIK_API_AUDIO_TRACK_METADATA_PATH "/api/audio/track/metadata"
#define ZIK_API_AUDIO_VOLUME_PATH "/api/audio/volume"

/* Bluetooth */
#define ZIK_API_BLUETOOTH_FRIENDLY_NAME_PATH "/api/bluetooth/friendlyname"

/* Software */
#define ZIK_API_SOFTWARE_VERSION_PATH "/api/software/version"
#define ZIK_API_SOFTWARE_TTS_PATH "/api/software/tts"

/* System */
#define ZIK_API_SYSTEM_ANC_PHONE_MODE_ENABLED_PATH "/api/system/anc_phone_mode/enabled"
#define ZIK_API_SYSTEM_AUTO_CONNECTION_ENABLED_PATH "/api/system/auto_connection/enabled"
#define ZIK_API_SYSTEM_BATTERY_PATH "/api/system/battery"
#define ZIK_API_SYSTEM_BATTERY_FORECAST_PATH "/api/system/battery/forecast"
#define ZIK_API_SYSTEM_COLOR_PATH "/api/system/color"
#define ZIK_API_SYSTEM_DEVICE_TYPE_PATH "/api/system/device_type"
#define ZIK_API_SYSTEM_FLIGHT_MODE_PATH "/api/flight_mode"
#define ZIK_API_SYSTEM_HEAD_DETECTION_ENABLED_PATH "/api/system/head_detection/enabled"
#define ZIK_API_SYSTEM_PI_PATH "/api/system/pi"
#define ZIK_API_SYSTEM_AUTO_POWER_OFF_PATH "/api/system/auto_power_off"

/* Other */
#define ZIK_API_FLIGHT_MODE_PATH "/api/flight_mode"

#endif
