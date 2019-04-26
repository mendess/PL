#!/bin/gawk -f
BEGIN { FS = "\t" }
$0 !~ /^</ { dict[$4][$1][$5]++ }
END {
    idx = "dictionary/index.html"
    print "<html>" > idx
    print "<head>" >> idx
    print "  <meta charset='Utf-8'/>" >> idx
    print "</head>" >> idx
    print "<body>" >> idx
    print "  <h1>Dicionario</h1>" >> idx
    print "  <table>" >> idx
    for(word in dict) {
        word_escaped = word
        gsub("/", "+", word_escaped)
        print "    <tr><th style='border-right: 1px solid #000;'><a href='"word_escaped".html'>"word_escaped"</a></th>" >> idx
        for(lema in dict[word]) {
            word_file = "dictionary/"word_escaped".html"
            print "<html>" > word_file
            print "<head>" >> word_file
            print "  <meta charset='Utf-8'/>" >> word_file
            print "</head>" >> word_file
            print "<body>" >> word_file
            print "  <h1>Dicionario</h1>" >> word_file
            print "  <table>" >> word_file
            line = "<tr><td>"lema"</td>"
            for(pos in dict[word][lema]){
                line += "<td>"pos"</td>"
            }
            line += "</tr>"
            print line >> word_file
            print "</table>" >> word_file
            print "</body>" >> word_file
            print "</html>" >> word_file
        }
    }
    print "</body>" >> idx
    print "</html>" >> idx
}
