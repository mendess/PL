    tr '\n' ' ' < input/folha8.OUT.txt \
        |  grep -oP 'tag\:\{[^}]+' \
        | sed -E 's|tag\:\{([^}]+)|\1|' \
        | sort \
        | uniq \
        | wc -l
