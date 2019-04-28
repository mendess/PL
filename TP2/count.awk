#!/usr/bin/gawk -f
BEGIN { FS="\t" }
$1 == "<s>" { s++ }
$1 ~ /<p[^>]*>/ { p++ }
$1 ~ /<ext[^>]*>/ { e++ }
$1 ~ /<mwe[^>]*>/ { mwe++ }
END {
    print "#Extracts:\t"s
    print "#Paragraphs:\t"p
    print "#Sentences:\t"e
    print "#MWExpressions:\t"mwe
}
