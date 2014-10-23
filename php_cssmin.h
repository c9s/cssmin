#ifndef PHP_CSSMIN_H
#define PHP_CSSMIN_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#include "zend_API.h"
#include "zend_variables.h"
#include "ext/standard/head.h"
#include "php_globals.h"
#include "php_main.h"
#include "ext/standard/php_string.h"
#include "ext/standard/php_smart_str.h"
#include "ext/standard/php_string.h"
#include "php_globals.h"

#define PHP_CSSMIN_VERSION "1.0.0"
#define PHP_CSSMIN_EXTNAME "cssmin"


PHP_MINIT_FUNCTION(cssmin);
PHP_MSHUTDOWN_FUNCTION(cssmin);
PHP_MINFO_FUNCTION(cssmin);

extern zend_module_entry cssmin_module_entry;

PHP_FUNCTION(cssmin);

#endif
