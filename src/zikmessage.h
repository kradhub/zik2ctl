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

#ifndef ZIK_MESSAGE_H
#define ZIK_MESSAGE_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _ZikMessage ZikMessage;
typedef struct _ZikRequestReplyData ZikRequestReplyData;

void zik_message_free (ZikMessage * msg);

ZikMessage *zik_message_new_from_buffer (const guint8 * data, gsize size);
guint8 *zik_message_make_buffer (ZikMessage * msg, gsize *out_size);

ZikMessage *zik_message_new_open_session (void);
ZikMessage *zik_message_new_close_session (void);

gboolean zik_message_is_acknowledge (ZikMessage * msg);

ZikMessage *zik_message_new_request (const gchar * path, const gchar * method,
    const gchar * args);
gboolean zik_message_is_request (ZikMessage * msg);
gboolean zik_message_parse_request_reply (ZikMessage * msg,
    ZikRequestReplyData ** reply);
gchar *zik_message_get_request_reply_xml (ZikMessage * msg);

void zik_request_reply_data_free (ZikRequestReplyData * reply_data);
gpointer zik_request_reply_data_find_node_info (ZikRequestReplyData * reply,
    GType type);
gboolean zik_request_reply_data_error (ZikRequestReplyData * reply);

G_END_DECLS

#endif
