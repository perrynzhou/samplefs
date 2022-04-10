/*************************************************************************
    > File Name: chunk.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Fri 08 Apr 2022 11:28:01 PM UTC
 ************************************************************************/

#include "chunk.h"
int min(int a, int b)
{
  if (a < b)
  {
    return a;
  }
  return b;
}

void *chunk_heal_check(void *arg)
{
  return arg;
}
int chunk_init(chunk_t *blk, int id, void *ctx)
{
  if (blk != NULL && id >= 0)
  {
    char buf[64] = {'\0'};
    snprintf((char *)&buf, 64, "%d.chunk", id);
    int fd = open((char *)&buf, O_RDWR | O_CREAT | O_APPEND, 0644);
    assert(fd != -1);
    blk->fd = fd;
    blk->offset = 0;
    blk->chunk_id = id;
    blk->ctx = ctx;
    pthread_create(&blk->thd, NULL, chunk_heal_check, blk);
    return 0;
  }
  return -1;
}
chunk_t * chunk_alloc(int id, void *ctx) {
   chunk_t *ck = (chunk_t *)calloc(1,sizeof(chunk_t));
   assert(ck !=NULL);
   if(chunk_init(ck,id,ctx) !=0) {

     free(ck);
     ck = NULL;
   }
   return ck;
}
ssize_t chunk_write(chunk_t *blk, void *buf, size_t pos, size_t len)
{
  lseek(blk->fd, pos, SEEK_CUR);
  size_t total = 0;
  size_t w_sz = 0;
  while (total < len)
  {
    char *ptr = (char *)buf;
    size_t batch_size = min(len - total, CHUNK_OP_LEN);
    ssize_t bytes = write(blk->fd, ptr + total, batch_size);
    if (w_sz < 0)
    {
      return -1;
    }
    total += bytes;
  }
  return total;
}
ssize_t chunk_read(chunk_t *blk, void *buf, size_t pos, size_t len)
{
  lseek(blk->fd, pos, SEEK_CUR);
  size_t total = 0;
  size_t r_sz = 0;
  while (total < len)
  {
    char *ptr = (char *)buf;
    size_t batch_size = min(len - total, CHUNK_OP_LEN);
    ssize_t bytes = read(blk->fd, ptr + total, batch_size);
    if (r_sz < 0)
    {
      return -1;
    }
    total += bytes;
  }
  return total;
}
void chunk_deinit(chunk_t *blk)
{
  if (blk != NULL)
  {
    pthread_join(blk->thd,NULL);
    close(blk->fd);
    blk->fd = -1;
  }
}

void chunk_destroy(chunk_t *ck)
{
  chunk_deinit(ck);
  if(ck !=NULL) {
     free(ck);
     ck = NULL;
  }
}