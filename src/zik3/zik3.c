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

#include <string.h>

#include "zik3.h"
#include "zikconnection.h"
#include "zikmessage.h"
#include "zikinfo.h"
#include "zikapi.h"

enum
{
  PROP_0,
  PROP_AUTO_NOISE_CONTROL,
  PROP_SOUND_EFFECT_MODE,
};

struct _Zik3Private
{
  gboolean auto_noise_control;
  gchar *sound_effect_mode;
};

#define parent_class zik3_parent_class
G_DEFINE_TYPE (Zik3, zik3, ZIK_TYPE);

/* GObject methods */
static void zik3_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec *pspec);
static void zik3_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec *pspec);

static void
zik3_class_init (Zik3Class * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (Zik3Private));

  gobject_class->get_property = zik3_get_property;
  gobject_class->set_property = zik3_set_property;

  /* FIXME: auto noise control may be a noise control mode depending on
   * what it is */
  g_object_class_install_property (gobject_class, PROP_AUTO_NOISE_CONTROL,
      g_param_spec_boolean ("auto-noise-control", "Auto Noise Control",
          "Whether automatic noise control is active or not", FALSE,
          G_PARAM_READWRITE | G_PARAM_STATIC_STRINGS));

  g_object_class_install_property (gobject_class, PROP_SOUND_EFFECT_MODE,
      g_param_spec_string ("sound-effect-mode", "Sound Effect Mode",
          "Current sound effect mode", NULL,
          G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
zik3_init (Zik3 * zik3)
{
  zik3->priv = G_TYPE_INSTANCE_GET_PRIVATE (zik3, ZIK3_TYPE, Zik3Private);
}

static void
zik3_sync_auto_noise_control (Zik3 * zik3)
{
  /* TODO: should be somewhere get along with base Zik Class noise control
   * synchronization. ie noise control get is done twice with that */

  ZikNoiseControlInfo *info;

  info = zik_request_info (ZIK_CAST (zik3), ZIK_API_AUDIO_NOISE_CONTROL_PATH,
      ZIK_NOISE_CONTROL_INFO_TYPE);
  if (info == NULL) {
    g_warning ("failed to get noise control info for auto noise control");
    return;
  }

  zik3->priv->auto_noise_control = info->auto_nc;
  zik_noise_control_info_unref (info);
}

static void
zik3_sync_sound_effect_mode (Zik3 * zik3)
{
  /* TODO: should be somewhere get along with base Zik Class as it is
   * already sync with sound effect */
  ZikSoundEffectInfo *info;

  info = zik_request_info (ZIK_CAST (zik3), ZIK_API_AUDIO_SOUND_EFFECT_PATH,
      ZIK_SOUND_EFFECT_INFO_TYPE);
  if (info == NULL) {
    g_warning ("failed to get sound effect mode");
    return;
  }

  g_free (zik3->priv->sound_effect_mode);
  zik3->priv->sound_effect_mode = g_strdup (info->mode);
  zik_sound_effect_info_unref (info);
}

static void
zik3_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec *pspec)
{
  Zik3 *zik3 = ZIK3 (object);

  switch (prop_id) {
    case PROP_AUTO_NOISE_CONTROL:
      g_value_set_boolean (value, zik3_is_auto_noise_control_active (zik3));
      break;
    case PROP_SOUND_EFFECT_MODE:
      g_value_set_string (value, zik3_get_sound_effect_mode (zik3));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
zik3_set_property (GObject * object, guint prop_id, const GValue * value,
    GParamSpec *pspec)
{
  Zik3 *zik3 = ZIK3 (object);

  switch (prop_id) {
    case PROP_AUTO_NOISE_CONTROL:
      zik3_set_auto_noise_control_active (zik3, g_value_get_boolean (value));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* Static properties are the one which not change at all or only change
 * with user action */
static void
zik3_sync_static_properties (Zik3 * zik3)
{
  zik_sync_static_properties (ZIK (zik3));

  zik3_sync_auto_noise_control (zik3);
  zik3_sync_sound_effect_mode (zik3);
}

/* @conn: (transfer full) */
Zik3 *
zik3_new (const gchar * name, const gchar * address, ZikConnection * conn)
{
  Zik3 *zik3;

  zik3 = g_object_new (ZIK3_TYPE, "name", name, "address", address,
      "connection", conn, NULL);

  zik3_sync_static_properties (zik3);

  return zik3;
}

gboolean
zik3_is_auto_noise_control_active (Zik3 * zik3)
{
  return zik3->priv->auto_noise_control;
}

gboolean
zik3_set_auto_noise_control_active (Zik3 * zik3, gboolean active)
{
  gboolean ret;

  ret = zik_do_request (ZIK_CAST (zik3),
      ZIK_API_AUDIO_NOISE_CONTROL_AUTO_NC_PATH, "set",
      active ? "true" : "false", NULL);
  if (ret)
    zik3->priv->auto_noise_control = active;

  return ret;
}

const gchar *
zik3_get_sound_effect_mode (Zik3 * zik3)
{
  return zik3->priv->sound_effect_mode;
}
