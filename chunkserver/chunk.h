/*************************************************************************
    > File Name: chunk.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Fri 08 Apr 2022 11:27:54 PM UTC
 ************************************************************************/

#ifndef _CHUNK_H
#define _CHUNK_H
#include "utils.h"
typedef struct
{
  uint32_t chunk_id;
  uint32_t offset;
  int fd;
  pthread_t thd;
  uint32_t total;
  void *ctx;
} chunk_t;
chunk_t * chunk_alloc(int id, void *ctx);
int chunk_init(chunk_t *blk,int id, void *ctx);
ssize_t chunk_write(chunk_t *blk, void *buf, size_t pos, size_t len);
ssize_t chunk_read(chunk_t *blk, void *buf, size_t pos, size_t len);
void chunk_deinit(chunk_t *blk);
void chunk_destroy(chunk_t *blk);
#endif
