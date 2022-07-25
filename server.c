/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>

#include "server.h"

int connected_sockets[MAX_CONNECTIONS] = { 0 };
int connections = 0;

pthread_t recv_from_client_thread;

/* clean-ish way of killing client */
void kill_client(int* client_fd_pointer, int connection_number)
{
    close(*client_fd_pointer);
    connected_sockets[connection_number] = 0;
    --connections;
}

void* recv_from_client(void* argument)
{
    int client_fd = *(int*)argument;
    int should_quit = 0;

    while (1) {
        if (should_quit)
            break;

        char recv_message[MAX_RECV_LENGTH] = "";
        ssize_t recv_return_value = recv(client_fd, recv_message, MAX_RECV_LENGTH, 0);

        /* received a message */
        if (recv_return_value > 0) {
            /* send the clients message to all other clients */
            for (int i = 0; i < MAX_CONNECTIONS; ++i) {
                /* skip empty slots */
                if (connected_sockets[i] == 0)
                    continue;

                /* 
                 * send the author a special character indicating
                 * that their message was recieved
                 */
                if (connected_sockets[i] == client_fd) {
                    send(connected_sockets[i], "\x04", strlen("\x04"), 0);
                    continue;
                }

                /* send message to other clients */
                if (send(connected_sockets[i], recv_message, MAX_SEND_LENGTH, 0) != -1) {
                    printf("sent a message to all clients!\n");
                } else {
                    printf("failed to send a message!\n");
                }
            }
        }
        /* client disconnect */
        else if (recv_return_value == 0) {
            for (int i = 0; i < MAX_CONNECTIONS; ++i) {
                if (connected_sockets[i] == client_fd) {
                    printf("a client disconnected! (closing their socket now)\n");
                    kill_client(&client_fd, i);
                    should_quit = 1;
                    break;
                }
            }
        }
    }

    return NULL;
}
