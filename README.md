cssmin
=============================================
A simple fast css minification extension

[![Build Status](https://travis-ci.org/c9s/cssmin.svg?branch=master)](https://travis-ci.org/c9s/cssmin)

the master branch is for php7.

See `php5` branch for the php5 extension.



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

TESTS
------
```
'.rule{}'                                                    - ok
'.rule{padding:1px 2px 3px 4px}'                             - ok
'.rule{padding:1px 2px 3px 4px}'                             - ok
'.rule{padding:1px 2px 3px 4px}'                             - ok
'.super > .parent > .child{}'                                - ok
'.rule{}'                                                    - ok
'.parent .child{-webkit-transform:translateX(200px)}'        - ok
'.rule{color:#000;background:#fff}'                          - ok
'.rule1{background:#fff}.rule2{filter:Alpha(Opacity=60)}'    - ok
```
