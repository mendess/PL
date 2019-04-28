#!/bin/gawk -f
BEGIN {
    FS = "\t"
    system("mkfifo pipe")
    system("./dictionary_html_fast pipe &")
}
$0 !~ /^</ && $4 !~ /[0-9¡ª´¹º©«».,&?;:'"{}=_~|`!@#$%^*()+\-\/\[\]\\]/ {
    word = toupper(substr($4, 1, 1)) substr($4, 2)
    print word"\t"$1"\t"$5 >> "pipe"
}
END { system("rm pipe") }
