#include <stdio.h>

int validResource(char *);

/* Test validResource */
int main() {
    char *s[] = {"/simple",
         "simple",
         "/tmp/simple",
         "/../simple",
        "/notvalid",
         "/term",
         "term",
         "slowcgi"};

    int i;
    for(i = 0; i < 8; i++) {
    if(validResource(s[i])) 
        printf("%s is a valid resource string\n", s[i]);
    else 
        printf("%s is not valid resource string\n", s[i]);
    }
    return 0;
}
