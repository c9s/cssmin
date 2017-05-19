#include "php.h"
#include "ext/standard/php_smart_string.h"
#include "ext/standard/info.h"
#include "php_ini.h"
#include "cssmin.h"
#include "php_cssmin.h"

static const zend_function_entry cssmin_functions[] = {
    PHP_FE(cssmin, NULL)
    PHP_FE_END
};


zend_module_entry cssmin_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_CSSMIN_EXTNAME,
    cssmin_functions,
    PHP_MINIT(cssmin),
    PHP_MSHUTDOWN(cssmin),
    NULL,
    NULL,
    NULL,
    PHP_CSSMIN_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_CSSMIN
ZEND_GET_MODULE(cssmin)
#endif

PHP_FUNCTION(cssmin) {
    char *css;
    size_t css_len;
    long  options = 0;
    /* parse parameters */
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l", &css, &css_len, &options) == FAILURE) {
        RETURN_FALSE;
    }
    cssmin_parser parser;

    parser.source = css;
    parser.source_len = css_len;
    parser.theLookahead = EOF;
    parser.state = 1;
    parser.pos = 0;
    parser.in_paren = 0;
    parser.options = options;

    smart_string minified = {0};
    parser.minified = &minified;
    cssmin(&parser);

    smart_string_0(&minified);

    RETVAL_STRINGL(minified.c, minified.len);
    smart_string_free(&minified);
}

PHP_MINIT_FUNCTION(cssmin) {
    REGISTER_LONG_CONSTANT("CSSMIN_SELECTOR", CSSMIN_SELECTOR, CONST_CS | CONST_PERSISTENT);
    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(cssmin) {
  UNREGISTER_INI_ENTRIES();
  return SUCCESS;
}

PHP_RINIT_FUNCTION(cssmin) {
  return SUCCESS;
}

