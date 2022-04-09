#include <stdio.h>
#include <string.h>

/* Initialize an array of allowed programs.  You may change this
 * array if you want to add other tests, but you must use
 * validResource in processRequest to validate the url request.
 * 
 */
#define MAXPROGS 4

char *progs[MAXPROGS] = {"slowcgi", "term", "simple", "large"};

/* Return 0 if str is NULL or if str is not in the list of valid programs to
 * run. Note that str does not begin with '/', nor does it contain the 
 * optional '?' or the arguments that follow.
 *
 * For example, if your web server receives the request
 *    GET /simple?filenum=2 HTTP/1.1
 * the resource is "/simple?filenum=2", and you will call validResource()
 * on "simple" (the string between '/' and '?') You must not call execl
 * unless validResource returns 1.
 */
int validResource(char *str) { 
    if(str == NULL) {
        return 0;
    }

    for(int i = 0; i < MAXPROGS; i++) {
        if(strcmp(str, progs[i]) == 0) {
            return 1;
        }
    }
    /* str did not match any of the valid programs */
    return 0;
}

