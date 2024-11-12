#!/usr/bin/python3

import pprint
import json
import os
import re
import argparse



from collections import defaultdict, Counter

# Set up argparse to take filename as a command-line argument
parser = argparse.ArgumentParser(description="Parse network data and extract information block by block.")
parser.add_argument("--directory", type=str, help="The directory containing the json files with syslog data")
args = parser.parse_args()


# Dictionary to store unique IPs per day and aggregated word counts
unique_ips_per_day = defaultdict(set)
aggregated_word_counts = Counter()

# Function to extract the date portion (YYYY-MM-DD) from a timestamp
def extract_date(timestamp):
    return timestamp.split(" ")[0]

def identify_non_number(s):
    # Check if it's a decimal or hexadecimal number
    if re.fullmatch(r'[+-]?\d+', s) or re.fullmatch(r'0[xX][0-9a-fA-F]+', s) or re.fullmatch(r'[0-9a-fA-F]+', s):
        return False
    else:
        return True


# Process each JSON file
for filename in os.listdir(args.directory):
    if filename.endswith(".json"):
        file_path = os.path.join(args.directory, filename)
        #print(f"Processing file: {file_path}")
        with open(file_path, 'r') as json_file:
            data = json.load(json_file)  # Load the JSON data
            for entry in data:
                # Extract date and source IP
                date = extract_date(entry['timestamp'])
                src_ip = entry['src_ip']
                # Add the source IP to the set for the specific date
                unique_ips_per_day[date].add(src_ip)
                # Update the aggregated word counts with this block's word counts
                aggregated_word_counts.update(entry['word_counts'])

# Count the unique IPs per day
unique_ip_counts = {date: len(ips) for date, ips in unique_ips_per_day.items()}

# Print the unique IP counts per day
#pprint.pprint(aggregated_word_counts)

#print("Unique Syslog Source IPs per Day:")
for date, count in sorted(unique_ip_counts.items()):
    buf = []
    # Print the aggregated word counts
    for word, count in aggregated_word_counts.most_common():
        if identify_non_number(word):
            #buf.append(word+":"+str(count))
            buf.append((word,count))
    s = json.dumps(buf)
    print(f"{date} {count} {s}" )

