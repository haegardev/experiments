#!/usr/bin/python3
#
# Usage: tcpdump -n -tttt -r brol.cap | python3 annotate.py --mmdb=2024-11-12-GeoOpen-Country-ASN.mmdb

import re
import sys
import maxminddb
import argparse
import pprint
import ipaddress

parser = argparse.ArgumentParser(
    description="Read tcpdump output with the -tttt option from stdin and annotate IP addresses"
)
parser.add_argument("--mmdb", type=str, help="The MMDB file")
parser.add_argument(
    "--dont-skip-rfc1918",
    action="store_true",
    default=False,
    help="Don't skip private networks defined in RFC1918",
)
args = parser.parse_args()


def annotate_logs_with_country(mmdb_file_path):
    """
    Reads log lines from standard input, appends the country code to the first IP address,
    and writes the annotated lines to standard output.
    """
    # Compile a regex to match the first IP address in each line
    ip_regex = re.compile(r'\b\d{1,3}(?:\.\d{1,3}){3}\b')
    # Open the MaxMind database and process the input
    with maxminddb.open_database(args.mmdb) as reader:
        for line in sys.stdin:
            line = line.strip()  # Remove trailing whitespace
            match = ip_regex.search(line)
            if match:
                ip_address = match.group()
                if (
                    not args.dont_skip_rfc1918
                    and ipaddress.ip_address(ip_address).is_private
                ):
                    continue
                response = reader.get(ip_address)
                country_code = response['country']['iso_code']
                asn = response['country']['AutonomousSystemNumber']
                asn_name = response['country']['AutonomousSystemOrganization']
                annotated_line = line.replace(
                    ip_address, f"{ip_address} [{country_code},{asn},{asn_name}]"
                )
            else:
                annotated_line = (
                    line  # If no IP address is found, keep the line unchanged
                )
            print(annotated_line)


# Run the script
if __name__ == "__main__":
    annotate_logs_with_country(args.mmdb)
