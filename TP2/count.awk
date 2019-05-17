#!/usr/bin/gawk -f
BEGIN { FS="\t" }
$1 ~ /<s[^>]*>/ { s++ }
$1 ~ /<p[^>]*>/ { p++ }
$1 ~ /<ext[^>]*>/ { e++ }
$1 ~ /<mwe[^>]*>/ { mwe++ }
END {
    print "#Extracts:\t"e
    print "#Paragraphs:\t"p
    print "#Sentences:\t"s
    print "#MWExpressions:\t"mwe
}
