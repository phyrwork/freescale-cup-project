#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support/cqueue.h"

void
cqueue_init(cqueue_s *cqueue, void* *buf, uint32_t size) {
    cqueue->size = size;
    cqueue->buf = buf;
    cqueue->available = cqueue->size - 1;
    cqueue->used = 0;
    cqueue->rfx = 0;
    cqueue->wfx = 0;
}

static void
cqueue_update_size(cqueue_s *cq) {
    if(cq->wfx == cq->rfx)
        cq->used = 0;
    else if(cq->wfx < cq->rfx)
        cq->used = cq->wfx+(cq->size-cq->rfx);
    else
        cq->used = cq->wfx-cq->rfx;

    cq->available = cq->size - cq->used - 1;
}

/* OVERWITE MODE NOT IMPLEMENTED
 *
void
cqueue_set_mode(cqueue_s *cqueue, cqueue_mode_t mode)
{
    cqueue->mode = mode;
}

cqueue_mode_t
cqueue_mode(cqueue_s *cqueue)
{
    return cqueue->mode;
}
*/

void
cqueue_skip(cqueue_s *cq, uint32_t size) {
    if(size >= cq->size) { // just empty the ringbuffer
        cq->rfx = cq->wfx;
    } else {
        if (size > cq->size-cq->rfx) {
            size -= cq->size-cq->rfx;
            cq->rfx = size;
        } else {
            cq->rfx+=size;
        }
    }
    cqueue_update_size(cq);
}

uint32_t
cqueue_pop(cqueue_s *cq, void* *out, uint32_t size) {
    uint32_t read_size = cq->used; // never read more than available data
    uint32_t to_end = cq->size - cq->rfx;

    // requested size is less than stored data, return only what has been requested
    if(read_size > size)
        read_size = size;

    if(read_size > 0) {
        // if the write pointer is beyond the read pointer or the requested read_size is
        // smaller than the number of elements between the read pointer and the end of the buffer,
        // than we can safely copy all the elements in a single shot
        if(cq->wfx > cq->rfx || to_end >= read_size) {
            memcpy(out, &cq->buf[cq->rfx], read_size * sizeof(void*));
            cq->rfx += read_size;
        }
        else { // otherwise we have to wrap around the buffer and copy octest in two times
            memcpy(out, &cq->buf[cq->rfx], to_end * sizeof(void*));
            memcpy(out+to_end, &cq->buf[0], (read_size - to_end) * sizeof(void*));
            cq->rfx = read_size - to_end;
        }
    }

    cqueue_update_size(cq);

    return read_size;
}

uint32_t
cqueue_push(cqueue_s *cq, void* *in, uint32_t size) {
    uint32_t write_size = cq->available; // don't write more than available size

    if(!cq || !in || !size) // safety belt
        return 0;

    // if requested size fits the available space, use that
    if(write_size > size) {
        write_size = size;
    }
    /* OVERWRITE MODE NOT IMPLEMENTED
     *
    else if (cq->mode == RBUF_MODE_OVERWRITE) {
        if (size > cq->size - 1) {
            // the provided buffer is bigger than the
            // ringbuffer itself. Since we are in overwrite mode,
            // only the last chunk will be actually stored.
            write_size = cq->size - 1;
            in = in + (size - write_size);
            cq->rfx = 0;
            memcpy(cq->buf, in, write_size);
            cq->wfx = write_size;
            cqueue_update_size(cq);
            return size;
        }
        // we are in overwrite mode, so let's make some space
        // for the new data by advancing the read offset
        uint32_t diff = size - write_size;
        cq->rfx += diff;
        write_size += diff;
        if (cq->rfx >= cq->size)
            cq->rfx -= cq->size;
    }
    */

    if(cq->wfx >= cq->rfx) { // write pointer is ahead
        if(write_size <= cq->size - cq->wfx) {
            memcpy(&cq->buf[cq->wfx], in, write_size * sizeof(void*));
            cq->wfx+=write_size;
        } else { // and we have to wrap around the buffer
            uint32_t to_end = cq->size - cq->wfx;
            memcpy(&cq->buf[cq->wfx], in, to_end * sizeof(void*));
            memcpy(cq->buf, in+to_end, (write_size - to_end) * sizeof(void*));
            cq->wfx = write_size - to_end;
        }
    } else { // read pointer is ahead we can safely memcpy the entire chunk
        memcpy(&cq->buf[cq->wfx], in, write_size);
        cq->wfx+=write_size;
    }

    cqueue_update_size(cq);

    return write_size;
}

uint32_t
cqueue_used(cqueue_s *cq) {
    return cq->used;
}

uint32_t
cqueue_size(cqueue_s *cq) {
    return cq->size - 1;
}

uint32_t
cqueue_available(cqueue_s *cq) {
    return cq->available;
}

void
cqueue_clear(cqueue_s *cq) {
    cq->rfx = cq->wfx = 0;
    cqueue_update_size(cq);
}

// vim: tabstop=4 shiftwidth=4 expandtab:
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
