/*************************************************************************
    > File Name: chunk_cache.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 09 Apr 2022 05:32:25 AM UTC
 ************************************************************************/

#ifndef _CHUNK_CACHE_H
#define _CHUNK_CACHE_H
#include "chunk.h"
#include "c_hashmap.h"
#include "c_hash.h"
typedef struct
{
  c_hashmap_t *cache;
  chunk_t **chunks;
} chunk_cache_t;

chunk_cache_t *chunk_cache_new(uint32_t max_count);
int  chunk_cache_remove(chunk_cache_t *cc,void *key,size_t key_sz);
int  chunk_cache_add(chunk_cache_t *cc,void *key,size_t key_sz,void *value,size_t val_sz);
void chunk_cache_destroy(chunk_cache_t *cc);
#endif
