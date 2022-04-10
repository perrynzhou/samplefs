/*************************************************************************
  > File Name: net_sock_t.h
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 11 Nov 2021 05:46:12 PM CST
 ************************************************************************/

#ifndef _net_sock_t_H
#define _net_sock_t_H
#include <uv.h>
typedef struct
{
    uv_pipe_t pipe;
    pthread_t thread;
    uv_loop_t *thread_loop;
    int notify_receive_fd;
    uint64_t counter;
    int id;
    void *ctx;
} net_sock_thread_t;
typedef struct
{
    uv_tcp_t server;
    uv_pipe_t *pipe;
    uv_buf_t dummy_buf;
    char *addr;
    uv_loop_t *main_loop;
    int backlog;
    int port;
    uint64_t counter;
    int num_threads;
    net_sock_thread_t *threads;
    uint64_t round_robin_counter;
} net_sock_t;
net_sock_t *net_sock_create(char *addr, int port, int backlog, int num_threads);
void net_sock_destroy(net_sock_t *sk);
int net_sock_run(net_sock_t *sk);
int net_sock_init(net_sock_t *sk, char *addr, int port, int backlog, int num_threads);
void net_sock_deinit(net_sock_t *sk);
#endif