#!/bin/bash
#Download sequentialy common crawl data in parallel process it and remove the
#files. To not fill up the disks  the number of warcs can be fixed
CRAWL_NUM="CC-MAIN-2023-06"
MAX_FILES=100

if [ ! -e "./tmp" ]; then
    mdkir ./tmp
fi
#Remove temp downloads
rm ./tmp/*
if [ ! -e "./results" ]; then
    mkdir ./results
fi

if [ ! -e warc.paths.gz ]; then
    wget https://data.commoncrawl.org/crawl-data/$CRAWL_NUM/warc.paths.gz
fi

for i in `zcat warc.paths.gz`; do
    f=`basename $i`
    URL="https://data.commoncrawl.org/$i"
    wget $URL -O "./tmp/.$f"
    mv "./tmp/.$f" "tmp/$f"
done


