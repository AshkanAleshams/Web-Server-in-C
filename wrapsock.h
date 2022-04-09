#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>    /* Internet domain header */

#define LISTENQ 10

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr);
void Bind(int fd, const struct sockaddr *sa, socklen_t salen);

void Listen(int fd, int backlog);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds,
           struct timeval *timeout);

int Socket(int family, int type, int protocol);
void Close(int fd);
void Dup2(int oldfd, int newfd);

int setupServerSocket(unsigned short port);


