--TEST--
basic
--FILE--
<?php
echo cssmin('.rule { }');
--EXPECT--
.rule{}
