#include <stdlib.h>
#include <stdio.h>
#include <ext/standard/php_smart_str.h>

#define CSSMIN_SELECTOR 1

typedef struct _cssmin_parser { 
    int theLookahead;
    int previous_state;
    int state;
    int in_paren;
    char * source;
    int pos;
    int source_len;
    int options;
    smart_str *minified;
} cssmin_parser;

void cssmin();
