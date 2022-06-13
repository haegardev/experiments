#!/usr/bin/python3

#Format of output files. First line gives the WARC url preceded with #
#First column line number
#Second column: matched data type url, bitcoin, etc

import sys
import re
import argparse



parser = argparse.ArgumentParser(description='Process data from stdin do regexp and store output. First line is the name of the stream i.e. filename of gzip output or url. Each line is prefixed with the line number starting at 1')

parser.add_argument("-n","--name" ,required=True, help="Name where stdin cam from i.e compressed filename or URL")
parser.add_argument("-t","--target", required=True, help="Target where the data should be written")
parser.add_argument("-o", "--orginal", required=True, help="Original name of the data chunk)")

args = parser.parse_args()


#Uses at least 10GB of memory
#for line in sys.stdin.readlines():
#    print (line)


#Taken from https://github.com/CIRCL/AIL-framework/blob/master/bin/Cryptocurrencies.py
rd = {
    'bitcoin': {
                    'name': 'bitcoin',      # e.g. 1NbEPRwbBZrFDsx1QW19iDs8jQLevzzcms
                    'regex': r'\b(?<![+/=])[13][A-Za-z0-9]{26,33}(?![+/=])\b'
    },
    'bitcoin-bech32': {
                    'name': 'bitcoin',      # e.g. bc1qar0srrr7xfkvy5l643lydnw9re59gtzzwf5mdq
                    'regex': r'\bbc(?:0(?:[ac-hj-np-z02-9]{39}|[ac-hj-np-z02-9]{59})|1[ac-hj-np-z02-9]{8,87})\b'
    },
    'ethereum': {
                    'name': 'ethereum',     # e.g. 0x8466b50B53c521d0B4B163d186596F94fB8466f1
                    'regex': r'\b(?<![+/=])0x[A-Za-z0-9]{40}(?![+/=])\b'
    },
    'bitcoin-cash': {
                    'name': 'bitcoin-cash', # e.g. bitcoincash:pp8skudq3x5hzw8ew7vzsw8tn4k8wxsqsv0lt0mf3g
                    'regex': r'bitcoincash:[a-za0-9]{42}(?![+/=])\b'
    },
    'litecoin': {
                    'name': 'litecoin',     # e.g. MV5rN5EcX1imDS2gEh5jPJXeiW5QN8YrK3
                    'regex': r'\b(?<![+/=])[ML][A-Za-z0-9]{33}(?![+/=])\b'
    },
    'monero': {
                    'name': 'monero',       # e.g. 47JLdZWteNPFQPaGGNsqLBAU3qmTcWbRda4yJvaPTCB8JbY18MNrcmfCcxrfDF61Dm7pJc4bHbBW57URjwTWzTRH2RfsUB4
                    'regex': r'\b(?<![+/=()])4[A-Za-z0-9]{94}(?![+/=()])\b'
    },
    'zcash': {
                    'name': 'zcash',        # e.g. t1WvvNmFuKkUipcoEADNFvqamRrBec8rpUn
                    'regex': r'\b(?<![+/=()])t[12][A-Za-z0-9]{33}(?![+/=()])\b'
    },
    'dash': {
                    'name': 'dash',         # e.g. XmNfXq2kDmrNBTiDTofohRemwGur1WmgTT
                    'regex': r'\b(?<![+/=])X[A-Za-z0-9]{33}(?![+/=])\b'
    }
}


#Taken from: https://github.com/CIRCL/AIL-framework/blob/master/bin/modules/Onion.py
url_regex = "((http|https|ftp)?(?:\://)?([a-zA-Z0-9\.\-]+(\:[a-zA-Z0-9\.&%\$\-]+)*@)*((25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9])\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[1-9]|0)\.(25[0-5]|2[0-4][0-9]|[0-1]{1}[0-9]{2}|[1-9]{1}[0-9]{1}|[0-9])|localhost|([a-zA-Z0-9\-]+\.)*[a-zA-Z0-9\-]+\.onion)(\:[0-9]+)*(/($|[a-zA-Z0-9\.\,\?\'\\\+&%\$#\=~_\-]+))*)"

re.compile(url_regex)

#FIXME don't care if target file exists
f = open(args.target, "w")

cnt=0
f.write("#Original source:" + args.orginal+"\n")
f.write("#Name:"+args.name+"\n")
line = sys.stdin.readline()
while line:
    try:
        # do stuff with line
        line = sys.stdin.readline()
        cnt+=1
        #findall seems to return groups of the regexp fields
        for i in  (re.findall(url_regex, line)):
            for u in i:
                f.write (str(cnt)+"|url_regexp|"+u + "\n")
        #Try to find crypocurrencies
        for c in rd.keys():
            for u in re.findall(rd[c]['regex'], line):
                f.write(str(cnt)+"|"+rd[c]['name']+"|"+u+ "\n")
    except UnicodeDecodeError as u:
        pass

f.write("#Survived")
f.close()
