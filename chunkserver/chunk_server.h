/*************************************************************************
    > File Name: chunk_server.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sun 10 Apr 2022 11:06:11 AM UTC
 ************************************************************************/

#ifndef _CHUNK_SERVER_H
#define _CHUNK_SERVER_H
#include "chunk.h"
#include "chunk_node_meta.h"
#include "net_sock.h"
#include "chunk_cache.h"
typedef struct
{
  char *root;
  chunk_node_meta_t *node_meta;
  net_sock_t *meta_sock;
  net_sock_t *chunk_sock;
  chunk_t *chunks;
  chunk_cache_t *chunk_cache;
  size_t chunk_per_size;
  size_t chunk_count;
  char node_addr[0];
} chunk_server_t;
chunk_server_t *chunk_server_alloc(conf_t *cf);
void chunk_server_start(chunk_server_t *srv);
void chunk_server_destroy(chunk_server_t *cn);
#endif
