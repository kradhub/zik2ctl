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

#include <gio/gio.h>

#include "zik2connection.h"

struct _Zik2Connection
{
  GSocket *socket;
  guint8 *recv_buffer;
  gsize recv_buffer_size;
};

Zik2Connection *
zik2_connection_new (int fd)
{
  Zik2Connection *conn;
  GError *error = NULL;

  conn = g_slice_new0 (Zik2Connection);
  conn->socket = g_socket_new_from_fd (fd, &error);

  if (conn->socket == NULL) {
    g_critical ("failed to create socket from fd %d: %s", fd, error->message);
    g_error_free (error);
    g_slice_free (Zik2Connection, conn);
    return NULL;
  }

  /* message size is stored to an uint16_t so make receive buffer accordingly */
  conn->recv_buffer_size = G_MAXUINT16;
  conn->recv_buffer = g_malloc (conn->recv_buffer_size);

  return conn;
}

void
zik2_connection_free (Zik2Connection * conn)
{
  if (conn->socket)
    g_object_unref (conn->socket);

  g_free (conn->recv_buffer);
  g_slice_free (Zik2Connection, conn);
}

gboolean
zik2_connection_open_session (Zik2Connection * conn)
{
  Zik2Message *msg;
  gboolean ret;

  msg = zik2_message_new_open_session ();
  ret = zik2_connection_send_message (conn, msg, NULL);
  zik2_message_free (msg);

  return ret;
}

gboolean
zik2_connection_send_message (Zik2Connection * conn, Zik2Message * msg,
    Zik2Message ** out_answer)
{
  gboolean ret = FALSE;
  GError *error = NULL;
  guint8 *data;
  gsize size;
  gssize sbytes, rbytes;
  Zik2Message *answer;

  data = zik2_message_make_buffer (msg, &size);

  /* send data */
  sbytes = g_socket_send (conn->socket, (gchar *) data, size, NULL, &error);
  if (sbytes < 0) {
    g_critical ("Zik2Connection %p: failed to send data to socket: %s",
        conn, error->message);
    g_error_free (error);
    goto done;
  } else if ((gsize) sbytes < size) {
    g_warning ("Zik2Connection %p: failed to send all data: %" G_GSSIZE_FORMAT
        "/%" G_GSIZE_FORMAT, conn, sbytes, size);
    goto done;
  }

  /* wait for answer */
  rbytes = g_socket_receive_with_blocking (conn->socket,
      (gchar *) conn->recv_buffer, conn->recv_buffer_size, TRUE, NULL, &error);
  if (rbytes < 0) {
    g_critical ("Zik2Connection %p: failed to receive data from socket: %s",
        conn, error->message);
    g_error_free (error);
    goto done;
  } else if (rbytes == 0) {
    g_warning ("Zik2Connection %p: connection was closed while receiving",
        conn);
    goto done;
  } else if (rbytes < 3) {
    g_warning ("Zik2Connection %p: not enough data in answer: %" G_GSSIZE_FORMAT,
        conn, rbytes);
  }

  answer = zik2_message_new_from_buffer (conn->recv_buffer, rbytes);
  if (answer == NULL) {
    g_warning ("Zik2Connection %p: failed to make message from received buffer",
        conn);
    goto done;
  }

  /* depending on the sent message, it could be an ack or a request answer */
  if (!zik2_message_is_acknowledge (answer) &&
      !zik2_message_is_request (answer)) {
    g_warning ("Zik2Connection %p: bad answer %02x %02x %02x", conn,
        conn->recv_buffer[0], conn->recv_buffer[1], conn->recv_buffer[2]);
    zik2_message_free (answer);
    goto done;
  }

  if (out_answer != NULL)
    *out_answer = answer;
  else
    zik2_message_free (answer);

  ret = TRUE;

done:
  g_free (data);
  return ret;
}
