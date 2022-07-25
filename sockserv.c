/* See LICENSE file for copyright and license details. */
#include <netdb.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "helpers.h"
#include "server.h"

static const char* join_message = "welcome.\n";
static const char* port = "7777";

int main(int argc, char* argv[])
{
    /* criteria for socket address structures returned from getaddrinfo */
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    /* ipv4 (af_inet) */
    hints.ai_family = AF_INET;
    /* tcp (sock_stream); connection based */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    /* variable to hold server information */
    struct addrinfo* server;

    /* get hostname/service information; stored in a pointer to a addrinfo struct */
    if (getaddrinfo(NULL, port, &hints, &server) != 0)
        die("failed to get server information!\n");

    /* get server socket file descriptor */
    int server_fd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);

    int option_value = 1;

    /* allow the re-use of the socket to prevent bind() errors */
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &option_value, sizeof(option_value)) == -1)
        die("failed to set sock options!\n");

    /* bind the socket to the port specified used with getaddrinfo */
    if (bind(server_fd, server->ai_addr, server->ai_addrlen) == -1)
        die("failed to bind socket to port!\n");

    /* free the linked list */
    freeaddrinfo(server);

    /* listen for incoming connections */
    if (listen(server_fd, MAX_CONNECTIONS) == -1)
        die("failed to listen on socket!\n");

    /* constantly accept incoming connections */
    while (1) {
        printf("waiting for a connection...\n");

        struct sockaddr_storage client_address;
        socklen_t client_address_length;
        client_address_length = sizeof(client_address);

        /*
         * "client_fd" is the most recent clients socket file descriptor, ready to send() and recv() on
         * "server_fd" is going to keep accepting incoming connections (if slots are available)
         */
        int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_address_length);

        if (client_fd == -1) {
            printf("error accepting a connection request!\n");
            continue;
        } else if (connections == MAX_CONNECTIONS) {
            printf("denied a connection request! (connections: %i/%i)", connections, MAX_CONNECTIONS);
            continue;
        }

        if (send(client_fd, join_message, strlen(join_message), 0) != -1) {
            /* setup a receive thread for the newly connected client */
            pthread_create(&recv_from_client_thread, NULL, &recv_from_client, (void*)&client_fd);

            /* add the new connection descriptor in the first available spot */
            for (int i = 0; i < MAX_CONNECTIONS; ++i) {
                /* assign the accepted client's descriptor into a free slot in the array */
                if (connected_sockets[i] == 0) {
                    connected_sockets[i] = client_fd;
                    ++connections;
                    break;
                }
            }
        } else {
            printf("could not send join message to newest connection!\n");
            continue;
        }

        printf("accepted a connection request! (connections: %i/%i)\n", connections, MAX_CONNECTIONS);
    }

    return 0;
}
