#!/bin/bash
#Download sequentialy common crawl data in parallel process it and remove the
#files. To not fill up the disks  the number of warcs can be fixed
CRAWL_NUM="CC-MAIN-2023-06"
MAX_FILES=100
PROCESSED_URLS="processed_urls.txt"
if [ ! -e "./tmp" ]; then
    mdkir ./tmp
fi

touch $PROCESSED_URLS

if [ ! -e "./results" ]; then
    mkdir ./results
fi

if [ ! -e warc.paths.gz ]; then
    wget https://data.commoncrawl.org/crawl-data/$CRAWL_NUM/warc.paths.gz
fi

#Check maximum number of files in download folder

N=$(ls tmp/ -1 | wc -l | tr -d '\n')

if [ $N -gt $MAX_FILES ]; then
    echo "[INFO] found $N files. Max files = $MAX_FILES going to exit"
    exit 0
fi

for i in `zcat warc.paths.gz`; do
    if [ ! -z "$(grep $i $PROCESSED_URLS)" ]; then
        echo "[INFO] file $i was already processed"
        continue
    fi
    f=`basename $i`
    if [ -e "./tmp/$f" ]; then
        echo "[INFO] file $i was already downloaded"
        continue
    fi
    URL="https://data.commoncrawl.org/$i"
    nice -n 19 ionice -c 3 wget $URL -O "./tmp/.$f"
    mv "./tmp/.$f" "tmp/$f"
done


