#!/bin/gawk -f
BEGIN { FS = "\t" }
$0 !~ /^</ && $4 ~ /^[^¡ª´¹º©«»1-9.,&\/\?;\:\'\"\[\]{}=+\-_\\~\|`!@#$%\^*()].*/ { word = toupper(substr($4, 1, 1)) substr($4, 2); dict[word][$1][$5]++ }
END {
    idx = "dictionary/index.html"
    print "<html>" > idx
    print "<head>" >> idx
    print "  <meta charset='Utf-8'/>" >> idx
    print "</head>" >> idx
    print "<body>" >> idx
    print "  <h1>Dicionario</h1>" >> idx
    for(word in dict) { count++ }
    print "  <h2>"count" palavras</h2>" >> idx
    print "  <table>" >> idx
    print "    <tr>" >> idx
    split("ABCDEFGHIJKLMNOPQRSTUVWXYZÀÁÂÃÄÅÆÇÈÉÊËÌÍÎÑÒÓÔÕÖØÙÚÛÜÝ", chars, "")
    for(c in chars){
        print "    <td><a href='#"chars[c]"'>"chars[c]"</a></td>" >> idx
    }
    print "    </tr>" >> idx
    print "  </table>" >> idx
    print "  <table>" >> idx
    print "    <tr>" >> idx
    i = 0
    last_leter = ""
    PROCINFO["sorted_in"] = "strcmp"
    for(word in dict) {
        word_escaped = word
        gsub("/", " ", word_escaped)
        gsub("=", " ", word_escaped)
        if(last_leter != substr(word_escaped, 1, 1)) {
            last_leter = substr(word_escaped, 1, 1)
            print "    </tr>" >> idx
            print "  </table>" >> idx
            print "  <h3><a name='"toupper(last_leter)"'>"toupper(last_leter)"</a></h3>" >> idx
            print "  <table>" >> idx
            print "    <tr>" >> idx
            i = 0
        }
        if(i % 4 == 0) {
            print "   </tr>" >> idx
            print "   <tr>" > idx
        }
        i++
        print "      <td style='border-right: 1px solid #000;'>" >> idx
        print "        <a href='"word_escaped".html'>"word_escaped"</a>" >> idx
        print "      </td>" >> idx
        # Word file
        word_file = "dictionary/"word_escaped".html"
        print "<html>" > word_file
        print "<head>" >> word_file
        print "  <meta charset='Utf-8'/>" >> word_file
        print "</head>" >> word_file
        print "<body>" >> word_file
        print "  <h1>"word_escaped"</h1>" >> word_file
        print "  <table>" >> word_file
        for(lema in dict[word]) {
            line = "<tr><td>"lema"</td>"
            for(pos in dict[word][lema]){
                line = line "<td>"pos"</td>"
            }
            line = line "</tr>"
            print line >> word_file
        }
        print "</table>" >> word_file
        print "</body>" >> word_file
        print "</html>" >> word_file
    }
    print "</tr>" >> idx
    print "</body>" >> idx
    print "</html>" >> idx
}
function strcmp(i1, v1, i2, v2) {
    i1 = i1 ""
    i2 = i2 ""
    if(i1 < i2)
        return -1
    return i1 != i2
}
