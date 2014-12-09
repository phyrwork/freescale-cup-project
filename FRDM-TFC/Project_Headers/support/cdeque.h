/**
 * @file cdeque.h
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
} cdeque_mode_t;
*/

typedef struct {
    void* *buf;              // queue elements
    uint32_t size;           // queue size
    uint32_t available;      // queue size
    uint32_t used;           // used size
    uint32_t rfx;            // read offset
    uint32_t wfx;            // write offset
  //uint32_t mode;           // the queue mode (blocking/overwrite)
} cdeque_s;

void      cdeque_init(cdeque_s *cd, void* *buf, uint32_t size);
//void      cdeque_set_mode(cdeque_s *cdeque, cdeque_mode_t mode);
//cdeque_mode_t cdeque_mode(cdeque_s *cdeque);
void      cdeque_skip(cdeque_s *cd, uint32_t size);
uint32_t  cdeque_peek_front(cdeque_s *cd, void* *out, uint32_t size);
uint32_t  cdeque_peek_back(cdeque_s *cd, void* *out, uint32_t size);
uint32_t  cdeque_pop_front(cdeque_s *cd, void* *out, uint32_t size);
uint32_t  cdeque_pop_back(cdeque_s *cd, void* *out, uint32_t size);
uint32_t  cdeque_push_back(cdeque_s *cd, void* *in, uint32_t size);
uint32_t  cdeque_push_front(cdeque_s *cd, void* *in, uint32_t size);
uint32_t  cdeque_size(cdeque_s *cd);
uint32_t  cdeque_used(cdeque_s *cd);
uint32_t  cdeque_available(cdeque_s *cd);
void      cdeque_clear(cdeque_s *cd);

#ifdef __cplusplus
}
#endif

#endif

// vim: tabstop=4 shiftwidth=4 expandtab:
/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
