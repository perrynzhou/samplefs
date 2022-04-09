/*************************************************************************
  > File Name: conf.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Thu 11 Nov 2021 03:54:03 PM CST
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <jansson.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include "conf.h"
conf_t *conf_create(const char *conf_file)
{
  conf_t *m_conf = NULL;
  char *buf = NULL;
  json_t *root = NULL;
   size_t n =0;
   size_t readn;
   int fd = -1;
  if (conf_file == NULL)
  {
    goto out;
  }
  if (access(conf_file, F_OK) != 0)
  {
    goto out;
  }
  n = (1 << 10) * (1 << 10);
  fd = open(conf_file, O_RDONLY);
  buf = (char *)malloc(n);
  readn = read(fd, buf, n);
  if (readn <= 0)
  {
    goto out;
  }
  close(fd);
  json_error_t error;

  root = json_loads(buf, 0, &error);
  if (root != NULL)
  {
    m_conf = (conf_t *)malloc(sizeof(conf_t));
    assert(m_conf != NULL);
    json_t *chunk_root = json_object_get(root, "chunk_root");
    json_t *metaserver_addr = json_object_get(root, "metaserver_addr");
    json_t *tcp_backlog = json_object_get(root, "tcp_backlog");
\
    if (!tcp_backlog || !metaserver_addr || !chunk_root)
    {
      free(m_conf);
      m_conf = NULL;
      goto out;
    }
    m_conf->chunk_root = strdup(json_string_value(chunk_root));
    m_conf->metaserver_addr = strdup(json_string_value(metaserver_addr));
    m_conf->tcp_backlog = json_integer_value(tcp_backlog);
  }
out:
  if (buf != NULL)
  {
    free(buf);
  }
  if (root != NULL)
  {
    json_decref(root);
  }
  return m_conf;
}
void conf_destroy(conf_t *mconf)
{
  if (mconf != NULL)
  {
    if (mconf->chunk_root != NULL)
    {
      free(mconf->chunk_root);
    }
    free(mconf);
    mconf = NULL;
  }
}