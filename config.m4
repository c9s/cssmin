dnl $Id$

PHP_ARG_ENABLE(cssmin, whether to enable cssmin support,
    [  --disable-cssmin           Enable cssmin support], yes)

if test "$PHP_CSSMIN" != "no"; then
    PHP_NEW_EXTENSION(cssmin,php_cssmin.c cssmin.c, $ext_shared)
    PHP_SUBST(CSSMIN_SHARED_LIBADD)
fi

