/*************************************************************************
  > File Name: dict.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: 六  9/12 23:08:20 2020
 ************************************************************************/

#include "dict.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define DM_DELTA 0x9E3779B9
#define DM_FULLROUNDS 10
#define DM_PARTROUNDS 6
#if !defined(get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8) + (uint32_t)(((const uint8_t *)(d))[0]))
#endif
typedef struct dict_data_pair_t
{
  struct dict_data_pair_t *next;
  char *key;
  void *val;
} dict_data_pair;
inline static dict_data_pair *dict_data_pair_create(const char *key,void *val)
{
  dict_data_pair *dp = (dict_data_pair *)calloc(1, sizeof(dict_data_pair));
  dp->next = NULL;
  dp->key = strdup(key);
  dp->val = val;
  return dp;
}

static uint64_t dict_jump_consistent(uint64_t key, uint32_t num_buckets)
{

  int64_t b = -1, j = 0;
  uint32_t value = 0;

  while (j < num_buckets)
  {
    b = j;
    key = key * 2862933555777941757ULL + 1;
    j = (int64_t)((b + 1) * ((double)(1LL << 31) / (double)((key >> 33) + 1)));
  }
  value = (b < 0) ? (~b + 1) : b;
  return value;
}
uint32_t dict_default_hash_fn(const char *key, size_t key_length)
{
    uint32_t hash = 0, tmp;
    int rem;

    if (key_length <= 0 || key == NULL)
    {
        return 0;
    }

    rem = key_length & 3;
    key_length >>= 2;

    for (; key_length > 0; key_length--)
    {
        hash += get16bits(key);
        tmp = (get16bits(key + 2) << 11) ^ hash;
        hash = (hash << 16) ^ tmp;
        key += 2 * sizeof(uint16_t);
        hash += hash >> 11;
    }

    switch (rem)
    {
    case 3:
        hash += get16bits(key);
        hash ^= hash << 16;
        hash ^= (uint32_t)key[sizeof(uint16_t)] << 18;
        hash += hash >> 11;
        break;

    case 2:
        hash += get16bits(key);
        hash ^= hash << 11;
        hash += hash >> 17;
        break;

    case 1:
        hash += (unsigned char)(*key);
        hash ^= hash << 10;
        hash += hash >> 1;

    default:
        break;
    }

    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}
int dict_init(dict_t *d, uint32_t max_count, dict_hash_fn hash_fn)
{
  memset(d, 0, sizeof(*d));
  d->max_count = max_count;
  d->count = 0;
  d->hash_fn = (hash_fn == NULL) ? (dict_hash_fn)dict_default_hash_fn : hash_fn;
  d->members = (void *)calloc(max_count, sizeof(void *));
  d->member_count = (uint32_t *)calloc(max_count, sizeof(uint32_t));
  return 0;
}
dict_t *dict_create(uint32_t max_count, dict_hash_fn hash_fn)
{
  dict_t *d = (dict_t *)calloc(1, sizeof(dict_t));
  if (dict_init(d, max_count, hash_fn) != 0)
  {
    free(d);
    d = NULL;
  }
  return d;
}
static dict_data_pair *dict_find(dict_t *d, const char *key, uint32_t  *index_ptr,dict_data_pair **prev)
{
    size_t key_len = strlen(key);
  uint32_t hash = d->hash_fn(key, key_len);
  uint32_t index = dict_jump_consistent(hash, d->max_count);
  *index_ptr =index;
  dict_data_pair *cur = NULL;
  dict_data_pair *data = NULL;
  if (d->member_count[index] > 0)
  {
    for (cur = (dict_data_pair *)d->members[index]; cur != NULL; cur = cur->next)
    {
      if (strncmp(cur->key, key, key_len) == 0)
      {
        data = cur;
        break;
      }
      *prev = cur;
    }
  }
  return data;
}
void *dict_put(dict_t *d, char *key, void *val)
{

  uint32_t index = 0;
    dict_data_pair *prev = NULL;

  dict_data_pair *data = dict_find(d, key, &index,&prev);
  if (data != NULL)
  {
    return NULL;
  }
  data = dict_data_pair_create(key,val);
  if (d->members[index] == NULL)
  {
    d->members[index] = data;
  }
  else
  {
    data->next = d->members[index];
    d->members[index] = data;
  }
  __sync_fetch_and_add(&d->member_count[index], 1);
  __sync_fetch_and_add(&d->count, 1);
  return (void *)&data->val;
}
void *dict_get(dict_t *d, char *key)
{
  void *val = NULL;
  uint32_t index = 0;
  dict_data_pair *prev = NULL;
  dict_data_pair *dp = dict_find(d, key, &index,&prev);
  if (dp != NULL)
  {
    val = dp->val;
  }
  return val;
}
int dict_del(dict_t *d, char *key, dict_data_free_fn fn)
{
  dict_data_pair *prev = NULL;
  uint32_t index = 0;
  dict_data_pair *dp = dict_find(d, key, &index,&prev);
  if (dp != NULL)
  {
    if (prev == NULL)
    {
      d->members[index] = dp->next;
    }
    else
    {
      prev->next = dp->next;
    }
    dp->next = NULL;
    free(dp->key);
    if (fn != NULL)
    {
      fn(dp->val);
    }
    free(dp);
    __sync_fetch_and_sub(&d->member_count[index], 1);
    __sync_fetch_and_sub(&d->count, 1);
    return 0;
  }
  return -1;
}
void dict_dump(dict_t *d, dict_cb_fn cb)
{
  size_t i = 0;
  for (; i < d->max_count; i++)
  {
    if (d->member_count[i] > 0)
    {
      fprintf(stdout, "---------------dump index =%d ---------------\n", i);
      dict_data_pair *dp = (dict_data_pair *)d->members[i];
      while (dp != NULL)
      {
        char *key = dp->key;
        void *data = (void *)&dp->val;
        cb(key, data);
        dp = dp->next;
      }
    }
  }
}
void dict_deinit(dict_t *d, dict_data_free_fn free_cb)
{
  size_t i;
  for (i = 0; i < d->max_count; i++)
  {
    if (d->member_count[i] > 0)
    {
      dict_data_pair *dp = (dict_data_pair *)d->members[i];
      while (dp != NULL)
      {
        dict_data_pair *next = dp->next;
        free(dp->key);
        if (free_cb != NULL)
        {
          void *val = &dp->val;
          free_cb(val);
        }

        free(dp);
        dp = next;
        __sync_fetch_and_sub(&d->member_count[i], 1);
        __sync_fetch_and_sub(&d->count, 1);
      }
    }
  }
}
void dict_destroy(dict_t *d, dict_data_free_fn free_cb)
{
  if (d != NULL)
  {
    dict_deinit(d, free_cb);
    free(d);
    d = NULL;
  }
}