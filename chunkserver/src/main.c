/*************************************************************************
    > File Name: main.c
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Sat 09 Apr 2022 06:40:03 AM UTC
 ************************************************************************/

#include <stdio.h>
#include "chunk.h"
#include "chunk_cache.h"
#include "c_hash.h"
#include "c_hashmap.h"
int main() {
  chunk_cache_t  *blk=chunk_cache_new(10);
  chunk_cache_destroy(blk);
  return 0;
}