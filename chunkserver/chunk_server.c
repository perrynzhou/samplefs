/*************************************************************************
    > File Name: chunk_server.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sun 10 Apr 2022 11:06:15 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "chunk_server.h"
#include "conf.h"
chunk_server_t *chunk_server_alloc(conf_t *cf)
{
  chunk_server_t *srv = (chunk_server_t *)calloc(1,sizeof(chunk_server_t));
  assert(srv !=NULL);
  //net_sock_t *net_sock_create(char *addr, int port, int backlog, int num_threads);
  net_sock_t *chunk_sock = net_sock_create("127.0.0.1",cf->port,cf->tcp_backlog,cf->work_thread_num);
  srv->chunk_sock = chunk_sock;
  srv->root = strdup(cf->chunk_root);

  srv->chunk_cache = chunk_cache_alloc(cf->chunk_count);
  srv->chunk_count = cf->chunk_count;
  char **meta_addr = NULL;
  size_t count = 0;
  string_split(cf->metaserver_addr,':',&meta_addr,&count);
  srv->meta_sock =  net_sock_create(meta_addr[0],atoi(meta_addr[1]),cf->tcp_backlog,1);
  srv->chunk_per_size = cf->chunk_per_size;
  srv->chunk_count = cf->chunk_count;
  srv->chunks = (chunk_t *)calloc(cf->chunk_count,sizeof(chunk_t));
  assert(srv->chunks!=NULL);
  for(size_t i=0;i<cf->chunk_count;i++)
  {
      chunk_init(&srv->chunks[i],i,NULL);
  }
  return srv;
}
void chunk_server_start(chunk_server_t *srv)
{
  net_sock_run(srv->chunk_sock);
  net_sock_run(srv->meta_sock);
}
void chunk_server_destroy(chunk_server_t *srv)
{
  net_sock_destroy(srv->chunk_sock);
  net_sock_destroy(srv->chunk_sock);
  chunk_cache_destroy(srv->chunk_cache);
  for(size_t i=0;i<srv->chunk_count;i++)
  {
    chunk_deinit(&srv->chunks[i]);
  }
  free(srv->chunks);
}