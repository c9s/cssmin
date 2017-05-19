--TEST--
basic 02
--FILE--
<?php
echo cssmin(".rule { padding: 1px 2px 3px 4px;}");
--EXPECT--
.rule{padding:1px 2px 3px 4px}
