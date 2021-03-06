/*************************************************************************
    > File Name: utils.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Fri 08 Apr 2022 11:31:01 PM UTC
 ************************************************************************/

#ifndef _UTILS_H
#define _UTILS_H
#include <stdint.h>
#include <stddef.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <pthread.h>
#include <dirent.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <sys/stat.h>
#define CHUNK_OP_LEN (65535)
typedef uint32_t (*c_hashmap_hash_cb)(void *, size_t);
typedef int (*c_hashmap_traverse_cb)(void *, void *);
typedef int (*c_hashmap_key_compare_cb)(void *, void *, size_t);

typedef void (*c_hashmap_free_cb)(void *);
int gen_uuid(char *buf,size_t n);
bool is_digit(const char *buf,size_t n);
int string_split(char *buf,char delimit,char ***ptr,size_t *count);
#endif
