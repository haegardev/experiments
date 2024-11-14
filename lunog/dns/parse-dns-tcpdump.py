#!/usr/bin/python3

import re
import sys
from collections import Counter

def count_dates(filename):
    # Regular expression to match dates in the format YYYY-MM-DD
    date_pattern = re.compile(r'^\d{4}-\d{2}-\d{2}')
    # Counter to keep track of occurrences per date
    date_counts = Counter()
    # Open the file and process each line
    with open(filename, 'r') as file:
        for line in file:
            # Search for a date at the beginning of the line
            match = date_pattern.match(line)
            if match:
                # Extract the date and update the counter
                date = match.group(0)
                date_counts[date] += 1

    # Print the results
    for date, count in sorted(date_counts.items()):
        print(f"{date}: {count}")

# Ensure a filename is provided as a command-line argument
if len(sys.argv) != 2:
    print("Usage: python script.py <filename>")
else:
    filename = sys.argv[1]
    count_dates(filename)

