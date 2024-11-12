#!/bin/bash
FILENAME=$1

jq -r '.[].src_ip' $FILENAME | sort -u
