#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "wrapsock.h"
#include "ws_helpers.h"



void initClients(struct clientstate *client, int size) {
    // Initialize client array
    for (int i = 0; i < size; i++){
        client[i].sock = -1;	/* -1 indicates available entry */
        client[i].fd[0] = -1;
        client[i].request = NULL;	
        client[i].path = NULL;
        client[i].query_string = NULL;
        client[i].output = NULL;
        client[i].optr = client[i].output;
    }
}


/* Reset the client state cs.
 * Free the dynamically allocated fields
 */
void resetClient(struct clientstate *cs){
    cs->sock = -1;
    cs->fd[0] = -1;

    if(cs->path != NULL) {
        free(cs->path);
        cs->path = NULL;
    }
    if(cs->request != NULL) {
        free(cs->request);
        cs->request = NULL;
    }
    if(cs->output != NULL) {
        free(cs->output);
        cs->output = NULL;
    }
    if(cs->query_string != NULL) {
        free(cs->query_string);
        cs->query_string = NULL;
    }
}

/* Write the 404 Not Found error message on the file descriptor fd 
 */
void printNotFound(int fd) {

    char *error_str = "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
        "<html><head>\n"
        "<title>404 Not Found  </title>\n"
        "</head><body>\n"
        "<h1>Not Found (CSC209)</h1>\n"
        "<hr>\n</body>The server could not satisfy the request.</html>\n";

    int result = write(fd, error_str, strlen(error_str));
    if(result != strlen(error_str)) {
        perror("write");
    }
}

/* Write the 500 error message on the file descriptor fd 
 */
void printServerError(int fd) {

    char *error_str = "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: text/html\r\n\r\n"
        "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n"
        "<html><head>\n"
        "<title>500 Internal Server Error</title>\n"
        "</head><body>\n"
        "<h1>Internal Server Error (CSC209) </h1>\n"
        "The server encountered an internal error or\n"
        "misconfiguration and was unable to complete your request.<p>\n"
        "</body></html>\n";

    int result = write(fd, error_str, strlen(error_str));
    if(result != strlen(error_str)) {
        perror("write");
    }
}

/* Write the 200 OK response on the file descriptor fd, and write the 
 * content of the response from the string output. The string in output
 * is expected to be correctly formatted.
 */
void printOK(int fd, char *output, int length) {
    int nbytes = strlen("HTTP/1.1 200 OK\r\n");
    if(write(fd, "HTTP/1.1 200 OK\r\n", nbytes) != nbytes) {
        perror("write");
    }
    int n;
    while(length > MAXLINE) {
        n = write(fd, output, MAXLINE);
        length -= n;
        output += n;
    }
    n = write(fd, output, length);
    if(n != length) {
        perror("write");
    }
}

//*** my helpers ***


//Accepts the client and adds it to the clientstate
int acceptConnection(int fd, struct clientstate *client) {
    int client_index = 0;
    while (client_index < MAXCLIENTS && client[client_index].sock != -1) {
        client_index++;
    }

    int client_fd = accept(fd, NULL, NULL);
    if (client_fd < 0) {
        perror("server: accept");
        close(fd);
        exit(1);
    }

    client[client_index].sock = client_fd;
    return client_fd;
}



int prepareToProcess(struct clientstate *cs) {

    //now we are ready to call processRequest
        cs->output = malloc(MAXPAGE+1);
        cs->output[0] = '\0';

        //now we call processRequest and check for errors
        int child_fd = processRequest(cs);
        if (child_fd == -1){
            return -1; //no we should close the client
        }

       
            
        int output_len = 0;
        while (1) {

            char line[MAXLINE+1];

            int num_read = read(child_fd, &line, MAXLINE);
            line[num_read] = '\0';

            if ( num_read <= 0) {
                int status;
                wait(&status);

                if (WIFSIGNALED(status)) {
                    printServerError(cs->sock);
                    return -1;
                }
                cs->output[output_len-1] = '\0';
                break;

            } else {
                output_len += num_read;
                strcat(cs->output, line);
            }


        }
        //we are ready to print
        printOK(cs->sock, cs->output, output_len);
        fflush(STDIN_FILENO);
        

        //now that the request has been completed
        return 1;


}



/* Update the client state cs with the request input in line.
 * Intializes cs->request if this is the first read call from the socket.
 * Note that line must be null-terminated string.
 *
 * Return 0 if the get request message is not complete and we need to wait for
 *     more data
 * Return -1 if there is an error and the socket should be closed
 *     - Request is not a GET request
 *     - The first line of the GET request is poorly formatted (getPath, getQuery)
 * 
 * Return 1 if the get request message is complete and ready for processing
 *     cs->request will hold the complete request
 *     cs->path will hold the executable path for the CGI program
 *     cs->query will hold the query string
 *     cs->output will be allocated to hold the output of the CGI program
 *     cs->optr will point to the beginning of cs->output
 */
int handleClient(struct clientstate *cs, char *line) {
  

    //if first time calling handleClinet
    if (cs->request == NULL) {
        cs->request = malloc(MAXPAGE+1);
        cs->request[0]= '\0';
        sprintf(cs->request, "%s", line);
        
    }
    else {
        strncat(cs->request, line, strlen(line));
        
    }
    
    
    //if we find "\r\n\r\n", meaning the request is complete
    if (strstr(cs->request, "\r\n\r\n") != NULL) {
        
        //need to null terminate the request
        // cs->request[strlen(cs->request)-1] = '\0';

        //get the query
        cs->query_string = malloc(MAXLINE + 1);
        cs->query_string[0] = '\0';
        char* query = getQuery(cs->request);
        if (query == NULL) {
            return -1;
        }
        sprintf(cs->query_string, "%s", query);
        free(query);
       


        //get the path
        cs->path = malloc(MAXLINE + 1);
        cs->path[0] = '\0';
        char* path = getPath(cs->request);
        if (path == NULL) {
            return -1;
        }
        sprintf(cs->path, "%s", path);
        free(path);


        // A suggestion for printing some information about each client. 
        // You are welcome to modify or remove these print statements
        fprintf(stderr, "Client: sock = %d\n", cs->sock);
        fprintf(stderr, "        path = %s\n", cs->path);
        fprintf(stderr, "        query_string = %s\n", cs->query_string);


        // If the resource is favicon.ico we will ignore the request
        if(strcmp("favicon.ico", cs->path) == 0){
            // A suggestion for debugging output
            fprintf(stderr, "Client: sock = %d\n", cs->sock);
            fprintf(stderr, "        path = %s (ignoring)\n", cs->path);
            printNotFound(cs->sock);
            return -1;
        }

        
        //now we are ready to call processRequest so we call prepareToProcess
        return prepareToProcess(cs);
    }


    fprintf(stderr, "Request is incomplete \n");
    return 0;


}