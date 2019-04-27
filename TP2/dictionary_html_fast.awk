#!/bin/gawk -f
BEGIN { FS = "\t" }
$0 !~ /^</ && $4 !~ /[¡ª´¹º©«»0-9.,&\/\?;\:\'\"\[\]{}=+\-_\\~\|`!@#$%\^*()]/ {
    word = toupper(substr($4, 1, 1)) substr($4, 2); dict[word][$1][$5]++
}
END {
    system("mkfifo pipe")
    system("./dictionary_html_fast pipe &")
    for(word in dict) {
        for(lema in dict[word]) {
            for(pos in dict[word][lema]) {
                print word"\t"lema"\t"pos"" >> "pipe"
            }
        }
    }
    system("rm pipe")
}
