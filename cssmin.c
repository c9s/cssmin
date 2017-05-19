#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <ext/standard/php_smart_string.h>
#include "cssmin.h"

#define STATE_FREE 1
#define STATE_ATRULE 2
#define STATE_SELECTOR 3
#define STATE_BLOCK 4
#define STATE_DECLARATION 5
#define STATE_PROPERTY_VALUE 6
#define STATE_COMMENT 7

#define OPTION_CSSMIN_SELECTOR 0

static int cssmin_peek(cssmin_parser *parser);
static int cssmin_peek2(cssmin_parser *parser);
static int cssmin_get(cssmin_parser *parser);
static void cssmin_advance(cssmin_parser *parser);
static void cssmin_strip_spaces(cssmin_parser *parser);
static int is_simple_selector(char c);
static int option_enabled(cssmin_parser *parser, int pos);

static int option_enabled(cssmin_parser *parser, int pos)
{
    return parser->options >> pos & 1;
}

static void cssmin_advance(cssmin_parser *parser)
{
    parser->theLookahead = EOF;
    if (parser->pos < parser->source_len) {
        parser->pos++;
    }
}

static void cssmin_strip_spaces(cssmin_parser *parser) {
    char c;
    while(1) {
        c = cssmin_peek(parser);
        if (!isspace(c)) {
            break;
        }
        cssmin_advance(parser);
    }
}

static void cssmin_strip_comment(cssmin_parser *parser) {

    if (cssmin_peek(parser) == '/' && cssmin_peek2(parser) == '*') {
        parser->previous_state = parser->state;
        cssmin_advance(parser);
        cssmin_advance(parser);
        char ch1;
        char ch2;
        while(1) {
            ch1 = cssmin_get(parser);
            ch2 = cssmin_peek(parser);
            if (ch1 == '*' && ch2 == '/') {
                cssmin_advance(parser);
                break;
            }
            if (ch1 == EOF) {
                break;
            }
        }
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

static int cssmin_next_non_space_char(cssmin_parser *parser)
{
    int p = parser->pos;
    char c;
    while(p < parser->source_len) {
        c = parser->source[p];
        if (!isspace(c)) {
            return c;
        }
        p++;
    }
    return EOF;
}

static int cssmin_peek2(cssmin_parser *parser)
{
    if (parser->pos + 1 < parser->source_len) {
        return parser->source[parser->pos + 1];
    }
    return EOF;
}

static int cssmin_peek(cssmin_parser *parser)
{
    if (parser->pos < parser->source_len) {
        return parser->source[parser->pos];
    }
    return EOF;
}

// http://www.w3.org/TR/CSS21/grammar.html#grammar
static int is_simple_selector(char c)
{
    // element_name (nmstart)
    if (isalpha(c)) {
        return 1;
    }

    switch (c){
        case '#': // HASH (id)
        case '.': // class
        case ':': // pseudo
        case '[': // attrib
        case '*': // element_name
        case '-': // element_name
        case '_': // element_name (nmstart)
          return 1;
    }

    return 0;
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
                    } else if (option_enabled(parser, OPTION_CSSMIN_SELECTOR)) {
                        if (c == ' ' && !is_simple_selector(cssmin_next_non_space_char(parser))) {
                            c = 0;
                        } else if (!isalpha(c) && cssmin_peek(parser) == ' ') {
                            cssmin_strip_spaces(parser);
                        }
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
                break;
            } else {
                parser->state = STATE_DECLARATION;
            }
        case STATE_PROPERTY_VALUE:
            if (c == ' ' && cssmin_peek(parser) == ' ') {
                return 0;
            } else if (c == ' ' && cssmin_peek(parser) == ';') {
                return 0;
            } else if ((c == ';' || c == ' ') && cssmin_next_non_space_char(parser) == '}' ) {
                // ignore ";" of the last statment in the block
                parser->state = STATE_BLOCK;
                return 0;
            } else if (c == ';' && cssmin_peek(parser) == '}') {
                // ignore ";" of the last statment in the block
                parser->state = STATE_BLOCK;
                return 0;
            } else if (c == ';') {
                parser->state = STATE_BLOCK;
                return c;
            }
            return c;
            break;
        case STATE_DECLARATION:
            //support in paren because data can uris have ;
            if(c == '('){
                parser->in_paren = 1;
            }
            if(parser->in_paren == 0){
                if (c == ';') {
                    parser->state = STATE_BLOCK;
                    //could continue peeking through white space..
                    if (cssmin_peek(parser) == '}'){
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
                } else if (c == ':') {
                    // strip the white space after ":"
                    parser->state = STATE_PROPERTY_VALUE;
                    cssmin_strip_spaces(parser);
                    return c;
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
            smart_string_appendc(parser->minified, c);
        }
    }
}
