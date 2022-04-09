/*************************************************************************
    > File Name: chunk_node.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sat 09 Apr 2022 11:58:04 AM UTC
 ************************************************************************/

#ifndef _CHUNK_NODE_H
#define _CHUNK_NODE_H
#include "utils.h"
#include "conf.h"
#include "net_sock.h"
typedef struct  {
    char *root;
   int node_id;
   size_t  total_bytes;
   char node_addr[0];
   net_sock_t *meta_net;
   net_sock_t *chunk_net;

}chunk_node_t;
chunk_node_t *chunk_node_alloc(conf_t *cf);
void chunk_node_start(chunk_node_t *cn);
void chunk_node_destroy(chunk_node_t *cn);
#endif
