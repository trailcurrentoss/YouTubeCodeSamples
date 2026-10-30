#include "mesh_pb.h"
#include <string.h>

/* --- Writer --- */

void pbw_init(pbw_t *w, uint8_t *buf, size_t cap)
{
    w->buf = buf;
    w->cap = cap;
    w->len = 0;
    w->overflow = false;
}

static void put(pbw_t *w, const void *data, size_t n)
{
    if (w->overflow || w->len + n > w->cap) {
        w->overflow = true;
        return;
    }
    memcpy(w->buf + w->len, data, n);
    w->len += n;
}

static size_t varint_encode(uint8_t *out, uint64_t v)
{
    size_t n = 0;
    do {
        uint8_t b = v & 0x7F;
        v >>= 7;
        out[n++] = b | (v ? 0x80 : 0);
    } while (v);
    return n;
}

static void put_varint(pbw_t *w, uint64_t v)
{
    uint8_t tmp[10];
    put(w, tmp, varint_encode(tmp, v));
}

static void put_key(pbw_t *w, uint32_t field, uint8_t wire)
{
    put_varint(w, ((uint64_t)field << 3) | wire);
}

void pbw_varint(pbw_t *w, uint32_t field, uint64_t v)
{
    put_key(w, field, 0);
    put_varint(w, v);
}

void pbw_int32(pbw_t *w, uint32_t field, int32_t v)
{
    pbw_varint(w, field, (uint64_t)(int64_t)v);
}

void pbw_bool(pbw_t *w, uint32_t field, bool v)
{
    pbw_varint(w, field, v ? 1 : 0);
}

void pbw_fixed32(pbw_t *w, uint32_t field, uint32_t v)
{
    uint8_t le[4] = { v, v >> 8, v >> 16, v >> 24 };
    put_key(w, field, 5);
    put(w, le, 4);
}

void pbw_float(pbw_t *w, uint32_t field, float v)
{
    union { float f; uint32_t u; } c = { .f = v };
    pbw_fixed32(w, field, c.u);
}

void pbw_bytes(pbw_t *w, uint32_t field, const void *data, size_t len)
{
    put_key(w, field, 2);
    put_varint(w, len);
    put(w, data, len);
}

void pbw_string(pbw_t *w, uint32_t field, const char *s)
{
    pbw_bytes(w, field, s, strlen(s));
}

size_t pbw_sub_begin(pbw_t *w, uint32_t field)
{
    put_key(w, field, 2);
    uint8_t zero = 0;
    put(w, &zero, 1);       /* 1-byte length placeholder, widened in end() */
    return w->len;
}

void pbw_sub_end(pbw_t *w, size_t mark)
{
    if (w->overflow) return;
    size_t body = w->len - mark;
    uint8_t tmp[10];
    size_t n = varint_encode(tmp, body);
    if (n > 1) {
        if (w->len + n - 1 > w->cap) {
            w->overflow = true;
            return;
        }
        memmove(w->buf + mark + n - 1, w->buf + mark, body);
        w->len += n - 1;
    }
    memcpy(w->buf + mark - 1, tmp, n);
}

/* --- Reader --- */

void pbr_init(pbr_t *r, const uint8_t *buf, size_t len)
{
    r->p = buf;
    r->end = buf + len;
}

static bool get_varint(pbr_t *r, uint64_t *out)
{
    uint64_t v = 0;
    for (int shift = 0; shift < 64; shift += 7) {
        if (r->p >= r->end) return false;
        uint8_t b = *r->p++;
        v |= (uint64_t)(b & 0x7F) << shift;
        if (!(b & 0x80)) {
            *out = v;
            return true;
        }
    }
    return false;
}

bool pbr_next(pbr_t *r, pbr_field_t *f)
{
    uint64_t key;
    if (r->p >= r->end || !get_varint(r, &key)) return false;
    f->field = key >> 3;
    f->wire = key & 7;
    f->ptr = NULL;
    f->len = 0;
    f->u = 0;

    switch (f->wire) {
    case 0:
        return get_varint(r, &f->u);
    case 1:
        if (r->end - r->p < 8) return false;
        for (int i = 7; i >= 0; i--) f->u = (f->u << 8) | r->p[i];
        r->p += 8;
        return true;
    case 2: {
        uint64_t len;
        if (!get_varint(r, &len) || len > (uint64_t)(r->end - r->p)) return false;
        f->ptr = r->p;
        f->len = len;
        r->p += len;
        return true;
    }
    case 5:
        if (r->end - r->p < 4) return false;
        f->u = r->p[0] | r->p[1] << 8 | r->p[2] << 16 | (uint32_t)r->p[3] << 24;
        r->p += 4;
        return true;
    default:
        return false;
    }
}

void pbr_strcpy(const pbr_field_t *f, char *dst, size_t dst_size)
{
    size_t n = f->len < dst_size - 1 ? f->len : dst_size - 1;
    memcpy(dst, f->ptr, n);
    dst[n] = 0;
}
