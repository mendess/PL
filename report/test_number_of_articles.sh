
    grep -Po 'post-[0-9]+' input/folha8.OUT.txt \
        | sort \
        | uniq -c \
        | sort -n -r \
        | cut -d' ' -f7 \
        | uniq -c
