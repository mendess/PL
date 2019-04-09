#!/bin/bash

cd "$(dirname "$0")"/.. || exit 1

mkdir -p input
cd input || exit 0

rm -f ./*.txt
curl http://natura.di.uminho.pt/\~jj/pl-18/TP1/CORPORA1/ \
    | grep td \
    | awk -F'=' '{print $5}' \
    | awk -F'"' '{print $2}' \
    | tail -3 \
    | sed 's|^|http://natura.di.uminho.pt/\~jj/pl-18/TP1/CORPORA1/|' \
    | xargs wget

xz -d ./*.xz
chmod -w ./*
