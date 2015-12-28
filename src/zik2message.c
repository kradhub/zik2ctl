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

#include <stdlib.h>
#include <string.h>

#include "zik2message.h"
#include "zik2info.h"

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

  gchar *payload;
  gsize payload_size;
};

struct _Zik2RequestReplyData
{
  GNode *root;
};

/* MessageReply XML parsing */
typedef struct
{
  GNode *root;
  GNode *parent;

  gboolean finished;
} ParserData;

static void
zik2_xml_parser_start_element (GMarkupParseContext * context,
    const gchar * element_name, const gchar ** attribute_names,
    const gchar ** attribute_values, gpointer userdata, GError ** error)
{
  ParserData *data = (ParserData *) userdata;
  GSList *stack;

  stack = (GSList *) g_markup_parse_context_get_element_stack (context);

  if (data->finished)
    return;

  if (g_strcmp0 (element_name, "answer") == 0) {
    gchar *path;
    gboolean err;

    if (g_slist_length (stack) > 1) {
      /* answer shall be first */
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<answer> elements can only be top-level element");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error, G_MARKUP_COLLECT_STRING, "path", &path,
          G_MARKUP_COLLECT_BOOLEAN | G_MARKUP_COLLECT_OPTIONAL, "error", &err,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->root = g_node_new (zik2_answer_info_new (path, err));
    data->parent = data->root;
  } else if (g_strcmp0 (element_name, "audio") == 0) {
    if (g_slist_length (stack) < 2) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<audio> element should be embedded in <answer>");
      return;
    }

    /* no attribute to collect, yet */
    data->parent = g_node_append_data (data->parent, zik2_audio_info_new ());
  } else if (g_strcmp0 (element_name, "software") == 0) {
    gchar *sip6, *pic, *tts;

    if (g_slist_length (stack) < 2) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<software> element should be embedded in <answer>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error, G_MARKUP_COLLECT_STRING, "sip6", &sip6,
          G_MARKUP_COLLECT_STRING, "pic", &pic,
          G_MARKUP_COLLECT_STRING, "tts", &tts,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent,
        zik2_software_info_new (sip6, pic, tts));
  } else if (g_strcmp0 (element_name, "system") == 0) {
    gchar *pi;

    if (g_slist_length (stack) < 2) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<system> element should be embedded in <answer>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error,
          G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "pi", &pi,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent, zik2_system_info_new (pi));
  } else if (g_strcmp0 (element_name, "noise_control") == 0) {
    gboolean enabled;
    gchar *type;
    gchar *valstr;
    guint value = 0;

    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "audio")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<noise_control> element should be embedded in <audio>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error,
          G_MARKUP_COLLECT_BOOLEAN | G_MARKUP_COLLECT_OPTIONAL, "enabled", &enabled,
          G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "type", &type,
          G_MARKUP_COLLECT_STRING | G_MARKUP_COLLECT_OPTIONAL, "value", &valstr,
          G_MARKUP_COLLECT_INVALID))
      return;

    if (valstr)
      value = atoi (valstr);

    data->parent = g_node_append_data (data->parent,
        zik2_noise_control_info_new (enabled, type, value));
  } else if (g_strcmp0 (element_name, "source") == 0) {
    gchar *type;

    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "audio")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<source> element should be embedded in <audio>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error, G_MARKUP_COLLECT_STRING, "type", &type,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent,
        zik2_source_info_new (type));
  } else if (g_strcmp0 (element_name, "battery") == 0) {
    gchar *state;
    gchar *percent_str;

    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "system")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<battery> element should be embedded in <system>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error, G_MARKUP_COLLECT_STRING, "state", &state,
          G_MARKUP_COLLECT_STRING, "percent", &percent_str,
          G_MARKUP_COLLECT_STRING, "timeleft", NULL,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent,
        zik2_battery_info_new (state, atoi (percent_str)));
  } else if (g_strcmp0 (element_name, "volume") == 0) {
    gchar *value;

    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "audio")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<volume> element should be embedded in <audio>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error, G_MARKUP_COLLECT_STRING, "value", &value,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent,
        zik2_volume_info_new (atoi (value)));
  } else if (g_strcmp0 (element_name, "head_detection") == 0) {
    gboolean enabled;

    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "system")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<head_detection> element should be embedded in <system>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error, G_MARKUP_COLLECT_BOOLEAN, "enabled", &enabled,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent,
        zik2_head_detection_info_new (enabled));
  } else if (g_strcmp0 (element_name, "color") == 0) {
    gchar *value;

    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "system")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<color> element should be embedded in <system>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error, G_MARKUP_COLLECT_STRING, "value", &value,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent,
        zik2_color_info_new (atoi (value)));
  } else if (g_strcmp0 (element_name, "flight_mode") == 0) {
    gboolean value;

    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "answer")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<flight_mode> element should be embedded in <answer>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error, G_MARKUP_COLLECT_BOOLEAN, "enabled", &value,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent,
        zik2_flight_mode_info_new (value));
  } else if (g_strcmp0 (element_name, "bluetooth") == 0) {
    const gchar *value;

    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "answer")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<bluetooth> element should be embedded in <answer>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error, G_MARKUP_COLLECT_STRING, "friendlyname",
          &value, G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent,
        zik2_bluetooth_info_new (value));
  } else if (g_strcmp0 (element_name, "sound_effect") == 0) {
    gboolean enabled;
    const gchar *room_size;
    const gchar *angle;

    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "audio")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<sound_effect> element should be embedded in <audio>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error,
          G_MARKUP_COLLECT_BOOLEAN, "enabled", &enabled,
          G_MARKUP_COLLECT_STRING, "room_size", &room_size,
          G_MARKUP_COLLECT_STRING, "angle", &angle,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent,
        zik2_sound_effect_info_new (enabled, room_size, atoi (angle)));
  } else if (g_strcmp0 (element_name, "auto_connection") == 0) {
    gboolean enabled;

    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "system")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<auto_connection> element should be embedded in <system>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error,
          G_MARKUP_COLLECT_BOOLEAN, "enabled", &enabled,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent,
        zik2_auto_connection_info_new (enabled));
  } else if (g_strcmp0 (element_name, "track") == 0) {
    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "audio")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<track> element should be embedded in <audio>");
      return;
    }

    data->parent = g_node_append_data (data->parent, zik2_track_info_new ());
  } else if (g_strcmp0 (element_name, "metadata") == 0) {
    gboolean playing;
    const gchar *title;
    const gchar *artist;
    const gchar *album;
    const gchar *genre;

    if (g_slist_length (stack) < 2 || g_strcmp0 (stack->next->data, "track")) {
      g_set_error_literal (error, G_MARKUP_ERROR,
          G_MARKUP_ERROR_INVALID_CONTENT,
          "<metadata> element should be embedded in <track>");
      return;
    }

    if (!g_markup_collect_attributes (element_name, attribute_names,
          attribute_values, error,
          G_MARKUP_COLLECT_BOOLEAN, "playing", &playing,
          G_MARKUP_COLLECT_STRING, "title", &title,
          G_MARKUP_COLLECT_STRING, "artist", &artist,
          G_MARKUP_COLLECT_STRING, "album", &album,
          G_MARKUP_COLLECT_STRING, "genre", &genre,
          G_MARKUP_COLLECT_INVALID))
      return;

    data->parent = g_node_append_data (data->parent,
        zik2_metadata_info_new (playing, title, artist, album, genre));
  }
}

static void
zik2_xml_parser_end_element (GMarkupParseContext * context,
    const gchar * element_name, gpointer userdata, GError **error)
{
  ParserData *data = (ParserData *) userdata;

  if (g_strcmp0 (element_name, "answer") == 0) {
    data->finished = TRUE;
    data->parent = NULL;
  } else {
    data->parent = data->parent->parent;
  }
}

static void
zik2_xml_parser_error (GMarkupParseContext * context, GError * error,
    gpointer userdata)
{
  gint line_number;
  gint char_number;

  g_markup_parse_context_get_position (context, &line_number, &char_number);

  g_prefix_error (&error, "%d:%d: ", line_number, char_number);
}

static const GMarkupParser zik2_request_reply_xml_parser_cbs = {
  .start_element = zik2_xml_parser_start_element,
  .end_element = zik2_xml_parser_end_element,
  .text = NULL,
  .passthrough = NULL,
  .error = zik2_xml_parser_error
};


/** Zik2Message API */

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

/* arg is the string that follow arg=%s and could be NULL */
Zik2Message *
zik2_message_new_request (const gchar * path, const gchar * method,
    const gchar * args)
{
  Zik2Message *msg;

  msg = g_slice_new0 (Zik2Message);
  msg->id = ZIK2_MESSAGE_ID_REQ;

  if (args)
    msg->payload = g_strdup_printf ("GET %s/%s?arg=%s", path, method, args);
  else
    msg->payload = g_strdup_printf ("GET %s/%s", path, method);

  msg->payload_size = strlen ((gchar *) msg->payload);

  return msg;
}

gboolean
zik2_message_is_request (Zik2Message * msg)
{
  return msg->id == ZIK2_MESSAGE_ID_REQ;
}

gboolean
zik2_message_parse_request_reply (Zik2Message * msg,
    Zik2RequestReplyData ** reply)
{
  Zik2RequestReplyData *result;
  GMarkupParseContext *parser;
  ParserData pdata;
  gchar *xml;
  gssize xml_size;
  GError *error = NULL;
  gboolean ret = FALSE;

  g_return_val_if_fail (zik2_message_is_request (msg), FALSE);

  result = g_slice_new0 (Zik2RequestReplyData);

  /* request reply seems to begin with 0x01 0x01 and the payload size again
   * on the two following bytes in network bytes order
   * hence xml size is payload_size - 4 */
  xml = msg->payload + 4;
  xml_size = msg->payload_size - 4;

  memset (&pdata, 0, sizeof (pdata));

  parser = g_markup_parse_context_new (&zik2_request_reply_xml_parser_cbs, 0,
      &pdata, NULL);
  if (!g_markup_parse_context_parse (parser, xml, xml_size, &error)) {
    g_critical ("failed to parse request reply: %s", error->message);
    /* use result to ease cleanup */
    result->root = pdata.root;
    zik2_request_reply_data_free (result);
    g_error_free (error);
    goto out;
  }

  if (!g_markup_parse_context_end_parse (parser, &error)) {
    /* ignore error */
    g_error_free (error);
  }

  result->root = pdata.root;
  *reply = result;

  ret = TRUE;

out:
  g_markup_parse_context_free (parser);
  return ret;
}

gchar *
zik2_message_get_request_reply_xml (Zik2Message * msg)
{
  gchar *xml;
  gsize xml_size;

  g_return_val_if_fail (zik2_message_is_request (msg), FALSE);

  xml = msg->payload + 4;
  xml_size = msg->payload_size - 4;

  return g_strndup (xml, xml_size);
}

/** Zik2RequestReplyData API */
static gboolean
zik2_request_reply_data_free_node (GNode * node, gpointer userdata)
{
  /* first field of node data is a GType */
  GType *type = (GType *) node->data;

  g_boxed_free (*type, node->data);

  /* don't stop traversal */
  return FALSE;
}

void
zik2_request_reply_data_free (Zik2RequestReplyData * reply)
{
  g_node_traverse (reply->root, G_PRE_ORDER, G_TRAVERSE_ALL, -1,
      zik2_request_reply_data_free_node, NULL);
  g_node_destroy (reply->root);
  g_slice_free (Zik2RequestReplyData, reply);
}

typedef struct
{
  GType type;
  GNode *result;
} FindNodeFuncData;

static gboolean
zik2_request_reply_data_find_node_func (GNode * node, gpointer userdata)
{
  FindNodeFuncData *data = (FindNodeFuncData *) userdata;
  GType *type = (GType *) node->data;

  if (*type == data->type) {
    data->result = node;
    return TRUE;
  }

  /* continue traverse */
  return FALSE;
}

/* transfer none */
gpointer
zik2_request_reply_data_find_node_info (Zik2RequestReplyData * reply,
    GType type)
{
  GNode *node;
  FindNodeFuncData data;

  memset (&data, 0, sizeof (data));
  data.type = type;

  g_node_traverse (reply->root, G_PRE_ORDER, G_TRAVERSE_ALL, -1,
      zik2_request_reply_data_find_node_func, &data);

  node = data.result;

  if (node == NULL)
    return NULL;

  return node->data;
}

gboolean
zik2_request_reply_data_error (Zik2RequestReplyData * reply)
{
  Zik2AnswerInfo *info;

  g_return_val_if_fail (reply != NULL, FALSE);
  g_return_val_if_fail (reply->root != NULL, FALSE);
  info = reply->root->data;
  g_return_val_if_fail (info->itype == ZIK2_ANSWER_INFO_TYPE, FALSE);

  return info->error;
}
