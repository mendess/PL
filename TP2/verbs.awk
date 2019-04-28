#!/usr/bin/gawk -f
BEGIN { FS="\t" }
$5 ~ /^V/ { verbs[tolower($4)]++ }
END { for(v in verbs) { print verbs[v]"\t"v } }
