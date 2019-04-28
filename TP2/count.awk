#!/bin/awk -f
BEGIN { FS="\t" }
$1 == "<s>" { s++ }
$1 ~ /<p[^>]*>/ { p++ }
$1 ~ /<ext[^>]*>/ { e++ }
$1 ~ /<mwe[^>]*>/ { mwe++ }
END {
    print "#Extratos:\t"s
    print "#Paragrafos:\t"p
    print "#Frases:\t"e
    print "#MWExpressions:\t"mwe
}
