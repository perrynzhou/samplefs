/*************************************************************************
    > File Name: utils.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 09 Apr 2022 12:02:33 PM UTC
 ************************************************************************/

#include <stdio.h>
#include "utils.h"
static char runes[] = "qwertyuiopasdfghjklzxcvbnm0987654321";
int gen_uuid(char *buf, size_t n)
{
  if (buf != NULL && n > 0)
  {
    uint32_t seed = 0x19610910;
    for (size_t i = 0; i < n; i++)
    {
      size_t index = rand_r(&seed) % strlen((char *)&runes);
      buf[i] = runes[index];
    }
    return 0;
  }
  return -1;
}
bool is_digit(const char *buf,size_t n)
{

  if(buf==NULL|| n<=0) {
    return false;
  }
  for(size_t i=0;i<n;i++){
    if(isdigit(buf[i])==0)
    {
      return false;
    }
  }
  return true;
 
}