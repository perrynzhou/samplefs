/*************************************************************************
    > File Name: chunk_node.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 09 Apr 2022 11:58:04 AM UTC
 ************************************************************************/

#ifndef _chunk_node_meta_META_H
#define _chunk_node_meta_META_H
#include "conf.h"
#include "utils.h"
#include "net_sock.h"
typedef struct
{
   int node_id;
   size_t total_bytes;
   size_t chunk_size;
   char node_addr[0];
} chunk_node_meta_t;
chunk_node_meta_meta_t *chunk_node_meta_alloc(conf_t *cf);
int chunk_node_meta_start(chunk_node_meta_t *node_meta,net_sock_t *meta);
void chunk_node_meta_destroy(chunk_node_meta_t *cn);
#endif
