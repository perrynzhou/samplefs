/*************************************************************************
    > File Name: chunk_cache.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sat 09 Apr 2022 05:32:32 AM UTC
 ************************************************************************/

#include "chunk_cache.h"


inline static uint32_t hashmap_hash_cb(void *k, size_t sz)
{
  return c_hash_fnv1a_64((const char *)k,sz);

}

inline static int hashmap_key_compare_cb(void *k1,void *k2,size_t len)
{
   int key1 = *(int *)k1;
   int key2=*(int *)k2;
   return key1==key2;
}
chunk_cache_t *chunk_cache_new(uint32_t  max_count)
{
  chunk_cache_t *blk_cache = (chunk_cache_t *)calloc(1,sizeof(chunk_cache_t));
  // c_hashmap_t *c_hashmap_alloc(size_t max_count,c_hashmap_hash_cb hash_cb,c_hashmap_key_compare_cb cmp_cb);

  blk_cache->cache = c_hashmap_alloc(max_count,hashmap_hash_cb,hashmap_key_compare_cb);
  blk_cache->chunks = (chunk_t **)calloc(max_count,sizeof(void *));
  for(size_t i=0;i<max_count;i++) 
  {
    char buf[64] = {'\0'};
    snprintf((char *)&buf,64,"chunk_%ld",i);
    size_t buf_len = strlen((char *)&buf);
    chunk_t *blk = (chunk_t *)calloc(1,sizeof(chunk_t));
    chunk_init(blk,i,NULL);
    c_hashmap_put(blk_cache->cache, (char *)&buf,buf_len, blk,sizeof(void *));
    blk_cache->chunks[i]=blk;
  }
  return blk_cache;
}
void chunk_cache_destroy(uint32_t  max_count)
{

}