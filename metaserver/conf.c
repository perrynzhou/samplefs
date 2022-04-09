/*************************************************************************
  > File Name: conf.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Wednesday, September 09, 2020 AM08:34:22
 ************************************************************************/

#include "conf.h"
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <jansson.h>
#include <assert.h>
conf_t *conf_alloc(const char *conf_file)
{
  size_t len = 0;
  struct stat st;
  stat(conf_file, &st);
  int fd = open(conf_file, O_RDONLY, 0755);
  char *buf = calloc(st.st_size + 1, sizeof(char));
  ssize_t rz = read(fd, buf, st.st_size);
  assert(rz != -1);
  json_t *root;
  json_error_t error;

  root = json_loads(buf, 0, &error);
  assert(root != NULL);
  conf_t *cf = calloc(1, sizeof(conf_t));
  assert(cf != NULL);
  cf->root = root;
  cf->conf_file = strdup(conf_file);
  return cf;
}

json_t *conf_search(conf_t *cf, const char *key)
{

  if (cf == NULL || cf->root == NULL || key == NULL)
  {
    return NULL;
  }
  void *iter = json_object_iter(cf->root);
  json_t *object = NULL;
  size_t d_key_sz = strlen(key);
  while (iter)
  {
    char *s_key = json_object_iter_key(iter);
    size_t s_key_sz = strlen(key);
    if (s_key_sz == d_key_sz && strncmp(key, s_key, d_key_sz) == 0)
    {
      object = json_object_iter_value(iter);
      break;
    }
    iter = json_object_iter_next(cf->root, iter);
  }
  return object;
}
void conf_free(conf_t *cf)
{
  if (cf != NULL)
  {
    if (cf->root != NULL)
    {
      json_decref(cf->root);
    }
    if (cf->conf_file != NULL)
    {
      free(cf->conf_file);
    }
    free(cf);
    cf = NULL;
  }
}
