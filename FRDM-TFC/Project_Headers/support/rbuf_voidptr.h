/**
 * @file rbuf_voidptr.h
 *
 * @brief Ring buffers
 *
 * Ringbuffer implementation store/access acditrary binary data
 *
 * @todo allow to register i/o filters to be executed at read/write time
 *
 */

#ifndef __CQ_H__
#define __CQ_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* OVERWRITE MODE NOT IMPLEMENTED
 * 
typedef enum {
    RBUF_MODE_BLOCKING = 0,
    RBUF_MODE_OVERWRITE
} rbuf_voidptr_mode_t;
*/

typedef struct {
    void* *buf;              // queue elements
    uint32_t size;           // queue size
    uint32_t available;      // queue size
    uint32_t used;           // used size
    uint32_t rfx;            // read offset
    uint32_t wfx;            // write offset
  //uint32_t mode;           // the queue mode (blocking/overwrite)
} rbuf_voidptr_s;

void      rbuf_voidptr_init(rbuf_voidptr_s *cq, void* *buf, uint32_t size);
//void      rbuf_voidptr_set_mode(rbuf_voidptr_s *cqueue, rbuf_voidptr_mode_t mode);
//rbuf_voidptr_mode_t rbuf_voidptr_mode(rbuf_voidptr_s *cqueue);
void      rbuf_voidptr_skip(rbuf_voidptr_s *cq, uint32_t size);
uint32_t  rbuf_voidptr_pop(rbuf_voidptr_s *cq, void* *out, uint32_t size);
uint32_t  rbuf_voidptr_push(rbuf_voidptr_s *cq, void* *in, uint32_t size);
uint32_t  rbuf_voidptr_size(rbuf_voidptr_s *cq);
uint32_t  rbuf_voidptr_used(rbuf_voidptr_s *cq);
uint32_t  rbuf_voidptr_available(rbuf_voidptr_s *cq);
void      rbuf_voidptr_clear(rbuf_voidptr_s *cq);

#ifdef __cplusplus
}
#endif

#endif

// vim: tabstop=4 shiftwidth=4 expandtab:
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
