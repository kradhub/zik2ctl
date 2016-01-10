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

#ifndef ZIK2_CONNECTION_H
#define ZIK2_CONNECTION_H

#include <glib.h>

#include "zikmessage.h"

G_BEGIN_DECLS

typedef struct _Zik2Connection Zik2Connection;

Zik2Connection *zik2_connection_new (gint fd);
void zik2_connection_free (Zik2Connection * conn);

gboolean zik2_connection_open_session (Zik2Connection * conn);
gboolean zik2_connection_close_session (Zik2Connection * conn);

gboolean zik2_connection_send_message (Zik2Connection * conn, ZikMessage * msg,
    ZikMessage ** out_answer);

G_END_DECLS

#endif
