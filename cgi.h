#define MAX_LENGTH 1024

typedef struct formdata {
    char *name;
    char *value;
} Fdata;

Fdata *parse_query(char *str);
char *fdata2html(Fdata *f);
void fdata_free(Fdata *f);
