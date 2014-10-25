<?php
function desc($exp) {
    return ((strlen($exp) > 58) ? (substr($exp, 0, 58) . '...') : $exp);
}

function is($exp, $input, $msg = NULL) {
    if (!$msg) {
        $msg = "'" 
            . desc($exp)
            . "'";
    }
    $result = $exp === $input;
    printf("% -60s - %s\n", $msg, $result ? "ok" : "failed");
    if (!$result) {
        echo "\tExpected: '$exp'\n";
        echo "\tActual: '$input'\n";
    }
}

is(".rule{}", cssmin(".rule {  }"));
is(".rule{padding:1px 2px 3px 4px}", cssmin(".rule { padding: 1px 2px 3px 4px;}"));
is(".rule{padding:1px 2px 3px 4px}", cssmin(".rule { padding:   1px 2px 3px 4px; }"));
is(".rule{padding:1px 2px 3px 4px}", cssmin(".rule { padding:   1px   2px   3px   4px; }"));
is(".rule{padding:1px 2px 3px 4px}", cssmin(".rule { padding:   1px   2px   3px   4px   }"));


is(".rule{}", cssmin(".rule { } /* commment */    "));
is(".parent .child{-webkit-transform:translateX(200px)}", cssmin(".parent .child { \n-webkit-transform: translateX(200px); \n}"));
is(".rule{color:#000;background:#fff}", cssmin(".rule { 

    color:            #000  ;
    background:      #fff   ;

     }"));


is(".rule1{background:#fff}.rule2{filter:Alpha(Opacity=60)}", cssmin("
.rule1 { 
    background:      #fff;
}

.rule2 {
    filter: Alpha(Opacity=60);
}
"));


echo "\nExperimental\n";

echo "\nSelectors\n";
is(".super>.parent>.child{}", cssmin(".super > .parent > .child { \n }", CSSMIN_SELECTOR));
is(".parent>[src]{}", cssmin(".parent > [src] { \n }", CSSMIN_SELECTOR));
is(".parent>.child:not(.compound){}", cssmin(".parent > .child:not( .compound ) { \n }", CSSMIN_SELECTOR));
// An+B
// http://www.w3.org/TR/selectors4/#anb-grammar
is(".parent>a:nth-child(2n+1){}", cssmin(".parent > a:nth-child(2n \n +\t 1 ) { \n }", CSSMIN_SELECTOR));
is(".parent>a:nth-child(+1){}", cssmin(".parent > a:nth-child(  + 1 ) { \n }", CSSMIN_SELECTOR));
is(".parent>a:nth-child(1){}", cssmin(".parent > a:nth-child(1 ) { \n }", CSSMIN_SELECTOR));
