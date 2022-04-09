/*************************************************************************
  > File Name: conf.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Wednesday, September 09, 2020 AM08:34:22
 ************************************************************************/

#ifndef _CONF_H
#define _CONF_H
#include <jansson.h>
typedef struct
{
   char *conf_file;
   json_t *root;
} conf_t;

conf_t *conf_alloc(const char *conf_file);
json_t *conf_search(conf_t *cf,const char *key);
void conf_free(conf_t *cf);

#endif
