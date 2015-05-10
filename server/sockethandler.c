/****************************************************************************
 * Copyright (C) 2014 by Lukas Elsner                                       *
 *                                                                          *
 * This file is part of calory-counter.                                     *
 *                                                                          *
 ****************************************************************************/

/**
 * @file sockethandler.c
 * @author Lukas Elsner
 * @date 01-09-2014
 * @brief File containing the sockethandler structure and its member methods.
 *
 */
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/select.h>
#include <semaphore.h>
#include <pthread.h>
#include <assert.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "../lib/sock.h"
#include "../lib/food.h"
#include "../lib/foodlist.h"
#include "sockethandler.h"

#define MAX_THREADS 10 /**< Size of the Threadpool */
#define MAX_SOCKETS 5 /**< Maximum number of waiting clients */

/**
 * @brief sockethandler structure for representing a sockethandler item
 *
 */
struct sockethandler {
  unsigned int listen_port; /**< Listen port for the server socket */
  pthread_t thread_pool[MAX_THREADS]; /**< Thread pool for handling client connections */
  int client_sockets[MAX_SOCKETS]; /**< Array of client sockets for consumer/producer principle */
  bool shutdown; /**< Flag to notifying all threads to shut down */
  foodlist *foodlist; /**< List of foods to work with */
  pthread_mutex_t mutex;/**< Mutex to mutual exclude the client_socket array. */
  sem_t empty;/**< Semaphore to block on empty socket list. */
  sem_t full;/**< Semaphore to block on full socket list. */
  size_t in; /**< position of producer in buffer */
  size_t out; /**< position of consumer in buffer */
  size_t count; /**< number of unconsumed items */
};

/**
 * @brief Method for client connection handling
 * @param sockethandler* A pointer to a valid sockethandler structure
 *
 * Every Thread is a consumer for the client_sockets[] array. If a socket is available, it is popped out
 * by one of the threads and served in a loop until the connection closes. After that, the thread waits for
 * its next client socket.
 *
 * */
void sockethandler_client_thread_func(sockethandler *s)
{
  while(!s->shutdown) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += 5;
    int sock = -1;
    if (sem_timedwait(&s->full, &ts) == 0) {
      /* Acquire mutex lock to protect buffer */
      pthread_mutex_lock(&(s->mutex));

      sock = s->client_sockets[s->out];
      s->out++;
      s->out %= MAX_SOCKETS;
      assert(s->count > 0);
      s->count--;
      assert(s->count == (s->in + MAX_SOCKETS - s->out) % MAX_SOCKETS);

      /* Release mutex lock and full semaphore */
      pthread_mutex_unlock(&(s->mutex));
      sem_post(&s->empty);

      struct timeval timeout;
      timeout.tv_sec = 5;
      timeout.tv_usec = 0;
      setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout));

      /* Receive a message from client */
      while( !s->shutdown ) {
        char buf[BUF_LEN] = { 0 };
        int r_len = sock_read(sock, buf);
        /* Client is disconnected */
        if(r_len == 0) {
          break;
        }
        if(r_len == -1) {
          /* 5 second timeout to be responsive to shutdown event, just try to read again */
          continue;
        }
        if(!strncmp("SEARCH:", buf, 7)) {
          /* client is searches for something */
          buf[strlen(buf) - 1] = 0; /* remove newline character */
          printf("Client %d is searching for some %s\n", sock, buf + 7);
          size_t n = 0;
          food **foods = foodlist_find(s->foodlist, buf + 7, &n);
          char cbuf[BUF_LEN] = { 0 };
          snprintf(cbuf, BUF_LEN, "%zu", n);
          if(sock_send_count(sock, cbuf)) {
            for(int i = 0; i < n; ++i) {
              food *f = foods[i];
              char *c = food_serialize(f);
              if(!sock_send_food(sock, c)) {
                printf("error sending food\n");
              }
              free(c);
            }
            free(foods);
          }
          printf("Sent %zu food items to client %d\n", n, sock);
        } else if(!strncmp("FOOD:", buf, 5)) {
          /* client adds some food */
          printf("Client %d wants to add food\n", sock);
          food *f = food_deserialize(buf + 5);
          foodlist_append(s->foodlist, &f);
          printf("Client %d added some %s\n", sock, food_get_name(f));
          continue;
        } else {
          printf("Error in protocol, expected SEARCH|FOOD");
        }
      }
      printf("Closing socket %d\n", sock);
      shutdown(sock, 2);
      close(sock);
    }
  }
}

sockethandler *sockethandler_init(foodlist * fl)
{
  sockethandler *s = (sockethandler *)malloc(sizeof(sockethandler));

  s->shutdown = false;
  s->foodlist = fl;
  s->in = 0;
  s->out = 0;
  s->count = 0;

  /* initialize semaphores and mutex */
  sem_init(&(s->empty), 0, MAX_SOCKETS);
  sem_init(&(s->full), 0, 0);
  pthread_mutex_init(&(s->mutex), NULL);

  s->listen_port = 11184;

  /* set of attributes for the thread */
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  for(int i = 0; i < MAX_THREADS; ++i) {
    /* create threads */
    pthread_create(&s->thread_pool[i], &attr, (void *(*)(void *))sockethandler_client_thread_func, (void *)s);
  }

  for(int i = 0; i < MAX_SOCKETS; ++i) {
    s->client_sockets[i] = 0;
  }

  return s;
}


void sockethandler_server_thread_func(sockethandler * s)
{
  while (!s->shutdown) {
    int socket_desc;
    int client_sock;
    struct sockaddr_in server;
    struct sockaddr_in client;

    /* create socket */
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1) {
      perror("Could not create socket, trying again in 5 seconds");
      sleep(5);
      continue;
    }

    /* Prepare the sockaddr_in structure */
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(s->listen_port);

    /* Bind */
    if( bind(socket_desc, (struct sockaddr *)&server , sizeof(server)) < 0) {
      printf("Could not bind to port %u, trying again in 5 seconds\n", s->listen_port);
        close(socket_desc);
      sleep(5);
      continue;
    }

    /* Listen */
    listen(socket_desc , 3);

    printf("Server bound to port %u, waiting for incoming connections\n", s->listen_port);

    while (!s->shutdown) {
      fd_set set;
      FD_ZERO(&set); /* clear the set */
      FD_SET(socket_desc, &set); /* add our file descriptor to the set */
      struct timeval timeout;
      timeout.tv_sec = 5;
      timeout.tv_usec = 0;

      int rv = select(socket_desc + 1, &set, NULL, NULL, &timeout);

      if(rv == -1) {
        perror("select"); /* an error accured */
        continue;
      } else if(rv == 0) {
        continue;
      } else {
        int len = sizeof(struct sockaddr_in);
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&len);
        if (client_sock < 0) {
          perror("accept failed");
          continue;
        } else {
          printf("New connection from %s on socket %d\n", inet_ntoa(client.sin_addr), client_sock);

          struct timespec ts;
          clock_gettime(CLOCK_REALTIME, &ts);
          ts.tv_sec += 5;

          if (sem_timedwait(&s->empty, &ts) == 0) {

            /* Acquire mutex lock to protect buffer */
            pthread_mutex_lock(&(s->mutex));

            s->client_sockets[s->in] = client_sock;
            s->in++;
            s->in %= MAX_SOCKETS;
            assert(s->count < MAX_SOCKETS);
            s->count++;

            /* Release mutex lock and full semaphore */
            pthread_mutex_unlock(&(s->mutex));
            sem_post(&s->full);
          }
        }
      }
    }
  }
}

void sockethandler_set_port(sockethandler * s, int port)
{
  s->listen_port = port;
}

void sockethandler_shutdown(sockethandler * s)
{
  s->shutdown = true;
  for(int i = 0; i < MAX_THREADS; ++i) {
    pthread_join(s->thread_pool[i], NULL);
  }
}

void sockethandler_destroy(sockethandler * s)
{
  pthread_mutex_destroy(&s->mutex);
  sem_destroy(&s->full);
  sem_destroy(&s->empty);
  free(s);
}












