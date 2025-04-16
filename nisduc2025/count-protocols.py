#!/usr/bin/python3
import argparse
import os
import pprint
import sys

from datetime import datetime

from collections import Counter

parser = argparse.ArgumentParser(description="Read output of tshark and aggregate them per 5 minutes blocks")
parser.add_argument("--filename", type=str, help="Output file created by tshark. The timestamp is derived from the filename")
parser.add_argument("--root", type=str,help="Directory where the aggregated data is put")
args = parser.parse_args()

# Derive date from filename
base = os.path.basename(args.filename.partition('.')[0])
date_items = base.split('-')
date_items = date_items[1:]

# Expected dates and time 2025-02-16-010543
date_str = '-'.join(date_items)
dt = datetime.strptime(date_str, "%Y-%m-%d-%H%M%S")

new_date = dt.strftime("%Y-%m-%d %H:%M")
with open(args.filename, 'r', encoding='utf-8') as file:
        counter = Counter(line.rstrip() for line in file)

# create target directory to store the files
target_dir=args.root+os.sep+str(dt.year)+os.sep+str(dt.month) + os.sep + str(dt.day)
target_file = target_dir +  os.sep + dt.strftime("%Y%m%d%H%M%S") + ".dat"

if (os.path.exists(target_file)):
    print (f"Target file ({target_file}) already exists skip it")
    sys.exit(0)

if (os.path.exists(target_dir) == False):
    os.makedirs(target_dir)

# store the unique lines and their counts
with open(target_file, 'w', encoding='utf-8') as file:
    for line, count in counter.most_common():
        #print(f"{new_date}, {line}, {count}")
        #print(f"{new_date}, {repr(line)}: {count}")
        file.write(new_date+","+line +","+str(count)+"\n")
