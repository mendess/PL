#!/bin/bash

echo "Implementation name?"
read -r implementation
impl_no_space=${implementation// /_}

if echo "$impl_no_space" | grep ','
then
    echo "Can't have ',' in implementation name"
    exit 1
fi

### MEMORY USAGE

DATA_FILE=benchmarks/"$impl_no_space".dat

memusage \
    --total \
    --data="$DATA_FILE" \
    target/release/mkhtml \
    < input/folha8.OUT.txt \
    2> /tmp/memusage.table

memusagestat \
    --total \
    --output=benchmarks/"$impl_no_space".png \
    --string="$implementation" \
    "$DATA_FILE"

### PERFORMANCE

hyperfine \
    --warmup 5 \
    --time-unit second \
    --export-csv /tmp/bench.csv \
    'target/release/mkhtml < input/folha8.OUT.txt'

touch benchmarks/times.csv

CSV_HEADER='mean,stddev,user,system,min,max'
CSV_HEADER_FULL='implementation,mean,stddev,user,system,min,max,heap total,heap peak,stack peak'
echo "$CSV_HEADER_FULL" > /tmp/times.csv

BENCH=$(sed "/$CSV_HEADER/d" /tmp/bench.csv \
    | sed -E "s|[^,]+|$implementation|" \
    | sed -E 's:([0-9]*\.[0-9]{4})[^,]*:\1:g')

BENCH="$BENCH"','$(grep 'Memory usage summary' /tmp/memusage.table \
    | awk -F ':' '{ print $3 $4 $5 }' \
    | sed -E 's/[^0-9,]//g')

echo "$BENCH" \
    | cat benchmarks/times.csv - \
    | sed "/$CSV_HEADER/d" \
    >> /tmp/times.csv

mv /tmp/times.csv benchmarks/times.csv

