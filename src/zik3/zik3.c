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
};

struct _Zik3Private
{
};

#define parent_class zik3_parent_class
G_DEFINE_TYPE (Zik3, zik3, ZIK_TYPE);

static void
zik3_class_init (Zik3Class * klass)
{
  g_type_class_add_private (klass, sizeof (Zik3Private));
}

static void
zik3_init (Zik3 * zik3)
{
  zik3->priv = G_TYPE_INSTANCE_GET_PRIVATE (zik3, ZIK3_TYPE, Zik3Private);
}

/* Static properties are the one which not change at all or only change
 * with user action */
static void
zik3_sync_static_properties (Zik3 * zik3)
{
  zik_sync_static_properties (ZIK (zik3));
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
