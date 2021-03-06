/*************************************************************************
  > File Name: conf.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 11 Nov 2021 03:53:57 PM CST
 ************************************************************************/

#ifndef _CONF_H
#define _CONF_H
#include "utils.h"
typedef struct  {
    char *chunk_root;
    char *metaserver_addr;
    int  tcp_backlog;
    int port;
    int work_thread_num;
    size_t chunk_per_size;
    size_t chunk_count;
}conf_t;
conf_t *conf_create(const char *conf_file);
void conf_dum( conf_t *m_conf);
void conf_destroy( conf_t *mconf);
#endif