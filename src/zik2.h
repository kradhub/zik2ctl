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

#ifndef ZIK2_H
#define ZIK2_H

#include <glib.h>
#include <glib-object.h>
#include "zik2connection.h"

G_BEGIN_DECLS

#define ZIK2_TYPE (zik2_get_type ())
#define ZIK2(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ZIK2_TYPE, Zik2))
#define ZIK2_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ZIK2_TYPE, Zik2Class))

enum _Zik2NoiseControlMode
{
  ZIK2_NOISE_CONTROL_MODE_OFF,
  ZIK2_NOISE_CONTROL_MODE_ANC,
  ZIK2_NOISE_CONTROL_MODE_AOC
};

typedef enum _Zik2NoiseControlMode Zik2NoiseControlMode;

enum _Zik2Color
{
  ZIK2_COLOR_BLUE = 0x2,
};
typedef enum _Zik2Color Zik2Color;

typedef struct _Zik2Class Zik2Class;
typedef struct _Zik2 Zik2;

struct _Zik2
{
  GObject parent;

  gchar *name;
  gchar *address;

  Zik2Connection *conn;

  gchar *serial;
  Zik2Color color;
  gchar *software_version;

  gboolean noise_control_enabled;
  Zik2NoiseControlMode noise_control_mode;
  guint noise_control_strength;

  gint noise_level;
  guint volume;
  gchar *source;
  gchar *battery_state;
  guint battery_percentage;
};

struct _Zik2Class
{
  GObjectClass parent_class;
};

GType zik2_get_type (void);
Zik2 *zik2_new (const gchar * name, const gchar * address, Zik2Connection * conn);

G_END_DECLS

#endif
