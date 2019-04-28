#!/usr/bin/gawk -f
BEGIN { FS="\t"; mwe = 0 }
$1 ~ /<mwe[^>]*>/              { mwe = 1; current = "" }
$1 ~ /<\/mwe[^>]*>/            { mwe = 0; expressions[current]++ }
mwe == 1 && $1 !~ /<mwe[^>]*>/ { current = current " " $1 }
END { for(e in expressions) { print expressions[e] "\t" e } }
