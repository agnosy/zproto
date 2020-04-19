/*  =========================================================================
    zproto_example - zproto example protocol

    Codec class for zproto_example.

    ** WARNING *************************************************************
    THIS SOURCE FILE IS 100% GENERATED. If you edit this file, you will lose
    your changes at the next build cycle. This is great for temporary printf
    statements. DO NOT MAKE ANY CHANGES YOU WISH TO KEEP. The correct places
    for commits are:

     * The XML model used for this code generation: zproto_example.xml, or
     * The code generation script that built this file: zproto_codec_c
    ************************************************************************
    Copyright (C) 2014 the Authors

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
    =========================================================================
*/

/*
@header
    zproto_example - zproto example protocol
@discuss
@end
*/

#ifdef NDEBUG
#undef NDEBUG
#endif

#include "../include/zproto_example.h"

//  Structure of our class

struct _zproto_example_t {
    zframe_t *routing_id;               //  Routing_id from ROUTER, if any
    int id;                             //  zproto_example message ID
    byte *needle;                       //  Read/write pointer for serialization
    byte *ceiling;                      //  Valid upper limit for read pointer
    uint16_t sequence;                  //  sequence
    byte level;                         //  Log severity level
    byte event;                         //  Type of event
    uint16_t node;                      //  Sending node
    uint16_t peer;                      //  Refers to this peer
    uint64_t time;                      //  Log date/time
    char host [256];                    //  Originating hostname
    char *data;                         //  Actual log message
    zlist_t *aliases;                   //  List of strings
    zhash_t *headers;                   //  Other random properties
    size_t headers_bytes;               //  Size of hash content
    byte flags [4];                     //  A set of flags
    zchunk_t *public_key;               //  Our public key
    zuuid_t *identifier;                //  Unique identity
    zframe_t *address;                  //  Return address as frame
    zmsg_t *content;                    //  Message to be delivered
    char client_forename [256];         //  Given name
    char client_surname [256];          //  Family name
    char client_mobile [256];           //  Mobile phone number
    char client_email [256];            //  Email address
    char supplier_forename [256];       //  Given name
    char supplier_surname [256];        //  Family name
    char supplier_mobile [256];         //  Mobile phone number
    char supplier_email [256];          //  Email address
};

//  --------------------------------------------------------------------------
//  Network data encoding macros

//  Put a block of octets to the frame
#define PUT_OCTETS(host,size) { \
    memcpy (self->needle, (host), size); \
    self->needle += size; \
}

//  Get a block of octets from the frame
#define GET_OCTETS(host,size) { \
    if (self->needle + size > self->ceiling) { \
        zsys_warning ("zproto_example: GET_OCTETS failed"); \
        goto malformed; \
    } \
    memcpy ((host), self->needle, size); \
    self->needle += size; \
}

//  Put a 1-byte number to the frame
#define PUT_NUMBER1(host) { \
    *(byte *) self->needle = (byte) (host); \
    self->needle++; \
}

//  Put a 2-byte number to the frame
#define PUT_NUMBER2(host) { \
    self->needle [0] = (byte) (((host) >> 8)  & 255); \
    self->needle [1] = (byte) (((host))       & 255); \
    self->needle += 2; \
}

//  Put a 4-byte number to the frame
#define PUT_NUMBER4(host) { \
    self->needle [0] = (byte) (((host) >> 24) & 255); \
    self->needle [1] = (byte) (((host) >> 16) & 255); \
    self->needle [2] = (byte) (((host) >> 8)  & 255); \
    self->needle [3] = (byte) (((host))       & 255); \
    self->needle += 4; \
}

//  Put a 8-byte number to the frame
#define PUT_NUMBER8(host) { \
    self->needle [0] = (byte) (((host) >> 56) & 255); \
    self->needle [1] = (byte) (((host) >> 48) & 255); \
    self->needle [2] = (byte) (((host) >> 40) & 255); \
    self->needle [3] = (byte) (((host) >> 32) & 255); \
    self->needle [4] = (byte) (((host) >> 24) & 255); \
    self->needle [5] = (byte) (((host) >> 16) & 255); \
    self->needle [6] = (byte) (((host) >> 8)  & 255); \
    self->needle [7] = (byte) (((host))       & 255); \
    self->needle += 8; \
}

//  Get a 1-byte number from the frame
#define GET_NUMBER1(host) { \
    if (self->needle + 1 > self->ceiling) { \
        zsys_warning ("zproto_example: GET_NUMBER1 failed"); \
        goto malformed; \
    } \
    (host) = *(byte *) self->needle; \
    self->needle++; \
}

//  Get a 2-byte number from the frame
#define GET_NUMBER2(host) { \
    if (self->needle + 2 > self->ceiling) { \
        zsys_warning ("zproto_example: GET_NUMBER2 failed"); \
        goto malformed; \
    } \
    (host) = ((uint16_t) (self->needle [0]) << 8) \
           +  (uint16_t) (self->needle [1]); \
    self->needle += 2; \
}

//  Get a 4-byte number from the frame
#define GET_NUMBER4(host) { \
    if (self->needle + 4 > self->ceiling) { \
        zsys_warning ("zproto_example: GET_NUMBER4 failed"); \
        goto malformed; \
    } \
    (host) = ((uint32_t) (self->needle [0]) << 24) \
           + ((uint32_t) (self->needle [1]) << 16) \
           + ((uint32_t) (self->needle [2]) << 8) \
           +  (uint32_t) (self->needle [3]); \
    self->needle += 4; \
}

//  Get a 8-byte number from the frame
#define GET_NUMBER8(host) { \
    if (self->needle + 8 > self->ceiling) { \
        zsys_warning ("zproto_example: GET_NUMBER8 failed"); \
        goto malformed; \
    } \
    (host) = ((uint64_t) (self->needle [0]) << 56) \
           + ((uint64_t) (self->needle [1]) << 48) \
           + ((uint64_t) (self->needle [2]) << 40) \
           + ((uint64_t) (self->needle [3]) << 32) \
           + ((uint64_t) (self->needle [4]) << 24) \
           + ((uint64_t) (self->needle [5]) << 16) \
           + ((uint64_t) (self->needle [6]) << 8) \
           +  (uint64_t) (self->needle [7]); \
    self->needle += 8; \
}

//  Put a string to the frame
#define PUT_STRING(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER1 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a string from the frame
#define GET_STRING(host) { \
    size_t string_size; \
    GET_NUMBER1 (string_size); \
    if (self->needle + string_size > (self->ceiling)) { \
        zsys_warning ("zproto_example: GET_STRING failed"); \
        goto malformed; \
    } \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}

//  Put a long string to the frame
#define PUT_LONGSTR(host) { \
    size_t string_size = strlen (host); \
    PUT_NUMBER4 (string_size); \
    memcpy (self->needle, (host), string_size); \
    self->needle += string_size; \
}

//  Get a long string from the frame
#define GET_LONGSTR(host) { \
    size_t string_size; \
    GET_NUMBER4 (string_size); \
    if (self->needle + string_size > (self->ceiling)) { \
        zsys_warning ("zproto_example: GET_LONGSTR failed"); \
        goto malformed; \
    } \
    free ((host)); \
    (host) = (char *) malloc (string_size + 1); \
    memcpy ((host), self->needle, string_size); \
    (host) [string_size] = 0; \
    self->needle += string_size; \
}

//  --------------------------------------------------------------------------
//  bytes cstring conversion macros

// create new array of unsigned char from properly encoded string
// len of resulting array is strlen (str) / 2
// caller is responsibe for freeing up the memory
#define BYTES_FROM_STR(dst, _str) { \
    char *str = (char*) (_str); \
    if (!str || (strlen (str) % 2) != 0) \
        return NULL; \
\
    size_t strlen_2 = strlen (str) / 2; \
    byte *mem = (byte*) zmalloc (strlen_2); \
    size_t i; \
\
    for (i = 0; i != strlen_2; i++) \
    { \
        char buff[3] = {0x0, 0x0, 0x0}; \
        strncpy (buff, str, 2); \
        unsigned int uint; \
        sscanf (buff, "%x", &uint); \
        assert (uint <= 0xff); \
        mem [i] = (byte) (0xff & uint); \
        str += 2; \
    } \
    dst = mem; \
}

// convert len bytes to hex string
// caller is responsibe for freeing up the memory
#define STR_FROM_BYTES(dst, _mem, _len) { \
    byte *mem = (byte*) (_mem); \
    size_t len = (size_t) (_len); \
    char* ret = (char*) zmalloc (2*len + 1); \
    char* aux = ret; \
    size_t i; \
    for (i = 0; i != len; i++) \
    { \
        sprintf (aux, "%02x", mem [i]); \
        aux+=2; \
    } \
    dst = ret; \
}

//  --------------------------------------------------------------------------
//  Create a new zproto_example

zproto_example_t *
zproto_example_new (void)
{
    zproto_example_t *self = (zproto_example_t *) zmalloc (sizeof (zproto_example_t));
    return self;
}

//  --------------------------------------------------------------------------
//  Create a new zproto_example from zpl/zconfig_t *

zproto_example_t *
    zproto_example_new_zpl (zconfig_t *config)
{
    assert (config);
    zproto_example_t *self = NULL;
    char *message = zconfig_get (config, "message", NULL);
    if (!message) {
        zsys_error ("Can't find 'message' section");
        return NULL;
    }

    if (streq ("ZPROTO_EXAMPLE_LOG", message)) {
        self = zproto_example_new ();
        zproto_example_set_id (self, ZPROTO_EXAMPLE_LOG);
    }
    else
    if (streq ("ZPROTO_EXAMPLE_STRUCTURES", message)) {
        self = zproto_example_new ();
        zproto_example_set_id (self, ZPROTO_EXAMPLE_STRUCTURES);
    }
    else
    if (streq ("ZPROTO_EXAMPLE_BINARY", message)) {
        self = zproto_example_new ();
        zproto_example_set_id (self, ZPROTO_EXAMPLE_BINARY);
    }
    else
    if (streq ("ZPROTO_EXAMPLE_TYPES", message)) {
        self = zproto_example_new ();
        zproto_example_set_id (self, ZPROTO_EXAMPLE_TYPES);
    }
    else
       {
        zsys_error ("message=%s is not known", message);
        return NULL;
       }

    char *s = zconfig_get (config, "routing_id", NULL);
    if (s) {
        byte *bvalue;
        BYTES_FROM_STR (bvalue, s);
        if (!bvalue) {
            zproto_example_destroy (&self);
            return NULL;
        }
        zframe_t *frame = zframe_new (bvalue, strlen (s) / 2);
        free (bvalue);
        self->routing_id = frame;
    }


    zconfig_t *content = NULL;
    switch (self->id) {
        case ZPROTO_EXAMPLE_LOG:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                zproto_example_destroy (&self);
                return NULL;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "sequence", NULL);
            if (!s) {
                zsys_error ("content/sequence not found");
                zproto_example_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/sequence: %s is not a number", s);
                zproto_example_destroy (&self);
                return NULL;
            }
            self->sequence = uvalue;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "level", NULL);
            if (!s) {
                zsys_error ("content/level not found");
                zproto_example_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/level: %s is not a number", s);
                zproto_example_destroy (&self);
                return NULL;
            }
            self->level = uvalue;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "event", NULL);
            if (!s) {
                zsys_error ("content/event not found");
                zproto_example_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/event: %s is not a number", s);
                zproto_example_destroy (&self);
                return NULL;
            }
            self->event = uvalue;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "node", NULL);
            if (!s) {
                zsys_error ("content/node not found");
                zproto_example_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/node: %s is not a number", s);
                zproto_example_destroy (&self);
                return NULL;
            }
            self->node = uvalue;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "peer", NULL);
            if (!s) {
                zsys_error ("content/peer not found");
                zproto_example_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/peer: %s is not a number", s);
                zproto_example_destroy (&self);
                return NULL;
            }
            self->peer = uvalue;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "time", NULL);
            if (!s) {
                zsys_error ("content/time not found");
                zproto_example_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/time: %s is not a number", s);
                zproto_example_destroy (&self);
                return NULL;
            }
            self->time = uvalue;
            }
            {
            char *s = zconfig_get (content, "host", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            strncpy (self->host, s, 255);
            }
            {
            char *s = zconfig_get (content, "data", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            self->data = strdup (s);
            }
            break;
        case ZPROTO_EXAMPLE_STRUCTURES:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                zproto_example_destroy (&self);
                return NULL;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "sequence", NULL);
            if (!s) {
                zsys_error ("content/sequence not found");
                zproto_example_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/sequence: %s is not a number", s);
                zproto_example_destroy (&self);
                return NULL;
            }
            self->sequence = uvalue;
            }
            {
            zconfig_t *zstrings = zconfig_locate (content, "aliases");
            if (zstrings) {
                zlist_t *strings = zlist_new ();
                zlist_autofree (strings);
                zconfig_t *child;
                for (child = zconfig_child (zstrings);
                                child != NULL;
                                child = zconfig_next (child))
                {
                    zlist_append (strings, zconfig_value (child));
                }
                self->aliases = strings;
            }
            }
            {
            zconfig_t *zhash = zconfig_locate (content, "headers");
            if (zhash) {
                zhash_t *hash = zhash_new ();
                zhash_autofree (hash);
                zconfig_t *child;
                for (child = zconfig_child (zhash);
                                child != NULL;
                                child = zconfig_next (child))
                {
                    zhash_update (hash, zconfig_name (child), zconfig_value (child));
                }
                self->headers = hash;
            }
            }
            break;
        case ZPROTO_EXAMPLE_BINARY:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                zproto_example_destroy (&self);
                return NULL;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "sequence", NULL);
            if (!s) {
                zsys_error ("content/sequence not found");
                zproto_example_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/sequence: %s is not a number", s);
                zproto_example_destroy (&self);
                return NULL;
            }
            self->sequence = uvalue;
            }
            {
            char *s = zconfig_get (content, "flags", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            byte *bvalue;
            BYTES_FROM_STR (bvalue, s);
            if (!bvalue) {
                zproto_example_destroy (&self);
                return NULL;
            }
            self->flags = bvalue;
            }
            {
            char *s = zconfig_get (content, "public_key", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            byte *bvalue;
            BYTES_FROM_STR (bvalue, s);
            if (!bvalue) {
                zproto_example_destroy (&self);
                return NULL;
            }
            zchunk_t *chunk = zchunk_new (bvalue, strlen (s) / 2);
            free (bvalue);
            self->public_key = chunk;
            }
            {
            char *s = zconfig_get (content, "identifier", NULL);
            if (s) {
                zuuid_t *uuid = zuuid_new ();
                zuuid_set_str (uuid, s);
                self->identifier = uuid;
            }
            }
            {
            char *s = zconfig_get (content, "address", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            byte *bvalue;
            BYTES_FROM_STR (bvalue, s);
            if (!bvalue) {
                zproto_example_destroy (&self);
                return NULL;
            }
            zframe_t *frame = zframe_new (bvalue, strlen (s) / 2);
            self->address = frame;
            free (bvalue);
            }

            {
            char *s = zconfig_get (content, "content", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            byte *bvalue;
            BYTES_FROM_STR (bvalue, s);
            if (!bvalue) {
                zproto_example_destroy (&self);
                return NULL;
            }
#if CZMQ_VERSION_MAJOR == 4
            zframe_t *frame = zframe_new (bvalue, strlen (s) / 2);
            zmsg_t *msg = zmsg_decode (frame);
            zframe_destroy (&frame);
#else
            zmsg_t *msg = zmsg_decode (bvalue, strlen (s) / 2);
#endif
            free (bvalue);
            self->content = msg;
            }
            break;
        case ZPROTO_EXAMPLE_TYPES:
            content = zconfig_locate (config, "content");
            if (!content) {
                zsys_error ("Can't find 'content' section");
                zproto_example_destroy (&self);
                return NULL;
            }
            {
            char *es = NULL;
            char *s = zconfig_get (content, "sequence", NULL);
            if (!s) {
                zsys_error ("content/sequence not found");
                zproto_example_destroy (&self);
                return NULL;
            }
            uint64_t uvalue = (uint64_t) strtoll (s, &es, 10);
            if (es != s+strlen (s)) {
                zsys_error ("content/sequence: %s is not a number", s);
                zproto_example_destroy (&self);
                return NULL;
            }
            self->sequence = uvalue;
            }
            {
            char *s = zconfig_get (content, "client_forename", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            strncpy (self->client_forename, s, 255);
            }
            {
            char *s = zconfig_get (content, "client_surname", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            strncpy (self->client_surname, s, 255);
            }
            {
            char *s = zconfig_get (content, "client_mobile", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            strncpy (self->client_mobile, s, 255);
            }
            {
            char *s = zconfig_get (content, "client_email", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            strncpy (self->client_email, s, 255);
            }
            {
            char *s = zconfig_get (content, "supplier_forename", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            strncpy (self->supplier_forename, s, 255);
            }
            {
            char *s = zconfig_get (content, "supplier_surname", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            strncpy (self->supplier_surname, s, 255);
            }
            {
            char *s = zconfig_get (content, "supplier_mobile", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            strncpy (self->supplier_mobile, s, 255);
            }
            {
            char *s = zconfig_get (content, "supplier_email", NULL);
            if (!s) {
                zproto_example_destroy (&self);
                return NULL;
            }
            strncpy (self->supplier_email, s, 255);
            }
            break;
    }
    return self;
}


//  --------------------------------------------------------------------------
//  Destroy the zproto_example

void
zproto_example_destroy (zproto_example_t **self_p)
{
    assert (self_p);
    if (*self_p) {
        zproto_example_t *self = *self_p;

        //  Free class properties
        zframe_destroy (&self->routing_id);
        free (self->data);
        if (self->aliases)
            zlist_destroy (&self->aliases);
        zhash_destroy (&self->headers);
        zchunk_destroy (&self->public_key);
        zuuid_destroy (&self->identifier);
        zframe_destroy (&self->address);
        zmsg_destroy (&self->content);

        //  Free object itself
        free (self);
        *self_p = NULL;
    }
}


//  --------------------------------------------------------------------------
//  Create a deep copy of a zproto_example instance

zproto_example_t *
zproto_example_dup (zproto_example_t *other)
{
    assert (other);
    zproto_example_t *copy = zproto_example_new ();

    // Copy the routing and message id
    zproto_example_set_routing_id (copy, zproto_example_routing_id (other));
    zproto_example_set_id (copy, zproto_example_id (other));


    // Copy the rest of the fields
    zproto_example_set_sequence (copy, zproto_example_sequence (other));
    zproto_example_set_level (copy, zproto_example_level (other));
    zproto_example_set_event (copy, zproto_example_event (other));
    zproto_example_set_node (copy, zproto_example_node (other));
    zproto_example_set_peer (copy, zproto_example_peer (other));
    zproto_example_set_time (copy, zproto_example_time (other));
    zproto_example_set_host (copy, zproto_example_host (other));
    {
        const char *str = zproto_example_data (other);
        if (str) {
            zproto_example_set_data (copy, str);
        }
    }
    {
        zlist_t *lcopy = zlist_dup (zproto_example_aliases (other));
        zproto_example_set_aliases (copy, &lcopy);
    }
    {
        zhash_t *dup_hash = zhash_dup (zproto_example_headers (other));
        zproto_example_set_headers (copy, &dup_hash);
    }
    {
        zchunk_t *dup_chunk = zchunk_dup (zproto_example_public_key (other));
        zproto_example_set_public_key (copy, &dup_chunk);
    }
    zproto_example_set_identifier (copy, zproto_example_identifier (other));
    {
        zframe_t *dup_frame = zframe_dup (zproto_example_address (other));
        zproto_example_set_address (copy, &dup_frame);
    }
    {
        zmsg_t *dup_msg = zmsg_dup (zproto_example_content (other));
        zproto_example_set_content (copy, &dup_msg);
    }
    zproto_example_set_client_forename (copy, zproto_example_client_forename (other));
    zproto_example_set_client_surname (copy, zproto_example_client_surname (other));
    zproto_example_set_client_mobile (copy, zproto_example_client_mobile (other));
    zproto_example_set_client_email (copy, zproto_example_client_email (other));
    zproto_example_set_supplier_forename (copy, zproto_example_supplier_forename (other));
    zproto_example_set_supplier_surname (copy, zproto_example_supplier_surname (other));
    zproto_example_set_supplier_mobile (copy, zproto_example_supplier_mobile (other));
    zproto_example_set_supplier_email (copy, zproto_example_supplier_email (other));

    return copy;
}


//  --------------------------------------------------------------------------
//  Receive a zproto_example from the socket. Returns 0 if OK, -1 if
//  the recv was interrupted, or -2 if the message is malformed.
//  Blocks if there is no message waiting.

int
zproto_example_recv (zproto_example_t *self, zsock_t *input)
{
    assert (input);
    int rc = 0;
    zmq_msg_t frame;
    zmq_msg_init (&frame);

    if (zsock_type (input) == ZMQ_ROUTER) {
        zframe_destroy (&self->routing_id);
        self->routing_id = zframe_recv (input);
        if (!self->routing_id || !zsock_rcvmore (input)) {
            zsys_warning ("zproto_example: no routing ID");
            rc = -1;            //  Interrupted
            goto malformed;
        }
    }
    int size;
    size = zmq_msg_recv (&frame, zsock_resolve (input), 0);
    if (size == -1) {
        if (errno != EAGAIN)
            zsys_warning ("zproto_example: interrupted");
        rc = -1;                //  Interrupted
        goto malformed;
    }
    self->needle = (byte *) zmq_msg_data (&frame);
    self->ceiling = self->needle + zmq_msg_size (&frame);


    //  Get and check protocol signature
    uint16_t signature;
    GET_NUMBER2 (signature);
    if (signature != (0xAAA0 | 0)) {
        zsys_warning ("zproto_example: invalid signature");
        rc = -2;                //  Malformed
        goto malformed;
    }
    //  Get message id and parse per message type
    GET_NUMBER1 (self->id);

    switch (self->id) {
        case ZPROTO_EXAMPLE_LOG:
            GET_NUMBER2 (self->sequence);
            {
                uint16_t version;
                GET_NUMBER2 (version);
                if (version != 3) {
                    zsys_warning ("zproto_example: version is invalid");
                    rc = -2;    //  Malformed
                    goto malformed;
                }
            }
            GET_NUMBER1 (self->level);
            GET_NUMBER1 (self->event);
            GET_NUMBER2 (self->node);
            GET_NUMBER2 (self->peer);
            GET_NUMBER8 (self->time);
            GET_STRING (self->host);
            GET_LONGSTR (self->data);
            break;

        case ZPROTO_EXAMPLE_STRUCTURES:
            GET_NUMBER2 (self->sequence);
            {
                size_t list_size;
                GET_NUMBER4 (list_size);
                zlist_destroy (&self->aliases);
                self->aliases = zlist_new ();
                zlist_autofree (self->aliases);
                while (list_size--) {
                    char *string = NULL;
                    GET_LONGSTR (string);
                    zlist_append (self->aliases, string);
                    free (string);
                }
            }
            {
                size_t hash_size;
                GET_NUMBER4 (hash_size);
                zhash_destroy (&self->headers);
                self->headers = zhash_new ();
                zhash_autofree (self->headers);
                while (hash_size--) {
                    char key [256];
                    char *value = NULL;
                    GET_STRING (key);
                    GET_LONGSTR (value);
                    zhash_insert (self->headers, key, value);
                    free (value);
                }
            }
            break;

        case ZPROTO_EXAMPLE_BINARY:
            GET_NUMBER2 (self->sequence);
            GET_OCTETS (self->flags, 4);
            {
                size_t chunk_size;
                GET_NUMBER4 (chunk_size);
                if (self->needle + chunk_size > (self->ceiling)) {
                    zsys_warning ("zproto_example: public_key is missing data");
                    rc = -2;    //  Malformed
                    goto malformed;
                }
                zchunk_destroy (&self->public_key);
                self->public_key = zchunk_new (self->needle, chunk_size);
                self->needle += chunk_size;
            }
            if (self->needle + ZUUID_LEN > (self->ceiling)) {
                zsys_warning ("zproto_example: identifier is invalid");
                rc = -2;        //  Malformed
                goto malformed;
            }
            zuuid_destroy (&self->identifier);
            self->identifier = zuuid_new_from (self->needle);
            self->needle += ZUUID_LEN;
            //  Get next frame off socket
            if (!zsock_rcvmore (input)) {
                zsys_warning ("zproto_example: address is missing");
                rc = -2;        //  Malformed
                goto malformed;
            }
            zframe_destroy (&self->address);
            self->address = zframe_recv (input);
            //  Get zero or more remaining frames
            zmsg_destroy (&self->content);
            if (zsock_rcvmore (input))
                self->content = zmsg_recv (input);
            else
                self->content = zmsg_new ();
            break;

        case ZPROTO_EXAMPLE_TYPES:
            GET_NUMBER2 (self->sequence);
            GET_STRING (self->client_forename);
            GET_STRING (self->client_surname);
            GET_STRING (self->client_mobile);
            GET_STRING (self->client_email);
            GET_STRING (self->supplier_forename);
            GET_STRING (self->supplier_surname);
            GET_STRING (self->supplier_mobile);
            GET_STRING (self->supplier_email);
            break;

        default:
            zsys_warning ("zproto_example: bad message ID");
            rc = -2;            //  Malformed
            goto malformed;
    }
    //  Successful return
    zmq_msg_close (&frame);
    return rc;

    //  Error returns
    malformed:
        zmq_msg_close (&frame);
        return rc;              //  Invalid message
}


//  --------------------------------------------------------------------------
//  Send the zproto_example to the socket. Does not destroy it. Returns 0 if
//  OK, else -1.

int
zproto_example_send (zproto_example_t *self, zsock_t *output)
{
    assert (self);
    assert (output);

    if (zsock_type (output) == ZMQ_ROUTER)
        zframe_send (&self->routing_id, output, ZFRAME_MORE + ZFRAME_REUSE);

    size_t frame_size = 2 + 1;          //  Signature and message ID

    switch (self->id) {
        case ZPROTO_EXAMPLE_LOG:
            frame_size += 2;            //  sequence
            frame_size += 2;            //  version
            frame_size += 1;            //  level
            frame_size += 1;            //  event
            frame_size += 2;            //  node
            frame_size += 2;            //  peer
            frame_size += 8;            //  time
            frame_size += 1 + strlen (self->host);
            frame_size += 4;
            if (self->data)
                frame_size += strlen (self->data);
            break;
        case ZPROTO_EXAMPLE_STRUCTURES:
            frame_size += 2;            //  sequence
            frame_size += 4;            //  Size is 4 octets
            if (self->aliases) {
                char *aliases = (char *) zlist_first (self->aliases);
                while (aliases) {
                    frame_size += 4 + strlen (aliases);
                    aliases = (char *) zlist_next (self->aliases);
                }
            }
            frame_size += 4;            //  Size is 4 octets
            if (self->headers) {
                self->headers_bytes = 0;
                char *item = (char *) zhash_first (self->headers);
                while (item) {
                    self->headers_bytes += 1 + strlen (zhash_cursor (self->headers));
                    self->headers_bytes += 4 + strlen (item);
                    item = (char *) zhash_next (self->headers);
                }
            }
            frame_size += self->headers_bytes;
            break;
        case ZPROTO_EXAMPLE_BINARY:
            frame_size += 2;            //  sequence
            frame_size += 4;            //  flags
            frame_size += 4;            //  Size is 4 octets
            if (self->public_key)
                frame_size += zchunk_size (self->public_key);
            frame_size += ZUUID_LEN;    //  identifier
            break;
        case ZPROTO_EXAMPLE_TYPES:
            frame_size += 2;            //  sequence
            frame_size += 1 + strlen (self->client_forename);
            frame_size += 1 + strlen (self->client_surname);
            frame_size += 1 + strlen (self->client_mobile);
            frame_size += 1 + strlen (self->client_email);
            frame_size += 1 + strlen (self->supplier_forename);
            frame_size += 1 + strlen (self->supplier_surname);
            frame_size += 1 + strlen (self->supplier_mobile);
            frame_size += 1 + strlen (self->supplier_email);
            break;
    }

    zmq_msg_t frame;
    zmq_msg_init_size (&frame, frame_size);
    self->needle = (byte *) zmq_msg_data (&frame);

    //  Now serialize message into the frame
    PUT_NUMBER2 (0xAAA0 | 0);
    PUT_NUMBER1 (self->id);
    bool have_content = false;
    size_t nbr_frames = 1;              //  Total number of frames to send

    switch (self->id) {
        case ZPROTO_EXAMPLE_LOG:
            PUT_NUMBER2 (self->sequence);
            PUT_NUMBER2 (3);
            PUT_NUMBER1 (self->level);
            PUT_NUMBER1 (self->event);
            PUT_NUMBER2 (self->node);
            PUT_NUMBER2 (self->peer);
            PUT_NUMBER8 (self->time);
            PUT_STRING (self->host);
            if (self->data) {
                PUT_LONGSTR (self->data);
            }
            else
                PUT_NUMBER4 (0);    //  Empty string
            break;

        case ZPROTO_EXAMPLE_STRUCTURES:
            PUT_NUMBER2 (self->sequence);
            if (self->aliases) {
                PUT_NUMBER4 (zlist_size (self->aliases));
                char *aliases = (char *) zlist_first (self->aliases);
                while (aliases) {
                    PUT_LONGSTR (aliases);
                    aliases = (char *) zlist_next (self->aliases);
                }
            }
            else
                PUT_NUMBER4 (0);    //  Empty string array
            if (self->headers) {
                PUT_NUMBER4 (zhash_size (self->headers));
                char *item = (char *) zhash_first (self->headers);
                while (item) {
                    PUT_STRING (zhash_cursor (self->headers));
                    PUT_LONGSTR (item);
                    item = (char *) zhash_next (self->headers);
                }
            }
            else
                PUT_NUMBER4 (0);    //  Empty hash
            break;

        case ZPROTO_EXAMPLE_BINARY:
            PUT_NUMBER2 (self->sequence);
            PUT_OCTETS (self->flags, 4);
            if (self->public_key) {
                PUT_NUMBER4 (zchunk_size (self->public_key));
                memcpy (self->needle,
                        zchunk_data (self->public_key),
                        zchunk_size (self->public_key));
                self->needle += zchunk_size (self->public_key);
            }
            else
                PUT_NUMBER4 (0);    //  Empty chunk
            if (self->identifier)
                zuuid_export (self->identifier, self->needle);
            else
                memset (self->needle, 0, ZUUID_LEN);
            self->needle += ZUUID_LEN;
            nbr_frames++;
            nbr_frames += self->content? zmsg_size (self->content): 1;
            have_content = true;
            break;

        case ZPROTO_EXAMPLE_TYPES:
            PUT_NUMBER2 (self->sequence);
            PUT_STRING (self->client_forename);
            PUT_STRING (self->client_surname);
            PUT_STRING (self->client_mobile);
            PUT_STRING (self->client_email);
            PUT_STRING (self->supplier_forename);
            PUT_STRING (self->supplier_surname);
            PUT_STRING (self->supplier_mobile);
            PUT_STRING (self->supplier_email);
            break;

    }

    //  Now send the data frame
    zmq_msg_send (&frame, zsock_resolve (output), --nbr_frames? ZMQ_SNDMORE: 0);

    //  Now send any frame fields, in order
    if (self->id == ZPROTO_EXAMPLE_BINARY) {
        //  If address isn't set, send an empty frame
        if (self->address)
            zframe_send (&self->address, output, ZFRAME_REUSE + (--nbr_frames? ZFRAME_MORE: 0));
        else
            zmq_send (zsock_resolve (output), NULL, 0, (--nbr_frames? ZMQ_SNDMORE: 0));
    }
    //  Now send the content if necessary
    if (have_content) {
        if (self->content) {
            zframe_t *frame = zmsg_first (self->content);
            while (frame) {
                zframe_send (&frame, output, ZFRAME_REUSE + (--nbr_frames? ZFRAME_MORE: 0));
                frame = zmsg_next (self->content);
            }
        }
        else
            zmq_send (zsock_resolve (output), NULL, 0, 0);
    }
    return 0;
}

//  --------------------------------------------------------------------------
//  Encode the first frame of zproto_example. Does not destroy it. Returns the frame if
//  OK, else NULL.

zframe_t *
zproto_example_encode (zproto_example_t *self)
{
    assert (self);

    size_t frame_size = 2 + 1;          //  Signature and message ID

    switch (self->id) {
        case ZPROTO_EXAMPLE_LOG:
            frame_size += 2;            //  sequence
            frame_size += 2;            //  version
            frame_size += 1;            //  level
            frame_size += 1;            //  event
            frame_size += 2;            //  node
            frame_size += 2;            //  peer
            frame_size += 8;            //  time
            frame_size += 1 + strlen (self->host);
            frame_size += 4;
            if (self->data)
                frame_size += strlen (self->data);
            break;
        case ZPROTO_EXAMPLE_STRUCTURES:
            frame_size += 2;            //  sequence
            frame_size += 4;            //  Size is 4 octets
            if (self->aliases) {
                char *aliases = (char *) zlist_first (self->aliases);
                while (aliases) {
                    frame_size += 4 + strlen (aliases);
                    aliases = (char *) zlist_next (self->aliases);
                }
            }
            frame_size += 4;            //  Size is 4 octets
            if (self->headers) {
                self->headers_bytes = 0;
                char *item = (char *) zhash_first (self->headers);
                while (item) {
                    self->headers_bytes += 1 + strlen (zhash_cursor (self->headers));
                    self->headers_bytes += 4 + strlen (item);
                    item = (char *) zhash_next (self->headers);
                }
            }
            frame_size += self->headers_bytes;
            break;
        case ZPROTO_EXAMPLE_BINARY:
            frame_size += 2;            //  sequence
            frame_size += 4;            //  flags
            frame_size += 4;            //  Size is 4 octets
            if (self->public_key)
                frame_size += zchunk_size (self->public_key);
            frame_size += ZUUID_LEN;    //  identifier
            break;
        case ZPROTO_EXAMPLE_TYPES:
            frame_size += 2;            //  sequence
            frame_size += 1 + strlen (self->client_forename);
            frame_size += 1 + strlen (self->client_surname);
            frame_size += 1 + strlen (self->client_mobile);
            frame_size += 1 + strlen (self->client_email);
            frame_size += 1 + strlen (self->supplier_forename);
            frame_size += 1 + strlen (self->supplier_surname);
            frame_size += 1 + strlen (self->supplier_mobile);
            frame_size += 1 + strlen (self->supplier_email);
            break;
    }

    zframe_t *frame = zframe_new (NULL, frame_size);
    self->needle = (byte *) zframe_data (frame);

    //  Now serialize message into the frame
    PUT_NUMBER2 (0xAAA0 | 0);
    PUT_NUMBER1 (self->id);
    bool have_content = false;
    size_t nbr_frames = 1;              //  Total number of frames to send

    switch (self->id) {
        case ZPROTO_EXAMPLE_LOG:
            PUT_NUMBER2 (self->sequence);
            PUT_NUMBER2 (3);
            PUT_NUMBER1 (self->level);
            PUT_NUMBER1 (self->event);
            PUT_NUMBER2 (self->node);
            PUT_NUMBER2 (self->peer);
            PUT_NUMBER8 (self->time);
            PUT_STRING (self->host);
            if (self->data) {
                PUT_LONGSTR (self->data);
            }
            else
                PUT_NUMBER4 (0);    //  Empty string
            break;

        case ZPROTO_EXAMPLE_STRUCTURES:
            PUT_NUMBER2 (self->sequence);
            if (self->aliases) {
                PUT_NUMBER4 (zlist_size (self->aliases));
                char *aliases = (char *) zlist_first (self->aliases);
                while (aliases) {
                    PUT_LONGSTR (aliases);
                    aliases = (char *) zlist_next (self->aliases);
                }
            }
            else
                PUT_NUMBER4 (0);    //  Empty string array
            if (self->headers) {
                PUT_NUMBER4 (zhash_size (self->headers));
                char *item = (char *) zhash_first (self->headers);
                while (item) {
                    PUT_STRING (zhash_cursor (self->headers));
                    PUT_LONGSTR (item);
                    item = (char *) zhash_next (self->headers);
                }
            }
            else
                PUT_NUMBER4 (0);    //  Empty hash
            break;

        case ZPROTO_EXAMPLE_BINARY:
            PUT_NUMBER2 (self->sequence);
            PUT_OCTETS (self->flags, 4);
            if (self->public_key) {
                PUT_NUMBER4 (zchunk_size (self->public_key));
                memcpy (self->needle,
                        zchunk_data (self->public_key),
                        zchunk_size (self->public_key));
                self->needle += zchunk_size (self->public_key);
            }
            else
                PUT_NUMBER4 (0);    //  Empty chunk
            if (self->identifier)
                zuuid_export (self->identifier, self->needle);
            else
                memset (self->needle, 0, ZUUID_LEN);
            self->needle += ZUUID_LEN;
            nbr_frames++;
            nbr_frames += self->content? zmsg_size (self->content): 1;
            have_content = true;
            break;

        case ZPROTO_EXAMPLE_TYPES:
            PUT_NUMBER2 (self->sequence);
            PUT_STRING (self->client_forename);
            PUT_STRING (self->client_surname);
            PUT_STRING (self->client_mobile);
            PUT_STRING (self->client_email);
            PUT_STRING (self->supplier_forename);
            PUT_STRING (self->supplier_surname);
            PUT_STRING (self->supplier_mobile);
            PUT_STRING (self->supplier_email);
            break;

    }

    return frame;
}


//  --------------------------------------------------------------------------
//  Print contents of message to stdout

void
zproto_example_print (zproto_example_t *self)
{
    assert (self);
    switch (self->id) {
        case ZPROTO_EXAMPLE_LOG:
            zsys_debug ("ZPROTO_EXAMPLE_LOG:");
            zsys_debug ("    sequence=%ld", (long) self->sequence);
            zsys_debug ("    version=3");
            zsys_debug ("    level=%ld", (long) self->level);
            zsys_debug ("    event=%ld", (long) self->event);
            zsys_debug ("    node=%ld", (long) self->node);
            zsys_debug ("    peer=%ld", (long) self->peer);
            zsys_debug ("    time=%ld", (long) self->time);
            zsys_debug ("    host='%s'", self->host);
            if (self->data)
                zsys_debug ("    data='%s'", self->data);
            else
                zsys_debug ("    data=");
            break;

        case ZPROTO_EXAMPLE_STRUCTURES:
            zsys_debug ("ZPROTO_EXAMPLE_STRUCTURES:");
            zsys_debug ("    sequence=%ld", (long) self->sequence);
            zsys_debug ("    aliases=");
            if (self->aliases) {
                char *aliases = (char *) zlist_first (self->aliases);
                while (aliases) {
                    zsys_debug ("        '%s'", aliases);
                    aliases = (char *) zlist_next (self->aliases);
                }
            }
            zsys_debug ("    headers=");
            if (self->headers) {
                char *item = (char *) zhash_first (self->headers);
                while (item) {
                    zsys_debug ("        %s=%s", zhash_cursor (self->headers), item);
                    item = (char *) zhash_next (self->headers);
                }
            }
            else
                zsys_debug ("(NULL)");
            break;

        case ZPROTO_EXAMPLE_BINARY:
            zsys_debug ("ZPROTO_EXAMPLE_BINARY:");
            zsys_debug ("    sequence=%ld", (long) self->sequence);
            zsys_debug ("    flags=[ ... ]");
            zsys_debug ("    public_key=[ ... ]");
            zsys_debug ("    identifier=");
            if (self->identifier)
                zsys_debug ("        %s", zuuid_str_canonical (self->identifier));
            else
                zsys_debug ("        (NULL)");
            zsys_debug ("    address=");
            if (self->address)
                zframe_print (self->address, NULL);
            else
                zsys_debug ("(NULL)");
            zsys_debug ("    content=");
            if (self->content)
                zmsg_print (self->content);
            else
                zsys_debug ("(NULL)");
            break;

        case ZPROTO_EXAMPLE_TYPES:
            zsys_debug ("ZPROTO_EXAMPLE_TYPES:");
            zsys_debug ("    sequence=%ld", (long) self->sequence);
            zsys_debug ("    client_forename='%s'", self->client_forename);
            zsys_debug ("    client_surname='%s'", self->client_surname);
            zsys_debug ("    client_mobile='%s'", self->client_mobile);
            zsys_debug ("    client_email='%s'", self->client_email);
            zsys_debug ("    supplier_forename='%s'", self->supplier_forename);
            zsys_debug ("    supplier_surname='%s'", self->supplier_surname);
            zsys_debug ("    supplier_mobile='%s'", self->supplier_mobile);
            zsys_debug ("    supplier_email='%s'", self->supplier_email);
            break;

    }
}

//  --------------------------------------------------------------------------
//  Export class as zconfig_t*. Caller is responsibe for destroying the instance

zconfig_t *
zproto_example_zpl (zproto_example_t *self, zconfig_t *parent)
{
    assert (self);

    zconfig_t *root = zconfig_new ("zproto_example", parent);

    switch (self->id) {
        case ZPROTO_EXAMPLE_LOG:
        {
            zconfig_put (root, "message", "ZPROTO_EXAMPLE_LOG");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "sequence", "%ld", (long) self->sequence);
            zconfig_putf (config, "version", "%s", "3");
            zconfig_putf (config, "level", "%ld", (long) self->level);
            zconfig_putf (config, "event", "%ld", (long) self->event);
            zconfig_putf (config, "node", "%ld", (long) self->node);
            zconfig_putf (config, "peer", "%ld", (long) self->peer);
            zconfig_putf (config, "time", "%ld", (long) self->time);
            zconfig_putf (config, "host", "%s", self->host);
            if (self->data)
                zconfig_putf (config, "data", "%s", self->data);
            break;
            }
        case ZPROTO_EXAMPLE_STRUCTURES:
        {
            zconfig_put (root, "message", "ZPROTO_EXAMPLE_STRUCTURES");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "sequence", "%ld", (long) self->sequence);
            if (self->aliases) {
                zconfig_t *strings = zconfig_new ("aliases", config);
                size_t i = 0;
                char *aliases = (char *) zlist_first (self->aliases);
                while (aliases) {
                    char *key = zsys_sprintf ("%zu", i);
                    zconfig_putf (strings, key, "%s", aliases);
                    zstr_free (&key);
                    i++;
                    aliases = (char *) zlist_next (self->aliases);
                }
            }
            if (self->headers) {
                zconfig_t *hash = zconfig_new ("headers", config);
                char *item = (char *) zhash_first (self->headers);
                while (item) {
                    zconfig_putf (hash, zhash_cursor (self->headers), "%s", item);
                    item = (char *) zhash_next (self->headers);
                }
            }
            break;
            }
        case ZPROTO_EXAMPLE_BINARY:
        {
            zconfig_put (root, "message", "ZPROTO_EXAMPLE_BINARY");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "sequence", "%ld", (long) self->sequence);
            {
            char *hex = NULL;
            STR_FROM_BYTES (hex, self->flags, 4);
            zconfig_putf (config, "flags", "%s", hex);
            zstr_free (&hex);
            }
            {
            char *hex = NULL;
            STR_FROM_BYTES (hex, zchunk_data (self->public_key), zchunk_size (self->public_key));
            zconfig_putf (config, "public_key", "%s", hex);
            zstr_free (&hex);
            }
            if (self->identifier)
                zconfig_putf (config, "identifier", "%s", zuuid_str (self->identifier));
            {
            char *hex = NULL;
            STR_FROM_BYTES (hex, zframe_data (self->address), zframe_size (self->address));
            zconfig_putf (config, "address", "%s", hex);
            zstr_free (&hex);
            }
            {
            char *hex = NULL;
#if CZMQ_VERSION_MAJOR == 4
            zframe_t *frame = zmsg_encode (self->content);
            STR_FROM_BYTES (hex, zframe_data (frame), zframe_size (frame));
            zconfig_putf (config, "content", "%s", hex);
            zstr_free (&hex);
            zframe_destroy (&frame);
#else
            byte *buffer;
            size_t size = zmsg_encode (self->content, &buffer);
            STR_FROM_BYTES (hex, buffer, size);
            zconfig_putf (config, "content", "%s", hex);
            zstr_free (&hex);
            free (buffer); buffer= NULL;
#endif
            }
            break;
            }
        case ZPROTO_EXAMPLE_TYPES:
        {
            zconfig_put (root, "message", "ZPROTO_EXAMPLE_TYPES");

            if (self->routing_id) {
                char *hex = NULL;
                STR_FROM_BYTES (hex, zframe_data (self->routing_id), zframe_size (self->routing_id));
                zconfig_putf (root, "routing_id", "%s", hex);
                zstr_free (&hex);
            }


            zconfig_t *config = zconfig_new ("content", root);
            zconfig_putf (config, "sequence", "%ld", (long) self->sequence);
            zconfig_putf (config, "client_forename", "%s", self->client_forename);
            zconfig_putf (config, "client_surname", "%s", self->client_surname);
            zconfig_putf (config, "client_mobile", "%s", self->client_mobile);
            zconfig_putf (config, "client_email", "%s", self->client_email);
            zconfig_putf (config, "supplier_forename", "%s", self->supplier_forename);
            zconfig_putf (config, "supplier_surname", "%s", self->supplier_surname);
            zconfig_putf (config, "supplier_mobile", "%s", self->supplier_mobile);
            zconfig_putf (config, "supplier_email", "%s", self->supplier_email);
            break;
            }
    }
    return root;
}

//  --------------------------------------------------------------------------
//  Get/set the message routing_id

zframe_t *
zproto_example_routing_id (zproto_example_t *self)
{
    assert (self);
    return self->routing_id;
}

void
zproto_example_set_routing_id (zproto_example_t *self, zframe_t *routing_id)
{
    if (self->routing_id)
        zframe_destroy (&self->routing_id);
    self->routing_id = zframe_dup (routing_id);
}


//  --------------------------------------------------------------------------
//  Get/set the zproto_example id

int
zproto_example_id (zproto_example_t *self)
{
    assert (self);
    return self->id;
}

void
zproto_example_set_id (zproto_example_t *self, int id)
{
    self->id = id;
}

//  --------------------------------------------------------------------------
//  Return a printable command string

const char *
zproto_example_command (zproto_example_t *self)
{
    assert (self);
    switch (self->id) {
        case ZPROTO_EXAMPLE_LOG:
            return ("LOG");
            break;
        case ZPROTO_EXAMPLE_STRUCTURES:
            return ("STRUCTURES");
            break;
        case ZPROTO_EXAMPLE_BINARY:
            return ("BINARY");
            break;
        case ZPROTO_EXAMPLE_TYPES:
            return ("TYPES");
            break;
    }
    return "?";
}


//  --------------------------------------------------------------------------
//  Get/set the sequence field

uint16_t
zproto_example_sequence (zproto_example_t *self)
{
    assert (self);
    return self->sequence;
}

void
zproto_example_set_sequence (zproto_example_t *self, uint16_t sequence)
{
    assert (self);
    self->sequence = sequence;
}


//  --------------------------------------------------------------------------
//  Get/set the level field

byte
zproto_example_level (zproto_example_t *self)
{
    assert (self);
    return self->level;
}

void
zproto_example_set_level (zproto_example_t *self, byte level)
{
    assert (self);
    self->level = level;
}


//  --------------------------------------------------------------------------
//  Get/set the event field

byte
zproto_example_event (zproto_example_t *self)
{
    assert (self);
    return self->event;
}

void
zproto_example_set_event (zproto_example_t *self, byte event)
{
    assert (self);
    self->event = event;
}


//  --------------------------------------------------------------------------
//  Get/set the node field

uint16_t
zproto_example_node (zproto_example_t *self)
{
    assert (self);
    return self->node;
}

void
zproto_example_set_node (zproto_example_t *self, uint16_t node)
{
    assert (self);
    self->node = node;
}


//  --------------------------------------------------------------------------
//  Get/set the peer field

uint16_t
zproto_example_peer (zproto_example_t *self)
{
    assert (self);
    return self->peer;
}

void
zproto_example_set_peer (zproto_example_t *self, uint16_t peer)
{
    assert (self);
    self->peer = peer;
}


//  --------------------------------------------------------------------------
//  Get/set the time field

uint64_t
zproto_example_time (zproto_example_t *self)
{
    assert (self);
    return self->time;
}

void
zproto_example_set_time (zproto_example_t *self, uint64_t time)
{
    assert (self);
    self->time = time;
}


//  --------------------------------------------------------------------------
//  Get/set the host field

const char *
zproto_example_host (zproto_example_t *self)
{
    assert (self);
    return self->host;
}

void
zproto_example_set_host (zproto_example_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->host)
        return;
    strncpy (self->host, value, 255);
    self->host [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the data field

const char *
zproto_example_data (zproto_example_t *self)
{
    assert (self);
    return self->data;
}

void
zproto_example_set_data (zproto_example_t *self, const char *value)
{
    assert (self);
    assert (value);
    free (self->data);
    self->data = strdup (value);
}


//  --------------------------------------------------------------------------
//  Get the aliases field, without transferring ownership

zlist_t *
zproto_example_aliases (zproto_example_t *self)
{
    assert (self);
    return self->aliases;
}

//  Get the aliases field and transfer ownership to caller

zlist_t *
zproto_example_get_aliases (zproto_example_t *self)
{
    assert (self);
    zlist_t *aliases = self->aliases;
    self->aliases = NULL;
    return aliases;
}

//  Set the aliases field, transferring ownership from caller

void
zproto_example_set_aliases (zproto_example_t *self, zlist_t **aliases_p)
{
    assert (self);
    assert (aliases_p);
    zlist_destroy (&self->aliases);
    self->aliases = *aliases_p;
    *aliases_p = NULL;
}



//  --------------------------------------------------------------------------
//  Get the headers field without transferring ownership

zhash_t *
zproto_example_headers (zproto_example_t *self)
{
    assert (self);
    return self->headers;
}

//  Get the headers field and transfer ownership to caller

zhash_t *
zproto_example_get_headers (zproto_example_t *self)
{
    zhash_t *headers = self->headers;
    self->headers = NULL;
    return headers;
}

//  Set the headers field, transferring ownership from caller

void
zproto_example_set_headers (zproto_example_t *self, zhash_t **headers_p)
{
    assert (self);
    assert (headers_p);
    zhash_destroy (&self->headers);
    self->headers = *headers_p;
    *headers_p = NULL;
}


//  --------------------------------------------------------------------------
//  Get/set the flags field

byte *
zproto_example_flags (zproto_example_t *self)
{
    assert (self);
    return self->flags;
}

void
zproto_example_set_flags (zproto_example_t *self, byte *flags)
{
    assert (self);
    memcpy (self->flags, flags, 4);
}


//  --------------------------------------------------------------------------
//  Get the public_key field without transferring ownership

zchunk_t *
zproto_example_public_key (zproto_example_t *self)
{
    assert (self);
    return self->public_key;
}

//  Get the public_key field and transfer ownership to caller

zchunk_t *
zproto_example_get_public_key (zproto_example_t *self)
{
    zchunk_t *public_key = self->public_key;
    self->public_key = NULL;
    return public_key;
}

//  Set the public_key field, transferring ownership from caller

void
zproto_example_set_public_key (zproto_example_t *self, zchunk_t **chunk_p)
{
    assert (self);
    assert (chunk_p);
    zchunk_destroy (&self->public_key);
    self->public_key = *chunk_p;
    *chunk_p = NULL;
}


//  --------------------------------------------------------------------------
//  Get/set the identifier field
zuuid_t *
zproto_example_identifier (zproto_example_t *self)
{
    assert (self);
    return self->identifier;
}

void
zproto_example_set_identifier (zproto_example_t *self, zuuid_t *uuid)
{
    assert (self);
    zuuid_destroy (&self->identifier);
    self->identifier = zuuid_dup (uuid);
}

//  Get the identifier field and transfer ownership to caller

zuuid_t *
zproto_example_get_identifier (zproto_example_t *self)
{
    zuuid_t *identifier = self->identifier;
    self->identifier = NULL;
    return identifier;
}


//  --------------------------------------------------------------------------
//  Get the address field without transferring ownership

zframe_t *
zproto_example_address (zproto_example_t *self)
{
    assert (self);
    return self->address;
}

//  Get the address field and transfer ownership to caller

zframe_t *
zproto_example_get_address (zproto_example_t *self)
{
    zframe_t *address = self->address;
    self->address = NULL;
    return address;
}

//  Set the address field, transferring ownership from caller

void
zproto_example_set_address (zproto_example_t *self, zframe_t **frame_p)
{
    assert (self);
    assert (frame_p);
    zframe_destroy (&self->address);
    self->address = *frame_p;
    *frame_p = NULL;
}


//  --------------------------------------------------------------------------
//  Get the content field without transferring ownership

zmsg_t *
zproto_example_content (zproto_example_t *self)
{
    assert (self);
    return self->content;
}

//  Get the content field and transfer ownership to caller

zmsg_t *
zproto_example_get_content (zproto_example_t *self)
{
    zmsg_t *content = self->content;
    self->content = NULL;
    return content;
}

//  Set the content field, transferring ownership from caller

void
zproto_example_set_content (zproto_example_t *self, zmsg_t **msg_p)
{
    assert (self);
    assert (msg_p);
    zmsg_destroy (&self->content);
    self->content = *msg_p;
    *msg_p = NULL;
}


//  --------------------------------------------------------------------------
//  Get/set the client_forename field

const char *
zproto_example_client_forename (zproto_example_t *self)
{
    assert (self);
    return self->client_forename;
}

void
zproto_example_set_client_forename (zproto_example_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->client_forename)
        return;
    strncpy (self->client_forename, value, 255);
    self->client_forename [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the client_surname field

const char *
zproto_example_client_surname (zproto_example_t *self)
{
    assert (self);
    return self->client_surname;
}

void
zproto_example_set_client_surname (zproto_example_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->client_surname)
        return;
    strncpy (self->client_surname, value, 255);
    self->client_surname [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the client_mobile field

const char *
zproto_example_client_mobile (zproto_example_t *self)
{
    assert (self);
    return self->client_mobile;
}

void
zproto_example_set_client_mobile (zproto_example_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->client_mobile)
        return;
    strncpy (self->client_mobile, value, 255);
    self->client_mobile [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the client_email field

const char *
zproto_example_client_email (zproto_example_t *self)
{
    assert (self);
    return self->client_email;
}

void
zproto_example_set_client_email (zproto_example_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->client_email)
        return;
    strncpy (self->client_email, value, 255);
    self->client_email [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the supplier_forename field

const char *
zproto_example_supplier_forename (zproto_example_t *self)
{
    assert (self);
    return self->supplier_forename;
}

void
zproto_example_set_supplier_forename (zproto_example_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->supplier_forename)
        return;
    strncpy (self->supplier_forename, value, 255);
    self->supplier_forename [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the supplier_surname field

const char *
zproto_example_supplier_surname (zproto_example_t *self)
{
    assert (self);
    return self->supplier_surname;
}

void
zproto_example_set_supplier_surname (zproto_example_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->supplier_surname)
        return;
    strncpy (self->supplier_surname, value, 255);
    self->supplier_surname [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the supplier_mobile field

const char *
zproto_example_supplier_mobile (zproto_example_t *self)
{
    assert (self);
    return self->supplier_mobile;
}

void
zproto_example_set_supplier_mobile (zproto_example_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->supplier_mobile)
        return;
    strncpy (self->supplier_mobile, value, 255);
    self->supplier_mobile [255] = 0;
}


//  --------------------------------------------------------------------------
//  Get/set the supplier_email field

const char *
zproto_example_supplier_email (zproto_example_t *self)
{
    assert (self);
    return self->supplier_email;
}

void
zproto_example_set_supplier_email (zproto_example_t *self, const char *value)
{
    assert (self);
    assert (value);
    if (value == self->supplier_email)
        return;
    strncpy (self->supplier_email, value, 255);
    self->supplier_email [255] = 0;
}



//  --------------------------------------------------------------------------
//  Selftest

void
zproto_example_test (bool verbose)
{
    printf (" * zproto_example: ");

    if (verbose)
        printf ("\n");

    //  @selftest
    //  Simple create/destroy test
    zconfig_t *config;
    zproto_example_t *self = zproto_example_new ();
    assert (self);
    zproto_example_destroy (&self);
    //  Create pair of sockets we can send through
    //  We must bind before connect if we wish to remain compatible with ZeroMQ < v4
    zsock_t *output = zsock_new (ZMQ_DEALER);
    assert (output);
    int rc = zsock_bind (output, "inproc://selftest-zproto_example");
    assert (rc == 0);

    zsock_t *input = zsock_new (ZMQ_ROUTER);
    assert (input);
    rc = zsock_connect (input, "inproc://selftest-zproto_example");
    assert (rc == 0);


    //  Encode/send/decode and verify each message type
    int instance;
    self = zproto_example_new ();
    zproto_example_set_id (self, ZPROTO_EXAMPLE_LOG);
    zproto_example_set_sequence (self, 123);
    zproto_example_set_level (self, 2);
    zproto_example_set_event (self, 3);
    zproto_example_set_node (self, 45536);
    zproto_example_set_peer (self, 65535);
    zproto_example_set_time (self, 1427261426);
    zproto_example_set_host (self, "localhost");
    zproto_example_set_data (self, "This is the message to log");
    // convert to zpl
    config = zproto_example_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    zproto_example_send (self, output);
    zproto_example_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        zproto_example_t *self_temp = self;
        if (instance < 2)
            zproto_example_recv (self, input);
        else {
            self = zproto_example_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (zproto_example_routing_id (self));
        assert (zproto_example_sequence (self) == 123);
        assert (zproto_example_level (self) == 2);
        assert (zproto_example_event (self) == 3);
        assert (zproto_example_node (self) == 45536);
        assert (zproto_example_peer (self) == 65535);
        assert (zproto_example_time (self) == 1427261426);
        assert (streq (zproto_example_host (self), "localhost"));
        assert (streq (zproto_example_data (self), "This is the message to log"));
        if (instance == 2) {
            zproto_example_destroy (&self);
            self = self_temp;
        }
    }
    zproto_example_set_id (self, ZPROTO_EXAMPLE_STRUCTURES);
    zproto_example_set_sequence (self, 123);
    zlist_t *structures_aliases = zlist_new ();
    zlist_append (structures_aliases, "First alias");
    zlist_append (structures_aliases, "Second alias");
    zlist_append (structures_aliases, "Third alias");
    zproto_example_set_aliases (self, &structures_aliases);
    zhash_t *structures_headers = zhash_new ();
    zhash_insert (structures_headers, "endpoint", "tcp://localhost:5665");
    zproto_example_set_headers (self, &structures_headers);
    // convert to zpl
    config = zproto_example_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    zproto_example_send (self, output);
    zproto_example_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        zproto_example_t *self_temp = self;
        if (instance < 2)
            zproto_example_recv (self, input);
        else {
            self = zproto_example_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (zproto_example_routing_id (self));
        assert (zproto_example_sequence (self) == 123);
        zlist_t *aliases = zproto_example_get_aliases (self);
        assert (aliases);
        assert (streq ((char *) zlist_first (aliases), "First alias"));
        assert (streq ((char *) zlist_next (aliases), "Second alias"));
        assert (streq ((char *) zlist_next (aliases), "Third alias"));
        zlist_destroy (&aliases);
        zlist_destroy (&structures_aliases);
        zhash_t *headers = zproto_example_get_headers (self);
        // Order of values is not guaranted
        char *zhash_headers_value = (char *) zhash_lookup (headers, "endpoint");
        assert (zhash_headers_value != NULL);
        assert (streq (zhash_headers_value, "tcp://localhost:5665"));
        zhash_destroy (&headers);
        if (instance == 2)
            zhash_destroy (&structures_headers);
        if (instance == 2) {
            zproto_example_destroy (&self);
            self = self_temp;
        }
    }
    zproto_example_set_id (self, ZPROTO_EXAMPLE_BINARY);
    zproto_example_set_sequence (self, 123);
    byte flags_data [ZPROTO_EXAMPLE_FLAGS_SIZE] = "b38c";
    zproto_example_set_flags (self, flags_data);
    zchunk_t *binary_public_key = zchunk_new ("89f5ffe70d747869dfe8", 20);
    zproto_example_set_public_key (self, &binary_public_key);
    zuuid_t *binary_identifier = zuuid_new ();
    zuuid_set_str (binary_identifier, "3a60e6850a1e4cc15f3bfd4b42bc6b3e");
    zproto_example_set_identifier (self, binary_identifier);
    zframe_t *binary_address = zframe_new ("0206f99f6137d9fe380f", 20);
    zproto_example_set_address (self, &binary_address);
    zmsg_t *binary_content = zmsg_new ();
    zproto_example_set_content (self, &binary_content);
    zmsg_addstr (zproto_example_content (self), "728a92c6749235ba7002");
    // convert to zpl
    config = zproto_example_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    zproto_example_send (self, output);
    zproto_example_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        zproto_example_t *self_temp = self;
        if (instance < 2)
            zproto_example_recv (self, input);
        else {
            self = zproto_example_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (zproto_example_routing_id (self));
        assert (zproto_example_sequence (self) == 123);
        assert (memcmp (zproto_example_flags (self), "b38c", ZPROTO_EXAMPLE_FLAGS_SIZE) == 0);
        assert (memcmp (zchunk_data (zproto_example_public_key (self)), "89f5ffe70d747869dfe8", 20) == 0);
        if (instance == 2)
            zchunk_destroy (&binary_public_key);
        assert (zuuid_eq (binary_identifier, zuuid_data (zproto_example_identifier (self))));
        if (instance == 2)
            zuuid_destroy (&binary_identifier);
        assert (zframe_streq (zproto_example_address (self), "0206f99f6137d9fe380f"));
        if (instance == 2)
            zframe_destroy (&binary_address);
        assert (zmsg_size (zproto_example_content (self)) == 1);
        char *content = zmsg_popstr (zproto_example_content (self));
        assert (streq (content, "728a92c6749235ba7002"));
        zstr_free (&content);
        if (instance == 2)
            zmsg_destroy (&binary_content);
        if (instance == 2) {
            zproto_example_destroy (&self);
            self = self_temp;
        }
    }
    zproto_example_set_id (self, ZPROTO_EXAMPLE_TYPES);
    zproto_example_set_sequence (self, 123);
    zproto_example_set_client_forename (self, "Lucius Junius");
    zproto_example_set_client_surname (self, "Brutus");
    zproto_example_set_client_mobile (self, "01234567890");
    zproto_example_set_client_email (self, "brutus@example.com");
    zproto_example_set_supplier_forename (self, "Leslie");
    zproto_example_set_supplier_surname (self, "Lamport");
    zproto_example_set_supplier_mobile (self, "01987654321");
    zproto_example_set_supplier_email (self, "lamport@example.com");
    // convert to zpl
    config = zproto_example_zpl (self, NULL);
    if (verbose)
        zconfig_print (config);
    //  Send twice
    zproto_example_send (self, output);
    zproto_example_send (self, output);

    for (instance = 0; instance < 3; instance++) {
        zproto_example_t *self_temp = self;
        if (instance < 2)
            zproto_example_recv (self, input);
        else {
            self = zproto_example_new_zpl (config);
            assert (self);
            zconfig_destroy (&config);
        }
        if (instance < 2)
            assert (zproto_example_routing_id (self));
        assert (zproto_example_sequence (self) == 123);
        assert (streq (zproto_example_client_forename (self), "Lucius Junius"));
        assert (streq (zproto_example_client_surname (self), "Brutus"));
        assert (streq (zproto_example_client_mobile (self), "01234567890"));
        assert (streq (zproto_example_client_email (self), "brutus@example.com"));
        assert (streq (zproto_example_supplier_forename (self), "Leslie"));
        assert (streq (zproto_example_supplier_surname (self), "Lamport"));
        assert (streq (zproto_example_supplier_mobile (self), "01987654321"));
        assert (streq (zproto_example_supplier_email (self), "lamport@example.com"));
        if (instance == 2) {
            zproto_example_destroy (&self);
            self = self_temp;
        }
    }

    zproto_example_destroy (&self);
    zsock_destroy (&input);
    zsock_destroy (&output);
#if defined (__WINDOWS__)
    zsys_shutdown();
#endif
    //  @end

    printf ("OK\n");
}
