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

#ifndef ZIK2_MESSAGE_H
#define ZIK2_MESSAGE_H

#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef struct _Zik2Message Zik2Message;
typedef struct _Zik2RequestReplyData Zik2RequestReplyData;

void zik2_message_free (Zik2Message * msg);

Zik2Message *zik2_message_new_from_buffer (const guint8 * data, gsize size);
guint8 *zik2_message_make_buffer (Zik2Message * msg, gsize *out_size);

Zik2Message *zik2_message_new_open_session (void);

gboolean zik2_message_is_acknowledge (Zik2Message * msg);

Zik2Message *zik2_message_new_request (const gchar * path, const gchar * method,
    const gchar * args);
gboolean zik2_message_is_request (Zik2Message * msg);
gboolean zik2_message_parse_request_reply (Zik2Message * msg,
    Zik2RequestReplyData ** reply);
gchar *zik2_message_get_request_reply_xml (Zik2Message * msg);

void zik2_request_reply_data_free (Zik2RequestReplyData * reply_data);
gpointer zik2_request_reply_data_find_node_info (Zik2RequestReplyData * reply,
    GType type);
gboolean zik2_request_reply_data_error (Zik2RequestReplyData * reply);

G_END_DECLS

#endif
