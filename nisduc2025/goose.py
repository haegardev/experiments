#!/usr/bin/python3
import argparse
import json

import pprint

parser = argparse.ArgumentParser(description="Read output of tshark and extract relevant fields")
parser.add_argument("--filename", type=str, help="Output file created by tshark. The timestamp is derived from the filename")
parser.add_argument("--root", type=str,help="Directory where the aggregated data is put")
args = parser.parse_args()

fp = open(args.filename,"r")
doc = json.load(fp)

for packet in doc:
    ts = 0
    ip_src = "na"
    src_port = "na"
    ip_dst="na"
    dst_port = 0
    goose="na"
    try:
        ts=packet["_source"]["layers"]["frame"]["frame.time_epoch"]
        ip_src=packet["_source"]["layers"]["ip"]["ip.src"]
        src_port = packet["_source"]["layers"]["udp"]["udp.srcport"]
        dst_port = packet["_source"]["layers"]["udp"]["udp.dstport"]
        ip_dest = packet["_source"]["layers"]["ip"]["ip.dst"]
        goose = packet["_source"]["layers"]["goose"]
        print (ts,ip_src,src_port, ip_dest, dst_port, goose)
    except KeyError as w:
        pass






