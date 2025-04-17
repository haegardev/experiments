#!/usr/bin/python3
import argparse
import os
import pprint
import sys
import traceback
from datetime import datetime
from pathlib import Path

parser = argparse.ArgumentParser(description="Read files created by count-protocols.py and aggregate them")
parser.add_argument("--directory", type=str, help="Directory containing the files created by count-protocols.py")
parser.add_argument("--root", type=str,help="Directory where the aggregated data is put")
args = parser.parse_args()

directory = Path(args.directory)
files = [f for f in Path(directory).iterdir() if f.is_file()]
files.sort()

data = dict()
first = None
for file in files:
    with open(file, 'r', encoding='utf-8') as fd:
        for line in fd:
            line = line.rstrip()
            try:
                (date, prot, freq) = line.split(',')
                dt = datetime.strptime(date, "%Y-%m-%d %H:%M")
                day = dt.strftime("%Y-%m-%d")
                # Check if all items belong to the same day
                if first is None:
                    first = day
                else:
                    if first != day:
                        print (f"ERROR the timestamp {date} does not match initial {first}. Looks mixup in datasets")
                        continue
                if prot not in data:
                    data[prot] = 0
                data[prot] = data[prot] + int(freq)
            except ValueError as e:
               print (f"[ERROR] cannot parse: {line}")
               traceback.print_exc()

# open file on last timestamp
if first is None:
    d = args.directory
    print (f"ERROR Could not find a timestamp. dirname={directory}.Abort. Data dump is shown below")
    pprint.pprint(data)
    sys.exit(1)
target_file = args.root  + os.sep + first + ".lst"
if os.path.exists(target_file):
    print (f"target_file {target_file} already exists")
    sys.exit(1)


sorted_dict = dict(sorted(data.items(), key=lambda item: item[1],reverse=True))
with open(target_file, 'w', encoding='utf-8') as file:
    for k in sorted_dict:
        file.write(k + "," + str(data[k]) + "\n")
file.close()
