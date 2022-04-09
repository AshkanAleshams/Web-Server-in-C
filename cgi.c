#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "cgi.h"

/* Return the number of name=value pairs in a query string. */
int num_pairs(char *str){
    int i;
    int count = 0;
    for(i = 0; i < strlen(str); i++) {
        if(str[i] == '=') {
            count++;
        }
    }
    return count;
}

/* Fill in f with the name=value pair in str. */
void update_fdata(Fdata *f, char *str) {
    char *eq_ptr;
    if((eq_ptr = strchr(str, '=')) == NULL) {
        fprintf(stderr, "Error: badly formatted query string (%s)\n", str);
        exit(1);
    } 
    *eq_ptr = '\0';
    eq_ptr++;

    f->name = malloc(strlen(str) + 1);
    strncpy(f->name, str, strlen(str) + 1);
    f->value = malloc(strlen(eq_ptr) + 1);
    strncpy(f->value, eq_ptr, strlen(eq_ptr) + 1);
}


/* Return an array of name value pairs given a query string.
 * The query string str has the format name1=value1&name2=value2
 *
 * This function alters the str argument.
 */
Fdata *parse_query(char *str) {
    char *amp_ptr;
    
    /* First figure out many key-value pairs we have */
    int count = num_pairs(str);

    Fdata *f = malloc((count + 1) * sizeof(Fdata));
    int i = 0;
    
    while((amp_ptr = strchr(str, '&')) != NULL){
        *amp_ptr = '\0';
        amp_ptr++;

        /* Now str points to a name=value pair */
        update_fdata(&f[i], str);

        i++;
        str = amp_ptr;
    }
    
    // copy the last name value pair
    if(str != NULL) {
        update_fdata(&f[i], str);
        i++;
    }

    
    // make the last element of Fdata point to NULLs so that we have a sentinel
    f[i].name = NULL;
    f[i].value = NULL;
    return f;
}

/* Convert the array of form data name-value pairs into an html list.
 * Return the html string. 
 */
char *fdata2html(Fdata *f) {
    char *html = malloc(MAX_LENGTH);
    strncpy(html, "<ul>\n", MAX_LENGTH);

    for(int i = 0; f[i].name != NULL; i++) {
        strncat(html, "<li>", MAX_LENGTH - strlen(html));
        strncat(html, f[i].name, MAX_LENGTH - strlen(html));    
        strncat(html, " = ", MAX_LENGTH - strlen(html));
        strncat(html, f[i].value, MAX_LENGTH - strlen(html));   
        strncat(html, "</li>\n", MAX_LENGTH - strlen(html));
    }
    strncat(html, "</ul>\n", MAX_LENGTH - strlen(html));
    return html;
}

void fdata_free(Fdata *f) {
    for(int i = 0; f[i].name != NULL; i++) {
        if(f[i].name != NULL){
            free(f[i].name);
        }
        free(f[i].value);
    }
    free(f);
}
