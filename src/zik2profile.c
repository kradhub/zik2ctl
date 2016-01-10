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

#include <gio/gunixfdlist.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "bluetooth-client.h"
#include "zik2profile.h"
#include "zik2.h"
#include "zikconnection.h"

#define ZIK2_PATH "/org/bluez/zik2"

#define parent_class zik2_profile_parent_class
G_DEFINE_TYPE (Zik2Profile, zik2_profile, ZIK_PROFILE_TYPE);

static GObject * zik2_profile_new_connection (ZikProfile * profile,
    BluetoothDevice1 *device, gint fd);
static gboolean zik2_profile_close_connection (ZikProfile * profile,
    GObject * zik);

static void
zik2_profile_class_init (Zik2ProfileClass * klass)
{
  ZikProfileClass *zik_profile_class = ZIK_PROFILE_CLASS (klass);

  zik_profile_class->profile_name = "Zik2 controller profile";
  zik_profile_class->profile_uuid = ZIK2_PROFILE_UUID;
  zik_profile_class->object_path = ZIK2_PATH;

  zik_profile_class->new_connection = zik2_profile_new_connection;
  zik_profile_class->close_connection = zik2_profile_close_connection;
}

static void
zik2_profile_init (Zik2Profile * profile)
{
}

static GObject *
zik2_profile_new_connection (ZikProfile * profile, BluetoothDevice1 *device,
    gint fd)
{
  Zik2 *zik2;
  ZikConnection *conn = NULL;

  conn = zik_connection_new (fd);
  if (conn == NULL) {
    g_critical ("failed to create zik2_connection for fd %d", fd);
    return NULL;
  }

  /* open session */
  if (!zik_connection_open_session (conn)) {
    g_warning ("failed to open session");
    zik_connection_free (conn);
    return NULL;
  }

  zik2 = zik2_new (bluetooth_device1_get_name (device),
      bluetooth_device1_get_address (device), conn);

  return G_OBJECT (zik2);
}

static gboolean
zik2_profile_close_connection (ZikProfile * profile, GObject * zik)
{
  Zik2 *zik2 = ZIK2 (zik);

  if (!zik_connection_close_session (zik2->conn))
    return FALSE;

  return TRUE;
}

Zik2Profile *
zik2_profile_new (void)
{
  return (Zik2Profile *) g_object_new (ZIK2_PROFILE_TYPE, NULL);
}
