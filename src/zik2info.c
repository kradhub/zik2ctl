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
  return zik2_answer_info_new (info->path, info->error);
}

void
zik2_answer_info_free (Zik2AnswerInfo * info)
{
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
  return zik2_audio_info_new ();
}

void
zik2_audio_info_free (Zik2AudioInfo * info)
{
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
  return zik2_software_info_new (info->sip6, info->pic, info->tts);
}

void
zik2_software_info_free (Zik2SoftwareInfo * info)
{
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
  return zik2_system_info_new (info->pi);
}

void
zik2_system_info_free (Zik2SystemInfo * info)
{
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
  return zik2_noise_control_info_new (info->enabled, info->type, info->value);
}

void
zik2_noise_control_info_free (Zik2NoiseControlInfo * info)
{
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
  return zik2_source_info_new (info->type);
}

void
zik2_source_info_free (Zik2SourceInfo * info)
{
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
  return zik2_battery_info_new (info->state, info->percent);
}

void
zik2_battery_info_free (Zik2BatteryInfo * info)
{
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
  return zik2_volume_info_new (info->volume);
}

void
zik2_volume_info_free (Zik2VolumeInfo * info)
{
  g_slice_free (Zik2VolumeInfo, info);
}
