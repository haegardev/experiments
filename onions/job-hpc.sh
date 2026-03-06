#!/bin/bash
# Take a URI from a of the WARC paths and downloads and process
set -e
BASEURL="https://data.commoncrawl.org"
URI=$1
NODE=$2
URL="$BASEURL/$URI"
tmpfile=$(mktemp)
trap 'rm -f "$tmpfile"' EXIT

echo "[TRACE][NODE=$NODE,PID=$$][`date`] Downloading file $URL" >&2
wget --quiet $URL -O $tmpfile
echo "[TRACE][NODE=$NODE,PID=$$][`date`] Wget exit code $E"
echo "[TRACE][NODE=$NODE,PID=$$][`date`] Processing file $URL" >&2
zcat $tmpfile | grep -a onion | tr ' "><' '\n' | grep "\.onion$"
echo "[TRACE][NODE=$NODE,PID=$$][`date`] Job for $URL done"
