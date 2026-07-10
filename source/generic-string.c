#include <stdbool.h>

#include "../include/generic-string.h"

/* Validate a UTF-8 byte sequence and count its code points.
   Returns true if well-formed, and writes the count to *out_count. */
bool utf8_validate_and_count(u8 *bytes, u32 length, u32 *out_count) {
    u32 count = 0;
    u32 i = 0;

    while (i < length) {
        u8 b = bytes[i];
        u32 seq_len;
        u32 min_val, max_val;

        if (b < 0x80) {
            seq_len = 1;
            min_val = 0x00;
            max_val = 0x7F;
        }
        else if ((b & 0xE0) == 0xC0) {
            seq_len = 2;
            min_val = 0x80;
            max_val = 0x7FF;
        }
        else if ((b & 0xF0) == 0xE0) {
            seq_len = 3;
            min_val = 0x800;
            max_val = 0xFFFF;
        }
        else if ((b & 0xF8) == 0xF0) {
            seq_len = 4;
            min_val = 0x10000;
            max_val = 0x10FFFF;
        }
        else {
            return false;  /* invalid leading byte */
        }

        if (i + seq_len > length) {
            return false;  /* truncated sequence */
        }

        u32 codepoint = b & (0xFF >> seq_len);
        for (u32 j = 1; j < seq_len; j++) {
            u8 c = bytes[i + j];
            if ((c & 0xC0) != 0x80) {
                return false;
            }
            codepoint = (codepoint << 6) | (c & 0x3F);
        }

        /* Reject overlong forms and out-of-range values */
        if (codepoint < min_val || codepoint > max_val) {
            return false;
        }

        /* Reject UTF-16 surrogate halves */
        if (codepoint >= 0xD800 && codepoint <= 0xDFFF) {
            return false;
        }

        ++count;
        i += seq_len;
    }

    *out_count = count;
    return true;
}

