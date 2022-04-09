#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>

/* A sample CGI program that terminates through a signal to test the 500 
 * error */

int main() {
    
    printf("Content-type: text/html\n\n");
    printf("<html><head><title>Hello World</title></head>\n");
    printf("<body>\n");
    printf("<h2>Hello, world! I'm about to terminate</h2>\n");
    fflush(stdout);
    sleep(5);
    kill(getpid(), SIGKILL);
    printf("<p> I should never get here</p>\n");
    printf("</body></html>\n");
    return 0;

}
