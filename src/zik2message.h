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

#ifndef ZIK2_MESSAGE_H
#define ZIK2_MESSAGE_H

#include <glib.h>
#include <glib-object.h>

#define API_AUDIO_TRACK_METADATA_URI "/api/audio/track/metadata"
#define API_URI "/api"
#define API_AUDIO_NOISE_CONTROL_ENABLED_URI "/api/audio/noise_control/enabled"
#define API_AUDIO_NOISE_CONTROL_URI "/api/audio/noise_control"
#define API_AUDIO_NOISE_CONTROL_PHONE_MODE_URI "/api/audio/noise_control/phone_mode"
#define API_AUDIO_NOISE_CANCELLATION_ENABLE_URI "api/audio/noise_cancellation/enabled"
#define API_AUDIO_THUMB_EQUALIZER_VALUE_URI "/api/audio/thumb_equalizer/value"
#define API_AUDIO_EQUALIZED_ENABLED_URI "/api/audio/equalizer/enabled"
#define API_AUDIO_SMART_AUDIO_TUNE_URI "/api/audio/smart_audio_tune"
#define API_AUDIO_PRESET_BYPASS_URI "/api/audio/preset/bypass"
#define API_AUDIO_PRESET_CURRENT_URI "/api/audio/preset/current"
#define API_AUDIO_SOUND_EFFECT_ENABLED_URI "/api/audio/sound_effect/enabled"
#define API_AUDIO_SOUND_EFFECT_URI "/api/audio/sound_effect"
#define API_AUDIO_NOISE_URI "/api/audio/noise"
#define API_AUDIO_VOLUME_URI "/api/audio/volume"
#define API_AUDIO_SOURCE_URI "/api/audio/source"

#define API_SOFTWARE_VERSION_URI "/api/software/version"

#define API_BLUETOOTH_FRIENDLY_NAME_URI "/api/bluetooth/friendlyname"

#define API_SYSTEM_BATTERY_URI "/api/system/battery"
#define API_SYSTEM_BATTERY_FORECAST_URI "/api/system/battery/forecast"
#define API_SYSTEM_AUTO_CONNECTION_ENABLED_URI "/api/system/auto_connection/enabled"
#define API_SYSTEM_ANC_PHONE_MODE_ENABLED_URI "/api/system/anc_phone_mode/enabled"
#define API_SYSTEM_DEVICE_TYPE_URI "/api/system/device_type"
#define API_SYSTEM_COLOR_URI "/api/system/color"
#define API_SYSTEM_PI_URI "/api/system/pi"
#define API_SYSTEM_HEAD_DETECTION_ENABLED_URI "/api/system/head_detection/enabled"
#define API_SYSTEM_FLIGHT_MODE_URI "/api/flight_mode"

G_BEGIN_DECLS

typedef struct _Zik2Message Zik2Message;
typedef struct _Zik2RequestReplyData Zik2RequestReplyData;

void zik2_message_free (Zik2Message * msg);

Zik2Message *zik2_message_new_from_buffer (const guint8 * data, gsize size);
guint8 *zik2_message_make_buffer (Zik2Message * msg, gsize *out_size);

Zik2Message *zik2_message_new_open_session (void);

gboolean zik2_message_is_acknowledge (Zik2Message * msg);

Zik2Message *zik2_message_new_request (const gchar * path, const gchar * method,
    const gchar * args);
gboolean zik2_message_is_request (Zik2Message * msg);
gboolean zik2_message_parse_request_reply (Zik2Message * msg,
    Zik2RequestReplyData ** reply);
gchar *zik2_message_get_request_reply_xml (Zik2Message * msg);

void zik2_request_reply_data_free (Zik2RequestReplyData * reply_data);
gpointer zik2_request_reply_data_find_node_info (Zik2RequestReplyData * reply,
    GType type);
gboolean zik2_request_reply_data_error (Zik2RequestReplyData * reply);

G_END_DECLS

#endif
