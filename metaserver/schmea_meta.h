/*************************************************************************
    > File Name: schmea_cache.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Tue 15 Mar 2022 07:46:54 AM UTC
 ************************************************************************/

#ifndef B5AF70E3_5A31_487F_9B65_7738A9E02CB9
#define B5AF70E3_5A31_487F_9B65_7738A9E02CB9

#ifndef _SCHMEA_META_CACHE_H
#define _SCHMEA_META_CACHE_H
#include <stdio.h>
#include "kv_db.h"
#include "dict.h"
#include "bitmap.h"

typedef struct
{
  uint32_t kv_count;
  uint64_t bytes;
  bool is_active;
  bitmap_t *bitmap;
} schema_meta_rec_t;

static char *SYS_SCHEMA_META_TABLE_NAME="sys_schema_meta";
int schmea_meta_rec_assign(schema_meta_rec_t *meta,uint32_t key_count,bool is_active,uint64_t bytes);
int schmea_meta_rec_swap(schema_meta_rec_t *src,schema_meta_rec_t *dst);

schema_meta_rec_t  *schmea_meta_rec_fetch(char *meta_schema_name, char *key,kv_db_t *db);
char *schmea_meta_rec_dump(schema_meta_rec_t* rec);
int  schmea_meta_rec_save( char *schema_name, char *key,void *val,size_t val_size,kv_db_t *db);

int schema_cache_load(char *meta_name,kv_db_t *db);
int schmea_cache_add(dict_t *cache, kv_db_t *db, char *schema_name, char *key, void *val, size_t val_sz);
int schmea_cache_del(dict_t *cache, kv_db_t *db, char *schema_name,  char *key);

#endif


#endif /* B5AF70E3_5A31_487F_9B65_7738A9E02CB9 */
