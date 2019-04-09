    grep 'tags/regime.html' noticias/tags.html \
        | awk -F'>' '{print $3}' \
        | grep -oP '[0-9]+'
