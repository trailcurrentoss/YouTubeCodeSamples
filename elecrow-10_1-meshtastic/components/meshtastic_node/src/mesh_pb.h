#pragma once

/*
 * Minimal protobuf wire-format writer/reader.
 *
 * Only what the Meshtastic phone API and radio payloads need: varints,
 * fixed32, float, length-delimited bytes/strings and nested messages.
 * Field numbers live in mesh_proto.h.
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* --- Writer --- */

typedef struct {
    uint8_t *buf;
    size_t   cap;
    size_t   len;
    bool     overflow;
} pbw_t;

void pbw_init(pbw_t *w, uint8_t *buf, size_t cap);

void pbw_varint(pbw_t *w, uint32_t field, uint64_t v);
void pbw_int32(pbw_t *w, uint32_t field, int32_t v);   /* negative -> 10-byte varint */
void pbw_bool(pbw_t *w, uint32_t field, bool v);
void pbw_fixed32(pbw_t *w, uint32_t field, uint32_t v);
void pbw_float(pbw_t *w, uint32_t field, float v);
void pbw_bytes(pbw_t *w, uint32_t field, const void *data, size_t len);
void pbw_string(pbw_t *w, uint32_t field, const char *s);

/* Nested message: begin returns a mark, end patches the length prefix. */
size_t pbw_sub_begin(pbw_t *w, uint32_t field);
void   pbw_sub_end(pbw_t *w, size_t mark);

/* --- Reader --- */

typedef struct {
    const uint8_t *p;
    const uint8_t *end;
} pbr_t;

typedef struct {
    uint32_t       field;
    uint8_t        wire;   /* 0 varint, 1 fixed64, 2 len-delimited, 5 fixed32 */
    uint64_t       u;      /* varint / fixed value */
    const uint8_t *ptr;    /* wire 2 data */
    size_t         len;    /* wire 2 length */
} pbr_field_t;

void pbr_init(pbr_t *r, const uint8_t *buf, size_t len);

/* Returns false at end of buffer or on malformed input. */
bool pbr_next(pbr_t *r, pbr_field_t *f);

static inline float pbr_float(const pbr_field_t *f)
{
    union { uint32_t u; float f; } c = { .u = (uint32_t)f->u };
    return c.f;
}

/* Copy a wire-2 field into a NUL-terminated string buffer. */
void pbr_strcpy(const pbr_field_t *f, char *dst, size_t dst_size);
