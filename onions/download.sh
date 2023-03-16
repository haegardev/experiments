#!/bin/bash
#Download sequentialy common crawl data and extratc onions


if [ ! -e "./tmp" ]; then
    mdkir ./tmp
fi
#Remove temp downloads
rm ./tmp/*
if [ ! -e "./results" ]; then
    mkdir ./results
fi

if [ ! -e warc.paths.gz ]; then
    wget https://data.commoncrawl.org/crawl-data/CC-MAIN-2023-06/warc.paths.gz
fi

for i in `zcat warc.paths.gz`; do
    f=`basename $i`
    URL="https://data.commoncrawl.org/$i"
    wget $URL -O "tmp/$f"
done


