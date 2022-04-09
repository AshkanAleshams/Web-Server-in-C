#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cgi.h"


/* A simple CGI program.  It grabs the form data from the environment variable
 * QUERY_STRING and prints it as an html list
 *
 * It first prints a header line to indicate the content type.  Note that
 * header lines are terminated with crlf (carriage return, line feed)
 * characters, and there must be a blank line between the end of the
 * headers and the beginning of the body of the http message
 */

#define CHUNK_SIZE 4096

int main() {
    char *name;

    printf("Content-type: text/html\r\n\r\n");
    printf("<html><head>\n");
    printf("<title>A Large web page</title>\n");
    printf("<link rel=\"icon\" href=\"data:,\"></head>\n");
    printf("<body>\n");
    printf("<h2>Simple CGI</h2>\n");
    if ((name = getenv("QUERY_STRING")) != NULL) {
        printf("<p>QUERY_STRING = %s</p>\n", name);
        Fdata *f = NULL;

        /* We aren't allowed to modify the string returned from getenv, so 
         * make a copy */

        if (strlen(name) > 0) {
            char *qstr = malloc(strlen(name) + 1);
            strncpy(qstr, name, strlen(name) + 1);
            
            f = parse_query(qstr);
            printf("%s", fdata2html(f));
            fdata_free(f);
        }
    }
    char data[CHUNK_SIZE + 1];
    for (int i = 0; i < 40; i++) {
        memset(data, 'a' + (i % 26), CHUNK_SIZE);
        data[CHUNK_SIZE] = '\0';
        printf("<p>%s</p>", data);
    }
    printf("</body></html>\n");
    fflush(stdout);
    return 0;
}
