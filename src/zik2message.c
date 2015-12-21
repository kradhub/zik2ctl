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

#include "zik2message.h"

#define ZIK2_MESSAGE_HEADER_LEN 3

typedef enum
{
  ZIK2_MESSAGE_ID_OPEN_SESSION = 0x0,
  ZIK2_MESSAGE_ID_ACK = 0x2,
  ZIK2_MESSAGE_ID_REQ = 0x80
} Zik2MessageId;

struct _Zik2Message
{
  Zik2MessageId id;

  guint8 *payload;
  gsize payload_size;

  /* message data and size */
  guint8 *data;
  guint16 size;
};

void
zik2_message_free (Zik2Message * msg)
{
  g_free (msg->payload);
  g_slice_free (Zik2Message, msg);
}


/* free after usage */
guint8 *
zik2_message_make_buffer (Zik2Message * msg, gsize *out_size)
{
  guint16 size;
  guint8 *data;

  if (ZIK2_MESSAGE_HEADER_LEN + msg->payload_size > G_MAXUINT16)
    return NULL;

  size = ZIK2_MESSAGE_HEADER_LEN + msg->payload_size;
  data = g_malloc (size);

  /* header structure is
   *   uint16_t: length of header + payload in network byte order
   *   uint8_t:  message id
   */

  *((guint16 *) data) = g_htons (size);
  data[2] = msg->id;

  if (msg->payload)
    memcpy (data + 3, msg->payload, msg->payload_size);

  *out_size = size;
  return data;
}

Zik2Message *
zik2_message_new_from_buffer (const guint8 * data, gsize size)
{
  Zik2Message *msg;
  gsize msg_size;

  if (size < ZIK2_MESSAGE_HEADER_LEN)
    goto too_small;

  msg_size = g_ntohs (*((guint16 *) data));

  if (msg_size < ZIK2_MESSAGE_HEADER_LEN || msg_size > size)
    goto bad_size;

  msg = g_slice_new0 (Zik2Message);
  msg->id = data[2];

  msg->payload_size = msg_size - ZIK2_MESSAGE_HEADER_LEN;
  msg->payload = g_malloc (msg->payload_size);
  memcpy (msg->payload, data + 3, msg->payload_size);
  return msg;

too_small:
  g_critical ("data is too small to contain a Zik2Message");
  return NULL;

bad_size:
  g_critical ("bad size in buffer: %" G_GSIZE_FORMAT, msg_size);
  return NULL;
}

Zik2Message *
zik2_message_new_open_session (void)
{
  Zik2Message *msg;

  msg = g_slice_new0 (Zik2Message);
  msg->id = ZIK2_MESSAGE_ID_OPEN_SESSION;
  msg->payload = NULL;
  msg->payload_size = 0;

  return msg;
}

gboolean
zik2_message_is_acknowledge (Zik2Message * msg)
{
  return msg->id == ZIK2_MESSAGE_ID_ACK;
}

Zik2Message *
zik2_message_new_request_get (const gchar * uri)
{
  Zik2Message *msg;

  msg = g_slice_new0 (Zik2Message);
  msg->id = ZIK2_MESSAGE_ID_REQ;
  msg->payload = (guint8 *) g_strdup_printf ("GET %s", uri);
  msg->payload_size = strlen ((gchar *) msg->payload);

  return msg;
}

gboolean
zik2_message_is_request (Zik2Message * msg)
{
  return msg->id == ZIK2_MESSAGE_ID_REQ;
}

gchar *
zik2_message_request_get_request_answer (Zik2Message * msg)
{
  g_return_val_if_fail (zik2_message_is_request (msg), NULL);

  /* answer has 4 unknown bytes prior to xml */
  return g_strndup (((gchar *) msg->payload) + 4, msg->payload_size - 4);
}

Zik2Message *
zik2_message_new_request_set (const gchar * uri, gboolean active)
{
  Zik2Message *msg;

  msg = g_slice_new0 (Zik2Message);
  msg->id = ZIK2_MESSAGE_ID_REQ;
  msg->payload = (guint8 *) g_strdup_printf ("GET %s=%s", uri,
      active ? "true" : "false");
  msg->payload_size = strlen ((gchar *) msg->payload);

  return msg;
}
