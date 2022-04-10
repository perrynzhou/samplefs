/*************************************************************************
    > File Name: utils.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Sat 09 Apr 2022 12:02:33 PM UTC
 ************************************************************************/

#include <stdio.h>
#include "utils.h"
#define STRING_SPLIT_MAX_COUNT (4096)
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
bool is_digit(const char *buf, size_t n)
{

  if (buf == NULL || n <= 0)
  {
    return false;
  }
  for (size_t i = 0; i < n; i++)
  {
    if (isdigit(buf[i]) == 0)
    {
      return false;
    }
  }
  return true;
}
int string_split(char *buf, char delimit, char ***ptr, size_t *count)
{
  if (buf != NULL && ptr != NULL)
  {
    char **tmp = (char **)calloc(STRING_SPLIT_MAX_COUNT, sizeof(char *));
    size_t j = 0;
    size_t len = strlen(buf);
    size_t k = 0;
    for (size_t i = 0; i < len; i++)
    {
      if (buf[i] == delimit)
      {
        tmp[j] = (char *)calloc(i - k + 1, sizeof(char));
        strncpy(tmp[j], buf + k, i - k);
        tmp[j++][i] = '\0';
        k = i;
        k++;
      }
    }
    if (j > 0 && k < len)
    {
      tmp[j] = (char *)calloc(len - k + 1, sizeof(char));
      strncpy(tmp[j++], buf + k, len - k);
    }
    if (j > 0)
    {
      char **ptr_result = (char **)calloc(j, sizeof(char *));
      assert(ptr_result != NULL);
      for (size_t i = 0; i < j; i++)
      {
        ptr_result[i] = strdup(tmp[i]);
      }
      *ptr = ptr_result;
      *count = j;
    }
    free(tmp);
    return 0;
  }
  return -1;
}
#ifdef TEST
int main()
{
  char *data = "127.0.0.1:9090";
  char **ptr = NULL;
  size_t count = 0;
  int ret = string_split(data, ':', &ptr, &count);
  fprintf(stdout, "count=%ld\n", count);
  return 0;
}
#endif