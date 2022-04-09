/*************************************************************************
  > File Name: dict.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: 六  9/12 23:08:14 2020
 ************************************************************************/

#ifndef F10B0126_165E_4143_A2F2_F9E7D6859F7D
#define F10B0126_165E_4143_A2F2_F9E7D6859F7D

#ifndef _DICT_H
#define _DICT_H
#include <stdint.h>
#include <stdio.h>
typedef uint32_t (*dict_hash_fn)(const char *key, size_t len);
typedef int (*dict_cb_fn)(char *key,void *data);
typedef void (dict_data_free_fn)(void *data);
typedef struct
{
  uint64_t max_count;
  uint32_t count;
  uint32_t *member_count;
  void **members;
  dict_hash_fn hash_fn;
} dict_t;
int dict_init(dict_t *d, uint32_t max_count, dict_hash_fn hash_fn);
dict_t *dict_create(uint32_t max_count, dict_hash_fn hash_fn);
void *dict_put(dict_t *d, char *key, void *val);
void *dict_get(dict_t *d, char *key);
int dict_del(dict_t *d, char *key,dict_data_free_fn fn);
void dict_deinit(dict_t *d,dict_data_free_fn free_cb);
void dict_dump(dict_t *d,dict_cb_fn cb);
void dict_destroy(dict_t *d,dict_data_free_fn free_cb);
#endif


#endif /* F10B0126_165E_4143_A2F2_F9E7D6859F7D */
