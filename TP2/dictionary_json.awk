#!/usr/bin/gawk -f
BEGIN { FS = "\t" }
$0 !~ /^</ && $4 !~ /[0-9¡ª´¹º©«».,&?;:'"{}=_~|`!@#$%^*()+\-\/\[\]\\]/ {
    word = toupper(substr($4, 1, 1)) substr($4, 2)
    dict[word][$1][$5]++
}
END {
    print "{"
    print "\t\"dictionary\": {"
    print "\t\t\"words\": [";
    for(w in dict) { n_words++ }
    for(word in dict) {
        print "\t\t\t{"
        word_escaped = word
        gsub("\"", "\\\"", word_escaped)
        print "\t\t\t\t\"word\": \""word_escaped"\","
        print "\t\t\t\t\"lemas\": ["
        for(l in dict[word]) { n_lemas++ }
        for(lema in dict[word]) {
            lema_escaped = lema
            gsub("\"", "\\\"", lema_escaped)
            print "\t\t\t\t\t\t{\""lema_escaped"\": ["
            for(p in dict[word][lema]) { n_pos++ }
            for(pos in dict[word][lema]) {
                if(n_pos == 1) {
                    print "\t\t\t\t\t\t\t\""pos"\""
                } else {
                    print "\t\t\t\t\t\t\t\""pos"\","
                }
                n_pos--
            }
            if(n_lemas == 1){
                print "\t\t\t\t\t\t]}"
            } else {
                print "\t\t\t\t\t]},"
            }
            n_lemas--
        }
        print "\t\t\t\t]"
        if(n_words == 1) {
            print "\t\t\t}"
        } else {
            print "\t\t\t},"
        }
        n_words--
    }
    print "\t\t]"
    print "\t}"
    print "}"
}
