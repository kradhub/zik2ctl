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

#ifndef ZIK3_PROFILE_H
#define ZIK3_PROFILE_H

#include <glib.h>
#include <gio/gio.h>

#include "zikprofile.h"

G_BEGIN_DECLS

#define ZIK3_PROFILE_UUID "8b6814d3-6ce7-4498-9700-9312c1711f64"

#define ZIK3_PROFILE_TYPE (zik3_profile_get_type ())
#define ZIK3_PROFILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), ZIK3_PROFILE_TYPE, Zik3Profile))
#define ZIK3_PROFILE_CLASS(obj) \
  (G_TYPE_CHECK_CLASS_CAST((obj), ZIK3_PROFILE_TYPE, Zik3ProfileClass))
#define ZIK3_PROFILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ZIK3_PROFILE_TYPE, Zik3ProfileClass))

typedef struct _Zik3ProfileClass Zik3ProfileClass;
typedef struct _Zik3Profile Zik3Profile;

struct _Zik3Profile
{
  ZikProfile parent;
};

struct _Zik3ProfileClass
{
  ZikProfileClass parent_class;
};

GType zik3_profile_get_type (void);

Zik3Profile *zik3_profile_new (void);

G_END_DECLS

#endif

