#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "ws_helpers.h"
#include "wrapsock.h"

void startCGI(struct clientstate *cs);
char *getPath(char *str);

/* Process the HTTP request in cs
 *    If the path is an allowed program, create a child process,
 *    set up a pipe, redirect standard output to the pipe, and execute 
 *    the program
 * Return the pipe descriptor from which the parent process will read the 
 * CGI output
 * Return -1 if there is an error
 */
int processRequest(struct clientstate *cs) {

    // Check if the program requested is in the allowed set
    if(!validResource(cs->path)) {
        printNotFound(cs->sock);
        return(-1);
    }
    int result;
    fprintf(stderr, "Setting up pipe and creating child process\n");
    if(pipe(cs->fd) == -1) {
        perror("pipe"); 

        // TODO: Handle this case
        printNotFound(cs->sock);
        exit(100);

        return -1;
    }

    result = fork();
    if(result == -1) {
        fprintf(stderr, "Fork failed\n");

        // TODO: Handle this case
        printServerError(cs->sock);
        exit(-1);


        return -1;

    } else if (result == 0) {
        startCGI(cs);

    } else { // parent
        Close(cs->fd[1]);
        // return pipe for reading
        return cs->fd[0];
    } 
    return -1; /* should never get here */
}

/* Start up the CGI program by setting the environment variable
 * and calling exec
  */
void startCGI(struct clientstate *cs) {
    fprintf(stdout, "Child query_string = %s\n", cs->query_string);

    /* set env in the child so that we don't have to 
     * worry about cleaning it up in the parent 
     */
    if(cs->query_string == NULL) {
        if((setenv("QUERY_STRING", "", 1)) == -1) {
            perror("setenv");
        }
    } else {
        if((setenv("QUERY_STRING", cs->query_string, 1)) == -1) {
            perror("setenv");
        }
    }
    
    Dup2(cs->fd[1], fileno(stdout));
    Close(cs->fd[0]);
    Close(cs->fd[1]);
    
    char path[MAXLINE] = "./";
    strncat(path, cs->path, MAXLINE - strlen(path) - 1);
    fprintf(stderr, "Program to execute = %s\n", path);

    execl(path, cs->path, NULL);
    perror("execl");
    exit(100);
}


/* Extract and return the resource string from the first line of a GET request
 * stored in str 
 */
char * getPath(char *str) {
    char *path = malloc(MAXLINE);
    int num;
    if(strncmp(str, "GET", 3) != 0) {
        fprintf(stderr, "Invalid request type: %s\n", str);
        return NULL;
    }
    
    /* This is not a robust way of extracting the resource, but we aren't
     * going to worry about handling badly formatted requests.
     */
    if((num = sscanf(str, "GET /%s HTTP/1.1", path)) != 1) {
        return NULL;
    } else {
        char *ptr = strchr(path, '?');
        if(ptr != NULL) {
            *ptr = '\0';
        }
        return path;
    }
}


/* Return the query string portion of a get request
 * Assume the only question mark in the request marks
 * the beginning of a query string.
 */
char *getQuery(char *resource) {
    char *query = malloc(MAXLINE);

    // locate the question mark
    char *ptr = strchr(resource, '?');
    if(ptr == NULL) {
        query[0] = '\0';
    } else {
        strcpy(query, ptr + 1);
        // locate the next space to null-terminate the query string
        ptr = strchr(query, ' ');
        if(ptr == NULL) {
            return NULL;
        } else {
            *ptr = '\0';
        }
    }
    return query;
}
