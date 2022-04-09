#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */

#include "wrapsock.h"
#include "ws_helpers.h"

#define MAXCLIENTS 10



int main(int argc, char **argv) {

    if(argc != 2) {
        fprintf(stderr, "Usage: wserver <port>\n");
        exit(1);
    }
    unsigned short port = (unsigned short)atoi(argv[1]);
    int listenfd;
    struct clientstate client[MAXCLIENTS];


    // Set up the socket to which the clients will connect
    listenfd = setupServerSocket(port);

    initClients(client, MAXCLIENTS);

    
    // First, we prepare to listen to multiple
    // file descriptors by initializing a set of file descriptors.
    int client_count = 0;
    int max_fd = listenfd;
    fd_set all_fds;
    FD_ZERO(&all_fds);
    FD_SET(listenfd, &all_fds);


    while(client_count != MAXCLIENTS + 1) {

        //timer stuff for select's timeout 
        struct timeval timeout;
        timeout.tv_sec = 300; //five minutes
        timeout.tv_usec = 0;

        fd_set client_fds = all_fds;

        //set up select
        int select_result = Select(max_fd + 1, &client_fds, NULL, NULL, &timeout);
        if (select_result == 0) {//timer has expired
            fprintf(stderr, "server timed out: select expired\n");
            exit(0);
        }


        // Accept a new connection (client) ...
        if (FD_ISSET(listenfd, &client_fds)) {
            int client_sock = acceptConnection(listenfd, client);
            if (client_sock > max_fd) {
                max_fd = client_sock;
            }
            FD_SET(client_sock, &all_fds);
            client_count++;
            fprintf(stderr, "Accepted a new client %d\n", client_sock);
        }


        //then check the clients and read from them if they are ready
        for (int index = 0; index < MAXCLIENTS; index++) {
            if (client[index].sock > -1 && FD_ISSET(client[index].sock, &client_fds)) {

            
                int client_fd = client[index].sock;
               

                //read from the client
                int hc_status = 0;
               
                while (hc_status == 0) {
                    
                    char line[MAXLINE + 1];

                    int num_read = read(client_fd, &line, MAXLINE);

                    fprintf(stderr, "Read %d bytes on socket for client %d \n", num_read, client_fd);
                    line[num_read] = '\0';
                    

                    //input each line of the client's input into handleClient
                    if (handleClient(&client[index], line) != 0) {
                        break;
                    }

                }
                //client must be closed
                resetClient(&client[index]);
                FD_CLR(client_fd, &all_fds);
                Close(client_fd);
                fprintf(stderr, "Client %d disconnected\n", client_fd);
                
               
            }
        }
    }
    //if we have reached max clients
    fprintf(stderr, "server: max concurrent connections\n");
    exit(0);
    

    return 0;
}






