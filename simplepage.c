
/* Return a string constant containing a simple web page.  
 * Useful for testing early on, but will not be part of the final
 * assignment submission. */
char * simple_page() {
    char *str =	 "Content-type: text/html\r\n\r\n"
        "<html><head><title>Hello World</title></head>\n"
        "<body>\n<h2>Hello, world!</h2>\n"
        "</body></html>\n");
    return str;
}