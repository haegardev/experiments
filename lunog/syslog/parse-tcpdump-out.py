#!/usr/bin/python3
import os
import re
import json
import argparse
from collections import Counter

# Set up argparse to take filename as a command-line argument
parser = argparse.ArgumentParser(description="Parse network data and extract information block by block.")
parser.add_argument("--filename", type=str, help="The input file containing the network data.")
parser.add_argument("--target", type=str, help="Target directory where the json files should be stored")

args = parser.parse_args()
args.filename = args.filename.replace("./","")

# Regular expressions for parsing
timestamp_pattern = re.compile(r'(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2})')
ip_port_pattern = re.compile(r'IP (\d+\.\d+\.\d+\.\d+)\.(\d+) > (\d+\.\d+\.\d+\.\d+)\.(\d+):')
payload_start_pattern = re.compile(r'<\d+>.*: (.*)')

# Initialize list for storing parsed results
results = []

# Helper function to count words in a text payload with more than 3 characters
def count_words(payload):
    words = re.findall(r'\b\w{4,}\b', payload)  # words with more than 3 characters
    word_count = Counter(words)
    return word_count

# Read and process the file block by block
with open(args.filename, 'r') as file:
    current_block = {}
    for line in file:
        # Check if the line has a new timestamp, marking a new block
        timestamp_match = timestamp_pattern.match(line)
        if timestamp_match:
            # If there is an existing block, process it
            if current_block:
                results.append(current_block)
            # Start a new block with the extracted timestamp
            current_block = {
                "timestamp": timestamp_match.group(1),
                "src_ip": None,
                "src_port": None,
                "dst_ip": None,
                "dst_port": None,
                "word_counts": {}  # Dictionary to track word occurrences in the payload
            }
        # Look for the IP and port information if not yet set in the current block
        if not current_block["src_ip"]:
            ip_port_match = ip_port_pattern.search(line)
            if ip_port_match:
                current_block["src_ip"] = ip_port_match.group(1)
                current_block["src_port"] = ip_port_match.group(2)
                current_block["dst_ip"] = ip_port_match.group(3)
                current_block["dst_port"] = ip_port_match.group(4)

        # Look for the payload and count words (more than 3 characters) in it
        payload_match = payload_start_pattern.search(line)
        if payload_match:
            payload = payload_match.group(1)
            current_block["word_counts"].update(count_words(payload))

    # Append the last block if it exists
    if current_block:
        results.append(current_block)

# Convert the Counter to a dictionary to make it JSON serializable
for result in results:
    result["word_counts"] = dict(result["word_counts"])



if args.target.endswith("/"):
    args.target = args.target[:-1]



tf = args.target+"/"+args.filename.replace(".tcpdump",".json")
td = os.path.dirname(tf)
try:
    os.makedirs(td)
except OSError as a:
    pass

# Write the results to a JSON file
with open(tf, 'w') as json_file:
    json.dump(results, json_file, indent=4)
