/*************************************************************************
  > File Name: net_sock.c
  > Author:perrynzhou
  > Mail:perrynzhou@gmail.com
  > Created Time: Thu 11 Nov 2021 05:47:23 PM CST
 ************************************************************************/

#include <stdio.h>

#include <pthread.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include "net_sock.h"
#include "utils.h"
typedef struct write_req
{
    uv_write_t req;
    uv_buf_t buf;
} write_req;
void free_write_req(uv_write_t *req)
{
    write_req *wr = (write_req *)req;
    free(wr->buf.base);
    free(wr);
}

void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = (char *)malloc(suggested_size);
    buf->len = suggested_size;
}

void write_cb(uv_write_t *req, int status)
{

    if (status)
    {
        fprintf(stderr, "Write error %s\n", uv_err_name(status));
    }
    free_write_req(req);
}

void read_cb(uv_stream_t *client, ssize_t nread, const uv_buf_t *buf)
{
    if (nread > 0)
    {
        write_req *req = (write_req *)malloc(sizeof(write_req));
        req->buf = uv_buf_init(buf->base, nread);
        //  fprintf(stdout, " read data=%s\n", req->buf);
        uv_write((uv_write_t *)req, client, &req->buf, 1, write_cb);
        return;
    }

    if (nread < 0)
    {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t *)client, NULL);
    }

    free(buf->base);
}

void thread_new_connection_cb(uv_stream_t *q, ssize_t nread, const uv_buf_t *buf)
{
    if (nread < 0)
    {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t *)q, NULL);
        return;
    }

    uv_pipe_t *pipe = (uv_pipe_t *)q;
    net_sock_thread_t *skt = (net_sock_thread_t *)q;
    if (!uv_pipe_pending_count(pipe))
    {
        fprintf(stderr, "No pending count\n");
        return;
    }

    uv_handle_type pending = uv_pipe_pending_type(pipe);
    assert(pending == UV_TCP);

    uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(skt->thread_loop, client);
    if (uv_accept(q, (uv_stream_t *)client) == 0)
    {
        uv_os_fd_t fd;
        uv_fileno((const uv_handle_t *)client, &fd);
        fprintf(stderr, "thread  %d: accepted fd= %d\n", skt->id, fd);
        __sync_fetch_and_add(&skt->counter, 1);
        uv_read_start((uv_stream_t *)client, alloc_buffer, read_cb);
    }
    else
    {
        uv_close((uv_handle_t *)client, NULL);
    }
}

void net_sock_thread_deinit(net_sock_thread_t *skt)
{
    if (skt != NULL)
    {
        uv_stop(skt->thread_loop);
        uv_loop_close(skt->thread_loop);
        pthread_join(skt->thread, NULL);
    }
}
void main_new_connection_cb(uv_stream_t *server, int status)
{
    if (status == -1)
    {
        return;
    }
    net_sock_t *sk = (net_sock_t *)server;
    uv_tcp_t *client = (uv_tcp_t *)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(sk->main_loop, client);
    __sync_fetch_and_add(&sk->counter, 1);

    if (uv_accept(server, (uv_stream_t *)client) == 0)
    {
        uv_write_t *write_req = (uv_write_t *)malloc(sizeof(uv_write_t));
        char *data = "a";
        sk->dummy_buf = uv_buf_init(data, 1);
        int index = sk->round_robin_counter % sk->num_threads;
        uv_write2(write_req, (uv_stream_t *)&sk->pipe[index], &sk->dummy_buf, 1, (uv_stream_t *)client, NULL);
        sk->round_robin_counter = sk->round_robin_counter + 1;
    }
    else
    {
        uv_close((uv_handle_t *)client, NULL);
    }
}

void *net_sock_thread_init(void *arg)
{

    net_sock_thread_t *skt = (net_sock_thread_t *)arg;
    net_sock_t *sk = (net_sock_t *)skt->ctx;

    skt->thread_loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
    uv_loop_init(skt->thread_loop);
    uv_pipe_init(skt->thread_loop, &skt->pipe, 1 /* ipc */);
    uv_pipe_open(&skt->pipe, skt->notify_receive_fd);
    uv_read_start((uv_stream_t *)&skt->pipe, alloc_buffer, thread_new_connection_cb);
    fprintf(stdout, "thread %d is running\n", skt->id);

    uv_run(skt->thread_loop, UV_RUN_DEFAULT);
    return NULL;
}

net_sock_t *net_sock_create(char *addr, int port, int backlog, int num_threads)
{
    net_sock_t *sk = (net_sock_t *)calloc(1, sizeof(*sk));
    assert(sk != NULL);
    net_sock_init(sk, addr, port, backlog, num_threads);
    return sk;
}
int net_sock_init(net_sock_t *sk, char *addr, int port, int backlog, int num_threads)
{
    const char *tmp_addr = "0.0.0.0";
    assert(sk != NULL);
    if (addr != NULL)
    {
        tmp_addr = addr;
    }
    sk->num_threads = num_threads;
    sk->addr = strdup(tmp_addr);
    sk->backlog = backlog;
    sk->port = port;
    sk->threads = (net_sock_thread_t *)calloc(num_threads, sizeof(net_sock_thread_t));
    sk->main_loop = (uv_loop_t *)malloc(sizeof(uv_loop_t));
    sk->pipe = (uv_pipe_t *)calloc(num_threads, sizeof(uv_pipe_t));
    uv_loop_init(sk->main_loop);

    int i;
    for (i = 0; i < num_threads; i++)
    {
        int fds[2];
        uv_pipe_init(sk->main_loop, &sk->pipe[i], 1);
        if (socketpair(AF_UNIX, SOCK_STREAM, 0, fds) != 0)
        {
            return -1;
        }
        uv_pipe_open(&sk->pipe[i], fds[0]);
        sk->threads[i].notify_receive_fd = fds[1];
    }
    uv_tcp_init(sk->main_loop, &sk->server);
    struct sockaddr_in bind_addr;
    uv_ip4_addr(sk->addr, sk->port, &bind_addr);
    uv_tcp_bind(&sk->server, (const struct sockaddr *)&bind_addr, 0);
    uv_listen((uv_stream_t *)&sk->server, sk->backlog, main_new_connection_cb);
    return 0;
}
void *net_handle_metric(void *arg)
{
    net_sock_t *sk = (net_sock_t *)arg;
    while (true)
    {
        int i;
        uint64_t success = 0;
        for (i = 0; i < sk->num_threads; i++)
        {
            success += sk->threads[i].counter;
        }
        fprintf(stdout, "total count=%ld,success=%ld,failure=%ld\n", sk->counter, success, sk->counter - success);
        /*
        for (i = 0; i < sk->num_threads; i++)
        {
            fprintf(stdout, " tid=%ld,count=%ld\n", sk->threads[i].id, sk->threads[i].counter);
        }
        */
        sleep(2);
    }
    return NULL;
}
int net_sock_run(net_sock_t *sk)
{
    int i = 0;
    for (; i < sk->num_threads; i++)
    {
        sk->threads[i].id = i;
        sk->threads[i].ctx = sk;
        pthread_create(&sk->threads[i].thread, NULL, (void *)net_sock_thread_init, (void *)&sk->threads[i]);
    }
    pthread_t id;
    pthread_create(&id, NULL, (void *)net_handle_metric, (void *)sk);

    return uv_run(sk->main_loop, UV_RUN_DEFAULT);
}
void net_sock_destroy(net_sock_t *sk)
{
    if (sk != NULL)
    {
        net_sock_deinit(sk);
        free(sk);
        sk = NULL;
    }
}
void net_sock_deinit(net_sock_t *sk)
{
    if (sk != NULL)
    {
        int i = 0;
        for (; i < sk->num_threads; i++)
        {
            net_sock_thread_deinit(&sk->threads[i]);
        }
        free(sk->threads);
        uv_stop(sk->main_loop);
    }
}