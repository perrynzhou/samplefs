/*************************************************************************
    > File Name: schmea_meta_cache.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Tue 15 Mar 2022 07:44:56 AM UTC
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <jansson.h>
#include "kv_db.h"
#include "dict.h"
#include "bitmap.h"
#include "log.h"
#include "schmea_meta.h"
#define SCHEMA_MAX_SIZE (65535)
int schmea_meta_rec_swap(schema_meta_rec_t *src,schema_meta_rec_t *dst)
{
  src->bytes = dst->bytes;
  src->kv_count = dst->kv_count;
  src->is_active = dst->is_active;
}
int schmea_meta_rec_assign(schema_meta_rec_t *meta,uint32_t key_count,bool is_active,uint64_t bytes)
{
  memset(meta,0,sizeof(*meta));
  meta->is_active=is_active;
  meta->kv_count=key_count;
  meta->bytes =bytes;
  return 0;
}
schema_meta_rec_t  *schmea_meta_rec_fetch(char *meta_schema_name, char *key,kv_db_t *db)
{
     return  (schema_meta_rec_t *)kv_db_get(db,meta_schema_name,key,strlen(key),NULL);
}
int  schmea_meta_rec_save(char *schema_name, char *key,void *val,size_t val_size,kv_db_t *db)
{
  return  kv_db_set(db,schema_name,key,strlen(key),val,val_size);
}
static int schema_cache_load_cb(void *ctx,void *key,void *val)
{
  kv_db_t *db = (kv_db_t *)ctx;
  kv_schema_t  *schema = kv_schema_alloc((char *)key,db,false);
  log_info("reload schmea=%s",schema->uri);
  schema_meta_rec_t *prev_meta = schmea_meta_rec_fetch(SYS_SCHEMA_META_TABLE_NAME,(char *)key, db);
  schema_meta_rec_t *new_meta = (schema_meta_rec_t *)calloc(1,sizeof(schema_meta_rec_t));
 new_meta->bitmap  = bitmap_alloc(UINT32_MAX>>4);

  schmea_meta_rec_assign(new_meta,prev_meta->kv_count,prev_meta->is_active,prev_meta->bytes);
  dict_put(db->schmea_meta_cache,(char *)key,new_meta);
  log_info("cache::load key=%s,count=%d,active=%d",(char *)key,prev_meta->kv_count,prev_meta->is_active);
  return (new_meta == NULL)?-1:0;
}

int schmea_cache_add(dict_t *cache,kv_db_t *db,char *schema_name, char *key,void *val,size_t val_sz)
{
  schema_meta_rec_t *meta_ptr = (schema_meta_rec_t *)val;
  log_info("cache:add key=%s,count=%d,active=%d",(char *)key,meta_ptr->kv_count,meta_ptr->is_active);

  int ret =kv_db_set(db,schema_name,key,strlen(key),val,val_sz);
  if(ret !=0) {
    return -1;
  }
  log_info("cache:add key=%s,count=%d,active=%d",(char *)key,meta_ptr->kv_count,meta_ptr->is_active);
  void *ptr =dict_put(cache,(char *)key,val);
  return (ptr == NULL)?-1:0;
}
int schmea_cache_del(dict_t *cache,kv_db_t *db,char *schema_name, char *key) {
  int ret =kv_db_del(db,schema_name,key,strlen(key));
  if(ret !=0) {
    return ret;
  }
  log_info("cache:del key=%s",(char *)key);
  dict_del(cache,key,free);
  return 0;
}
int schema_cache_load(char *meta_name,kv_db_t *db)
{
   kv_schema_t *meta = kv_db_fetch_schema(db,meta_name);
   db->schmea_meta_cache = dict_create(SCHEMA_MAX_SIZE,NULL);
   kv_schmea_search(meta,schema_cache_load_cb,db,NULL,0);
   return 0;

}
char *schmea_meta_rec_dump(schema_meta_rec_t* rec)
{
  json_t *obj  = json_object();
  json_object_set(obj, "kv_count", json_integer(rec->kv_count));
  json_object_set(obj, "total_bytes", json_integer(rec->bytes));
  int is_active = (rec->is_active)?1:0;
  json_object_set(obj, "is_active",json_integer(is_active));
  return json_dumps(obj,0);
}

