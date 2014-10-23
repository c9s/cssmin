<?php
function is($exp, $input, $msg = NULL) {
    echo $msg ?: "'" . substr($exp, 0, 32) . "'";
    echo " - ";
    if ($exp === $input) {
        echo "ok\n";
    } else {
        echo "failed\n";
        echo "\tExpected: '$exp'\n";
        echo "\tActual: '$input'\n";
    }
}

is(".rule{}", cssmin(".rule {  }"));
is(".rule{}", cssmin(".rule { } /* commment */    "));
is(".parent .child{-webkit-transform: translateX(200px);}", cssmin(".parent .child { \n-webkit-transform: translateX(200px); \n}"));
is(".rule{background: #fff;}", cssmin(".rule { 

    background:      #fff;

     }"));
