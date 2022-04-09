#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */

#include "wrapsock.h"

/* Socket function wrappers that incorporate error checking
 */

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr) {
    int  n;

    if ( (n = accept(fd, sa, salenptr)) < 0) {
        perror("accept error");
        exit(1);
    }
    return n;
}


void Bind(int fd, const struct sockaddr *sa, socklen_t salen) {
    if (bind(fd, sa, salen) < 0){
        perror("bind error");
        exit(1);
    }
}


void Listen(int fd, int backlog) {
    if (listen(fd, backlog) < 0) {
        perror("listen error");
        exit(1);
    }
}


int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
        struct timeval *timeout) {

    int n;
    if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0) {
        perror("select error");
        exit(1);
    }
    return n;              /* can return 0 on timeout */
}


int Socket(int family, int type, int protocol) {
    int n;

    if ( (n = socket(family, type, protocol)) < 0) {
        perror("socket error");
        exit(1);
    }
    return n;
}

void Close(int fd) {
    if (close(fd) == -1) {
        perror("close error");
        exit(1);
    }
}


void Dup2(int oldfd, int newfd) {
    if(dup2(oldfd, newfd) == -1) {
        perror("dup2");
        exit(1);
    }
}

/*
 * Create and set up a socket for a server to listen on.
 */
int setupServerSocket(unsigned short port) {
    int soc = Socket(PF_INET, SOCK_STREAM, 0);

    // Make sure we can reuse the port immediately after the
    // server terminates. Avoids the "address in use" error
    int on = 1;
    int status = setsockopt(soc, SOL_SOCKET, SO_REUSEADDR, 
                            (const char *) &on, sizeof(on));
    if (status < 0) {
        perror("setsockopt");
        exit(1);
    }

    struct sockaddr_in addr;

    // Allow sockets across machines.
    addr.sin_family = PF_INET;
    // The port the process will listen on.
    addr.sin_port = htons(port);
    // Clear this field; sin_zero is used for padding for the struct.
    memset(&(addr.sin_zero), 0, 8);
    // Listen on all network interfaces.
    addr.sin_addr.s_addr = INADDR_ANY;

    // Associate the process with the address and a port
    if (bind(soc, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        // bind failed; could be because port is in use.
        perror("bind");
        exit(1);
    }

    // Set up a queue in the kernel to hold pending connections.
    if (listen(soc, LISTENQ) < 0) {
        // listen failed
        perror("listen");
        exit(1);
    }

    return soc;
}

