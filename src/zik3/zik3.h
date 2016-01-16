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

#ifndef ZIK3_H
#define ZIK3_H

#include <glib.h>
#include <glib-object.h>
#include "zik.h"

G_BEGIN_DECLS

#define ZIK3_TYPE (zik3_get_type ())
#define ZIK3(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), ZIK3_TYPE, Zik3))
#define ZIK3_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), ZIK3_TYPE, Zik3Class))
#define ZIK3_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ZIK3_TYPE, Zik3Class))
#define IS_ZIK3(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), ZIK3_TYPE))
#define IS_ZIK3_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), ZIK3_TYPE))
#define ZIK3_CAST(obj) ((Zik3 *) (obj))

typedef struct _Zik3Class Zik3Class;
typedef struct _Zik3 Zik3;
typedef struct _Zik3Private Zik3Private;

struct _Zik3
{
  Zik parent;

  Zik3Private *priv;
};

struct _Zik3Class
{
  ZikClass parent_class;
};

GType zik3_get_type (void);
Zik3 *zik3_new (const gchar * name, const gchar * address, ZikConnection * conn);

gboolean zik3_is_auto_noise_control_active (Zik3 * zik3);
gboolean zik3_set_auto_noise_control_active (Zik3 * zik3, gboolean active);

const gchar *zik3_get_sound_effect_mode (Zik3 * zik3);

G_END_DECLS

#endif
