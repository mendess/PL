#!/bin/awk -f
BEGIN { FS="\t" }
$4 == "PT" ; $5 == "V" { verbs[tolower($1)]++ }
END { for(v in verbs) { print verbs[v]"\t"v } }
