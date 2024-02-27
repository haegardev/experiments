#!/bin/bash
FILENAME=$1
tshark -n -r $FILENAME -E occurrence=f -E separator=, -T fields -e frame.time_epoch -e ip.src -e ip.dst -e ip.proto 
