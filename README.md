cssmin
===========

INSTALL
---------

```
$ phpize
$ ./configure
$ make install
```

USAGE
-------

There is only one function to minify the css:

```php
<?php
var_dump(cssmin(".rule {  }"));
```

