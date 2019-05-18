#!/usr/bin/gawk -f
BEGIN { FS="\t"; title_mode = 0; }
$1 ~ /^<ext[^>]*>/    { title = ""; text = ""; }
$1 ~ /^<p[^>]*>/      { text = text"\n\n" }
$1 ~ /^<t[^>]*>/      { title_mode = 1; }
$1 ~ /^<\/t[^>]*>/    { title_mode = 0; }
$1 !~ /^<[^>]*>/      {
    text_ecaped = $1;
    gsub("\"", "``", text_ecaped)
    if(title_mode == 1) {
        title = title " " text_ecaped
    } else {
        text = text " " text_ecaped;
    }
}
$1 ~ /<\/ext[^>]*>/  { print title " " "\""text"\"" }
END {}
