BEGIN { FS="\t" }
$4 == "PT" ; $5 == "V" { verbs[$1]++ }
END { for(v in verbs) { print verbs[v]"\t"v } }
