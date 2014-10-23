#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <ext/standard/php_smart_str.h>
#include "cssmin.h"

#define STATE_FREE 1
#define STATE_ATRULE 2
#define STATE_SELECTOR 3
#define STATE_BLOCK 4
#define STATE_DECLARATION 5
#define STATE_COMMENT 6

static int cssmin_peek(cssmin_parser *parser);
static int cssmin_get(cssmin_parser *parser);
static void cssmin_advance(cssmin_parser *parser);
static void cssmin_strip_spaces(cssmin_parser *parser);

static void cssmin_advance(cssmin_parser *parser)
{
    parser->theLookahead = EOF;
    if (parser->pos < parser->source_len) {
        parser->pos++;
    }
}

static void cssmin_strip_spaces(cssmin_parser *parser) {
    while(true) {
        if (!isspace(cssmin_peek(parser))) {
            break;
        }
        cssmin_advance(parser);
    }
}

static int cssmin_get(cssmin_parser *parser)
{
    int c = parser->theLookahead;
    parser->theLookahead = EOF;
    if (c == EOF) {
        if (parser->pos < parser->source_len) {
            c = parser->source[parser->pos++];
        } else {
            c = EOF;
        }
    }
    if (c >= ' ' || c == '\n' || c == EOF) {
        return c;
    }
    if (c == '\r') {
        return '\n';
    }
    return ' ';
}

static int cssmin_peek(cssmin_parser *parser)
{
    if (parser->pos < parser->source_len) {
        return parser->source[parser->pos];
    }
    return EOF;
}

static int cssmin_machine(cssmin_parser *parser, int c)
{
    if (parser->state != STATE_COMMENT) {
        if(c == '/' && cssmin_peek(parser) == '*'){
            parser->previous_state = parser->state;
            parser->state = STATE_COMMENT;
        }
    }
    
    switch (parser->state){
        case STATE_FREE:
            if (c == ' ' && c == '\n' ) {
                c = 0;
            } else if (c == '@'){
                parser->state = STATE_ATRULE;
                break;
            } else if(c > 0){
                parser->state = STATE_SELECTOR;
            }
        case STATE_SELECTOR:
            if (c == '{') {
                parser->state = STATE_BLOCK;
            } else {
                if (c == '\n' || c == '\r') {
                    c = 0;
                } else {
                    if (c == '@'){
                        parser->state = STATE_ATRULE;
                    } else if (c == ' ' && cssmin_peek(parser) == '{') {
                        c = 0;
                    }
                }
            }
            break;
        case STATE_ATRULE:
            /* support 
                @import etc.
                @font-face{
            */
            if (c == '\n' || c == ';') {
                c = ';';
                parser->state = STATE_FREE;
            } else if(c == '{') {
                parser->state = STATE_BLOCK;
            }
            break;
        case STATE_BLOCK:
            if (c == ' ' || c == '\n' ) {
                c = 0;
                break;
            } else if (c == '}') {
                parser->state = STATE_FREE;
                cssmin_strip_spaces(parser);
                //fprintf(stdout,"closing bracket found in block\n");
                break;
            } else {
                parser->state = STATE_DECLARATION;
            }
        case STATE_DECLARATION:
            //support in paren because data can uris have ;
            if(c == '('){
                parser->in_paren = 1;
            }
            if(parser->in_paren == 0){
                
                if( c == ';') {
                    parser->state = STATE_BLOCK;
                    //could continue peeking through white space..
                    if(cssmin_peek(parser) == '}'){
                        c = 0;
                    }
                } else if (c == '}') {
                    //handle unterminated declaration
                    parser->state = STATE_FREE;
                } else if (c == '\n' || c == '\r') {
                  // skip new lines
                  c = 0;
                } else if (c == ' ') {
                  // skip multiple spaces after each other
                  if (cssmin_peek(parser) == ' ') {
                      c = 0;
                  }
                }
                
            } else if (c == ')') {
                parser->in_paren = 0;
            }

            break;
        case STATE_COMMENT:
            if (c == '*' && cssmin_peek(parser) == '/') {
                parser->theLookahead = EOF;
                parser->state = parser->previous_state;
                cssmin_advance(parser);
            }

            // Strip the space after the comment
            cssmin_strip_spaces(parser);
            c = 0;
            break;
    }
    return c;
}

void cssmin(cssmin_parser *parser)
{
    for (;;) {
        int c = cssmin_get(parser);
        if (c == EOF) {
            return;
        }
        c = cssmin_machine(parser, c);
        if (c != 0) {
            smart_str_appendc(parser->minified, c);
        }
    }
}
