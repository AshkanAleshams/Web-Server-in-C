#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* A program to simulate a CGI program that takes a long time. */

int main() {
    char *qstr;
    fprintf(stderr, "Starting output for slowcgi\n");
    printf("Content-type: text/html\r\n\r\n");
    printf("<html><head><title>Hello World</title></head>\n");
    printf("<body>\n");
    printf("<h2>SlowCGI<h2>\n");
    
    if((qstr = getenv("QUERY_STRING")) != NULL) {
        printf("<p>QUERY_STRING = %s</p>\n", qstr);
    }
    
    fprintf(stderr, "slowcgi sleeping\n");
    sleep(5);
    printf("</body></html>\n");
    fprintf(stderr, "DONE\n");
    return 0;
}
