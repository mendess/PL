#!/bin/awk -f
BEGIN { FS="\t" }
$1 == "<s>" { s++ }
$1 ~ /<p[^>]*>/ { p++ }
$1 ~ /<ext[^>]*>/ { e++ }
$1 ~ /<mwe[^>]*>/ { mwe++ }
END { print "#Extratos:\t"s"\n#Paragrafos:\t"p"\n#Frases:\t"e"\n#MWExpressions:\t"mwe }
