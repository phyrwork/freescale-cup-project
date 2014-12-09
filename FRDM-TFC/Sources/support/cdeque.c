#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "support/cdeque.h"

#define RBUF_DEFAULT_SIZE 4096

void
cdeque_init(cdeque_s *cdeque, void* *buf, uint32_t size) {
    cdeque->size = size;
    cdeque->buf = buf;
    cdeque->available = cdeque->size - 1;
    cdeque->used = 0;
    cdeque->rfx = 0;
    cdeque->wfx = 0;
}

static void
cdeque_update_size(cdeque_s *cd) {
    if(cd->wfx == cd->rfx)
        cd->used = 0;
    else if(cd->wfx < cd->rfx)
        cd->used = cd->wfx+(cd->size-cd->rfx);
    else
        cd->used = cd->wfx-cd->rfx;

    cd->available = cd->size - cd->used - 1;
}

/* OVERWITE MODE NOT IMPLEMENTED
 *
void
cdeque_set_mode(cdeque_s *cdeque, cdeque_mode_t mode)
{
    cdeque->mode = mode;
}

cdeque_mode_t
cdeque_mode(cdeque_s *cdeque)
{
    return cdeque->mode;
}
*/

void
cdeque_skip(cdeque_s *cd, uint32_t size) {
    if(size >= cd->size) { // just empty the ringbuffer
        cd->rfx = cd->wfx;
    } else {
        if (size > cd->size-cd->rfx) {
            size -= cd->size-cd->rfx;
            cd->rfx = size;
        } else {
            cd->rfx+=size;
        }
    }
    cdeque_update_size(cd);
}

uint32_t
cdeque_peek_front(cdeque_s *cd, void* *out, uint32_t size) {
	uint32_t read_size = cd->used; // never read more than available data
    uint32_t to_end = cd->size - cd->rfx;

    // requested size is less than stored data, return only what has been requested
    if(read_size > size)
        read_size = size;

    if(read_size > 0) {
        // if the write pointer is beyond the read pointer or the requested read_size is
        // smaller than the number of elements between the read pointer and the end of the buffer,
        // than we can safely copy all the elements in a single shot
        if(cd->wfx > cd->rfx || to_end >= read_size) {
            memcpy(out, &cd->buf[cd->rfx], read_size);
        }
        else { // otherwise we have to wrap around the buffer and copy octest in two times
            memcpy(out, &cd->buf[cd->rfx], to_end);
            memcpy(out+to_end, &cd->buf[0], read_size - to_end);
        }
    }

    cdeque_update_size(cd);

    return read_size;
}

uint32_t
cdeque_peek_back(cdeque_s *cd, void* *out, uint32_t size) {
    uint32_t read_size = cd->used; // never read more than available data
    uint32_t to_end = cd->wfx;

    // requested size is less than stored data, return only what has been requested
    if(read_size > size)
        read_size = size;

    if(read_size > 0) {
        // if the write pointer is beyond the read pointer or the requested read_size is
        // smaller than the number of elements between the read pointer and the end of the buffer,
        // than we can safely copy all the elements in a single shot
        if(cd->wfx > cd->rfx || to_end >= read_size) {
        	uint32_t lwfx = cd->wfx;
            for (uint32_t r = 0; r < read_size; ++r)
            	out[r] = cd->buf[--lwfx];
        }
        else { // otherwise we have to wrap around the buffer and copy octest in two times
        	uint32_t lwfx = cd->wfx;
           	for (uint32_t r = 0; r < to_end; ++r)
           		out[r] = cd->buf[--lwfx];
           	lwfx = cd->size;
            for (uint32_t r = to_end; r < read_size - to_end; ++r)
            	out[r] = cd->buf[--lwfx];
        }
    }

    cdeque_update_size(cd);

    return read_size;
}

uint32_t
cdeque_pop_front(cdeque_s *cd, void* *out, uint32_t size) {
    uint32_t read_size = cd->used; // never read more than available data
    uint32_t to_end = cd->size - cd->rfx;

    // requested size is less than stored data, return only what has been requested
    if(read_size > size)
        read_size = size;

    if(read_size > 0) {
        // if the write pointer is beyond the read pointer or the requested read_size is
        // smaller than the number of elements between the read pointer and the end of the buffer,
        // than we can safely copy all the elements in a single shot
        if(cd->wfx > cd->rfx || to_end >= read_size) {
            memcpy(out, &cd->buf[cd->rfx], read_size);
            cd->rfx += read_size;
        }
        else { // otherwise we have to wrap around the buffer and copy octest in two times
            memcpy(out, &cd->buf[cd->rfx], to_end);
            memcpy(out+to_end, &cd->buf[0], read_size - to_end);
            cd->rfx = read_size - to_end;
        }
    }

    cdeque_update_size(cd);

    return read_size;
}

uint32_t
cdeque_pop_back(cdeque_s *cd, void* *out, uint32_t size) {
    uint32_t read_size = cd->used; // never read more than available data
    uint32_t to_end = cd->wfx;

    // requested size is less than stored data, return only what has been requested
    if(read_size > size)
        read_size = size;

    if(read_size > 0) {
        // if the write pointer is beyond the read pointer or the requested read_size is
        // smaller than the number of elements between the read pointer and the end of the buffer,
        // than we can safely copy all the elements in a single shot
        if(cd->wfx > cd->rfx || to_end >= read_size) {
            for (uint32_t r = 0; r < read_size; ++r)
            	out[r] = cd->buf[--cd->wfx];
        }
        else { // otherwise we have to wrap around the buffer and copy octest in two times
           	for (uint32_t r = 0; r < to_end; ++r)
           		out[r] = cd->buf[--cd->wfx];
           	cd->wfx = cd->size;
            for (uint32_t r = to_end; r < read_size - to_end; ++r)
            	out[r] = cd->buf[--cd->wfx];
        }
    }

    cdeque_update_size(cd);

    return read_size;
}

uint32_t
cdeque_push_back(cdeque_s *cd, void* *in, uint32_t size) {
    uint32_t write_size = cd->available; // don't write more than available size

    if(!cd || !in || !size) // safety belt
        return 0;

    // if requested size fits the available space, use that
    if(write_size > size) {
        write_size = size;
    }
    /* OVERWRITE MODE NOT IMPLEMENTED
     *
    else if (cd->mode == RBUF_MODE_OVERWRITE) {
        if (size > cd->size - 1) {
            // the provided buffer is bigger than the
            // ringbuffer itself. Since we are in overwrite mode,
            // only the last chunk will be actually stored.
            write_size = cd->size - 1;
            in = in + (size - write_size);
            cd->rfx = 0;
            memcpy(cd->buf, in, write_size);
            cd->wfx = write_size;
            cdeque_update_size(cd);
            return size;
        }
        // we are in overwrite mode, so let's make some space
        // for the new data by advancing the read offset
        uint32_t diff = size - write_size;
        cd->rfx += diff;
        write_size += diff;
        if (cd->rfx >= cd->size)
            cd->rfx -= cd->size;
    }
    */

    if(cd->wfx >= cd->rfx) { // write pointer is ahead
        if(write_size <= cd->size - cd->wfx) {
            memcpy(&cd->buf[cd->wfx], in, write_size);
            cd->wfx+=write_size;
        } else { // and we have to wrap around the buffer
            uint32_t to_end = cd->size - cd->wfx;
            memcpy(&cd->buf[cd->wfx], in, to_end);
            memcpy(cd->buf, in+to_end, write_size - to_end);
            cd->wfx = write_size - to_end;
        }
    } else { // read pointer is ahead we can safely memcpy the entire chunk
        memcpy(&cd->buf[cd->wfx], in, write_size);
        cd->wfx+=write_size;
    }

    cdeque_update_size(cd);

    return write_size;
}

uint32_t
cdeque_push_front(cdeque_s *cd, void* *in, uint32_t size) {
    uint32_t write_size = cd->available; // don't write more than available size

    if(!cd || !in || !size) // safety belt
        return 0;

    // if requested size fits the available space, use that
    if(write_size > size) {
        write_size = size;
    }
    /* OVERWRITE MODE NOT IMPLEMENTED
     *
    else if (cd->mode == RBUF_MODE_OVERWRITE) {
        if (size > cd->size - 1) {
            // the provided buffer is bigger than the
            // ringbuffer itself. Since we are in overwrite mode,
            // only the last chunk will be actually stored.
            write_size = cd->size - 1;
            in = in + (size - write_size);
            cd->rfx = 0;
            memcpy(cd->buf, in, write_size);
            cd->wfx = write_size;
            cdeque_update_size(cd);
            return size;
        }
        // we are in overwrite mode, so let's make some space
        // for the new data by advancing the read offset
        uint32_t diff = size - write_size;
        cd->rfx += diff;
        write_size += diff;
        if (cd->rfx >= cd->size)
            cd->rfx -= cd->size;
    }
    */ 

    if(cd->wfx >= cd->rfx) { // write pointer is ahead
        if(write_size <= cd->rfx) {
            for (uint32_t w = 0; w < write_size; ++w)
	        	cd->buf[--cd->rfx] = in[w];
        } else { // and we have to wrap around the buffer
            uint32_t to_end = cd->rfx;
            //memcpy(&cd->buf[cd->wfx], in, to_end);
            for (uint32_t w = 0; w < to_end; ++w)
	        	cd->buf[--cd->rfx] = in[w];
            cd->rfx = cd->size;
            for (uint32_t w = to_end; w < write_size - to_end; ++w)
	        	cd->buf[--cd->rfx] = in[w];
        }
    } else { // write pointer is behind we can safely copy the entire chunk
        for (uint32_t w = 0; w < write_size; ++w)
        	cd->buf[--cd->rfx] = in[w];
    }

    cdeque_update_size(cd);

    return write_size;
}

uint32_t
cdeque_used(cdeque_s *cd) {
    return cd->used;
}

uint32_t
cdeque_size(cdeque_s *cd) {
    return cd->size - 1;
}

uint32_t
cdeque_available(cdeque_s *cd) {
    return cd->available;
}

void
cdeque_clear(cdeque_s *cd) {
    cd->rfx = cd->wfx = 0;
    cdeque_update_size(cd);
}

// vim: tabstop=4 shiftwidth=4 expandtab:
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
