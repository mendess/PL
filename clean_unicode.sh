#!/bin/bash

if [ "$#" -lt 1 ]
then
    echo "Usage: $0 [file,...]"
    exit 1
fi

for f in "$@"
do
    sed 's/━/#/g' "$f" > "${f%.*}".clean.txt
done
