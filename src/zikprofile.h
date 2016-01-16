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

#ifndef ZIK_PROFILE_H
#define ZIK_PROFILE_H

#include <glib.h>
#include <gio/gio.h>

#include "bluetooth-client.h"
#include "zik.h"

G_BEGIN_DECLS

#define ZIK_PROFILE_TYPE (zik_profile_get_type ())
#define ZIK_PROFILE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), ZIK_PROFILE_TYPE, ZikProfile))
#define ZIK_PROFILE_CLASS(obj) \
  (G_TYPE_CHECK_CLASS_CAST((obj), ZIK_PROFILE_TYPE, ZikProfileClass))
#define ZIK_PROFILE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), ZIK_PROFILE_TYPE, ZikProfileClass))

typedef struct _ZikProfileClass ZikProfileClass;
typedef struct _ZikProfile ZikProfile;

struct _ZikProfile
{
  GObject parent;

  GDBusObjectManager *manager;
  GDBusConnection *conn;
  guint id;

  /* connected devices */
  GHashTable *devices;
};

struct _ZikProfileClass
{
  GObjectClass parent_class;

  GDBusNodeInfo *introspection_data;
  GDBusInterfaceVTable interface_vtable;

  const gchar *profile_name;
  const gchar *profile_uuid;
  const gchar *object_path;

  /* Return: a #Zik instance */
  Zik * (*new_connection) (ZikProfile * profile, BluetoothDevice1 *device,
      gint fd);
  gboolean (*close_connection) (ZikProfile * profile, Zik * zik);
};

GType zik_profile_get_type (void);

gboolean zik_profile_install (ZikProfile * profile,
    GDBusObjectManager * manager);

void zik_profile_uninstall (ZikProfile * profile);

G_END_DECLS

#endif
