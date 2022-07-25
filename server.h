/* See LICENSE file for copyright and license details. */
#pragma once
#include <pthread.h>

#define MAX_CONNECTIONS 5
#define MAX_RECV_LENGTH 1024
#define MAX_SEND_LENGTH 1024

extern int connected_sockets[MAX_CONNECTIONS];
extern int connections;

/* thread to run recv for each client */
extern pthread_t recv_from_client_thread;

/* recieve messages from a clients socket */
/* this function should run for each connected client */
void* recv_from_client(void* argument);
