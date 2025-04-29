#!/usr/bin/python3
import argparse
import json
from  datetime import datetime
import pprint
import os

parser = argparse.ArgumentParser(description="Read the parallized output of tetra.py and filter out scanners")
parser.add_argument("--filename", type=str, help="Output file created by tetra.py")
parser.add_argument("--export", type=str, help="Directory where the grouped packets show be exported")
args = parser.parse_args()
f = open(args.filename, "r")
data = dict()
for line in f.readlines():
    line=line[:-1]
    t = line.split(" ")
    #1735965874.276608000 112.23.77.56 7074 193.168.114.55 38088 {'tetra.carrier': '1', 'tetra.header': {'tetra.timer': '0x4c00'}}
    try:
        ts = t[0]
        ip_src = t[1]
        src_ort = t[2]
        ip_dest = t[3]
        dest_port = t[4]
        tetra = t[5:]
        if ip_src not in data:
            data[ip_src] = dict()
        if ip_dest not in data[ip_src]:
            data[ip_src][ip_dest]=dict()
        if ts not in data[ip_src][ip_dest]:
            data[ip_src][ip_dest][ts] = []
        data[ip_src][ip_dest][ts].append(line)
    except IndexError as e:
        print (f"Error cannot parse {line}")


# print only the IP addresses that connected to less than t destination ip addresses

for ip_src in data:
    if len(data[ip_src].keys()) < 2:
        out =  []
        for target_ip in data[ip_src]:
            # Merge all entries according timestamp as they were previously grouped by target_ip
            for ts in data[ip_src][target_ip]:
                for line in data[ip_src][target_ip][ts]:
                    out.append(line)
        # sort according timestamp
        out.sort()
        g = None
        if args.export:
            targetfile=args.export + os.sep + ip_src + ".txt"
            g = open(targetfile,"w")
            print (targetfile)
        for  line in out:
            t = line.split()
            ts0 = t[0]
            rest = t[1:]
            (ts,su) = ts0.split(".")
            dt = datetime.utcfromtimestamp(int(ts))
            tsf =  dt.strftime("%Y-%m-%d %H:%M:%S")
            new_line = tsf + " "  + " ".join(rest)
            if g is not None:
                g.write(new_line+"\n")
            else:
                print (new_line)
        if g is not None:
            g.close()
