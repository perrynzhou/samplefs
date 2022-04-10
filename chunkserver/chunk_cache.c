/*************************************************************************
    > File Name: chunk_cache.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 09 Apr 2022 05:32:32 AM UTC
 ************************************************************************/

#include "chunk_cache.h"

inline static uint32_t hashmap_hash_cb(void *k, size_t sz)
{
  return c_hash_fnv1a_64((const char *)k, sz);
}

void hashmap_free_cb(void *arg)
{
  if (arg != NULL)
  {
    chunk_t *ck = (chunk_t *)arg;
    chunk_deinit(ck);
    free(ck);
    ck = NULL;
  }
}
inline static int hashmap_key_compare_cb(void *k1, void *k2, size_t len)
{
  int key1 = *(int *)k1;
  int key2 = *(int *)k2;
  return key1 == key2;
}
chunk_cache_t *chunk_cache_load(char *root)
{
  DIR *dir;
  struct dirent *dp;
  char *file_name;
  dir = opendir(root);
  char buf[256] = {'\0'};

  size_t root_sz = strlen(root);

  char *buf_ptr = (char *)&buf;
  if (root[root_sz - 1] == '/')
  {
    snprintf(buf_ptr, 256, "%s", root);
  }
  else
  {
    snprintf(buf_ptr, 256, "%s/", root);
    root_sz++;
  }
  chunk_t **vector = calloc(65535, sizeof(chunk_t *));
  size_t i = 0;
  while ((dp = readdir(dir)) != NULL)
  {
    if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
    {
      continue;
    }
    struct stat st;
    snrpintf(buf_ptr + root_sz, 256 - root_sz, "%s", dp->d_name);
    if (lstat(buf_ptr, &st) == -1)
    {
      continue;
    }
    if (S_ISREG(st.st_mode) && is_digit(dp->d_name, strlen(dp->d_name)))
    {

      int chunk_id = atoi(dp->d_name);
      chunk_t *ck = chunk_alloc(chunk_id, NULL);
      ck->total = st.st_size;

      vector[i++] = ck;
    }
  }
  closedir(dir);
  chunk_cache_t *cc = chunk_cache_new(i + 1);
  for (size_t j = 0; j < i; i++)
  {
    cc->chunks[j] = vector[j];
    char buf[64] = {'\0'};
    snprintf((char *)&buf, 64, "%s", cc->chunks[j]->chunk_id);
    c_hashmap_put(cc->cache, (char *)&buf, strlen((char *)&buf), cc->chunks[j], sizeof(void *));
  }
  return cc;
}
chunk_cache_t *chunk_cache_new(uint32_t max_count)
{
  chunk_cache_t *blk_cache = (chunk_cache_t *)calloc(1, sizeof(chunk_cache_t));
  // c_hashmap_t *c_hashmap_alloc(size_t max_count,c_hashmap_hash_cb hash_cb,c_hashmap_key_compare_cb cmp_cb);

  blk_cache->cache = c_hashmap_alloc(max_count, hashmap_hash_cb, hashmap_key_compare_cb);
  blk_cache->chunks = (chunk_t **)calloc(max_count, sizeof(void *));
  /*
  for (size_t i = 0; i < max_count; i++)
  {
    char buf[64] = {'\0'};
    snprintf((char *)&buf, 64, "chunk_%ld", i);
    size_t buf_len = strlen((char *)&buf);
    chunk_t *blk = (chunk_t *)calloc(1, sizeof(chunk_t));
    chunk_init(blk, i, NULL);
    c_hashmap_put(blk_cache->cache, (char *)&buf, buf_len, blk, sizeof(void *));
    blk_cache->chunks[i] = blk;
  }
  */
  return blk_cache;
}

int chunk_cache_remove(chunk_cache_t *cc, void *key, size_t key_sz)
{
  if (cc != NULL && cc->cache != NULL)
  {
    return c_hashmap_del(cc->cache, key, key_sz);
  }
  return -1;
}
int chunk_cache_add(chunk_cache_t *cc, void *key, size_t key_sz, void *value, size_t val_sz)
{
  if (cc != NULL && cc->cache != NULL)
  {

    return c_hashmap_put(cc->cache, key, key_sz, value, val_sz);
  }
  return -1;
}
void chunk_cache_destroy(chunk_cache_t *cc)
{
  if (cc != NULL)
  {
    c_hashmap_set_free_cb(cc->cache, free, hashmap_free_cb);
    if (cc->chunks != NULL)
    {
      free(cc->chunks);
      cc->chunks = NULL;
    }
  }
}