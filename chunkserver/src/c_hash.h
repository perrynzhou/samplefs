/*************************************************************************
    > File Name: c_c_hash.h
  > Author:perrynzhou 
  > Mail:perrynzhou@gmail.com 
  > Created Time: Tue 22 Mar 2022 08:41:36 AM UTC
 ************************************************************************/

#ifndef _C_HASH_FN_H
#define _C_HASH_FN_H
#include "utils.h"
uint32_t c_hash_crc16(const char* key, size_t key_length);
uint32_t c_hash_crc32(const char *key, size_t key_length);
uint32_t c_hash_crc32a(const char *key, size_t key_length);
uint32_t c_hash_fnv1_64(const char *key, size_t key_length);
uint32_t c_hash_hsieh(const char *key, size_t key_length);
uint32_t c_hash_jenkins(const char *key, size_t length);
uint32_t c_hash_fnv1a_64(const char *key, size_t key_length);
uint32_t c_hash_murmur(const char *key, size_t length);
uint32_t c_hash_fnv1_32(const char *key, size_t key_length);
uint32_t c_hash_fnv1a_32(const char *key, size_t key_length);
uint32_t c_hash_one_at_a_time(const char *key, size_t key_length);
uint32_t c_hash_jump_consistent(uint64_t key, int32_t num_buckets);
uint64_t c_hash_gfs(const char *msg, int len);
uint32_t c_hash_super_fast(const char *data, int32_t len);
void c_hash_murmurHash3_x64_128 ( const void * key, int len, uint32_t seed, void * out);

#endif
