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

#include "zik2.h"
#include "zikconnection.h"
#include "zikmessage.h"
#include "zikinfo.h"
#include "zikapi.h"

#define UNKNOWN_STR "unknown"
#define DEFAULT_NOISE_CONTROL_STRENGTH 1
#define DEFAULT_AUTO_POWER_OFF_TIMEOUT 0

enum
{
  PROP_0,
  PROP_COLOR,
};

struct _Zik2Private
{
  Zik2Color color;
};

#define ZIK2_COLOR_TYPE (zik2_color_get_type ())
static GType
zik2_color_get_type (void)
{
  static volatile GType type;
  static const GEnumValue colors[] = {
    { ZIK2_COLOR_UNKNOWN, "Unknown", "unknown" },
    { ZIK2_COLOR_BLACK, "Black", "black" },
    { ZIK2_COLOR_BLUE, "Blue", "blue" },
    { 0, NULL, NULL }
  };

  if (g_once_init_enter (&type)) {
    GType _type = g_enum_register_static ("Zik2Color", colors);
    g_once_init_leave (&type, _type);
  }

  return type;
}

#define parent_class zik2_parent_class
G_DEFINE_TYPE (Zik2, zik2, ZIK_TYPE);

/* GObject methods */
static void zik2_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec *pspec);

static void
zik2_class_init (Zik2Class * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (Zik2Private));

  gobject_class->get_property = zik2_get_property;

  g_object_class_install_property (gobject_class, PROP_COLOR,
      g_param_spec_enum ("color", "Color", "Zik2 color", ZIK2_COLOR_TYPE,
        ZIK2_COLOR_UNKNOWN, G_PARAM_READABLE | G_PARAM_STATIC_STRINGS));
}

static void
zik2_init (Zik2 * zik2)
{
  zik2->priv = G_TYPE_INSTANCE_GET_PRIVATE (zik2, ZIK2_TYPE, Zik2Private);
}

static void
zik2_sync_color (Zik2 * zik2)
{
  ZikColorInfo *info;

  info = zik_request_info (ZIK (zik2), ZIK_API_SYSTEM_COLOR_PATH,
      ZIK_COLOR_INFO_TYPE);
  if (info == NULL) {
    g_warning ("failed to get color");
    return;
  }

  zik2->priv->color = info->value;
  zik_color_info_unref (info);
}

static void
zik2_get_property (GObject * object, guint prop_id, GValue * value,
    GParamSpec *pspec)
{
  Zik2 *zik2 = ZIK2 (object);

  switch (prop_id) {
    case PROP_COLOR:
      g_value_set_enum (value, zik2_get_color (zik2));
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* Static properties are the one which not change at all or only change
 * with user action */
static void
zik2_sync_static_properties (Zik2 * zik2)
{
  zik_sync_static_properties (ZIK (zik2));

  zik2_sync_color (zik2);
}

/* @conn: (transfer full) */
Zik2 *
zik2_new (const gchar * name, const gchar * address, ZikConnection * conn)
{
  Zik2 *zik2;

  zik2 = g_object_new (ZIK2_TYPE, "name", name, "address", address,
      "connection", conn, NULL);

  zik2_sync_static_properties (zik2);

  return zik2;
}

Zik2Color
zik2_get_color (Zik2 * zik2)
{
  return zik2->priv->color;
}
