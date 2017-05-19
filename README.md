cssmin
=============================================
A simple fast css minification extension

[![Build Status](https://travis-ci.org/c9s/cssmin.svg?branch=master)](https://travis-ci.org/c9s/cssmin)

the master branch is for php7.

See the `php5` branch for the php5 extension.


INSTALL
---------

```
phpize && ./configure && make install
```

USAGE
-------

There is only one function to minify the css:

```php
<?php
var_dump(cssmin(".rule {  }"));
```

LICENSE
-------

MIT License
