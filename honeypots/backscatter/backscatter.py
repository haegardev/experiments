#!/usr/bin/python
#
#backscatter.py - Analyze backscatter traffic observed on honeypots
#
# Copyright (C) 2017 Gerard Wagener
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#

import sys
import argparse
import pprint
import datetime
import time
import math

data = []

def loadfile(filename):
    f = open(filename,"r")
    for line in f.readlines(): #Sort according timestamp
        try:
            line = line[:-1]
            (epoch, ipsrc, srcport, dst, dstport, udp_srcport, udp_dstport, proto, flags, ttl, seq) = line.split(',')
            (a,b) = epoch.split('.')
            entry = dict()
            entry['timestamp'] = (int(a),(b))
            entry['ipsrc'] = ipsrc
            entry['proto'] = int(proto)
            if entry['proto'] == 6: #TCP
                entry['srcport'] = int(srcport)
                entry['dstport'] = int(dstport)
            if entry['proto'] == 17: #UDP
                entry['srcport'] = int(udp_srcport)
                entry['dstport'] = int(udp_dstport)
            entry['flags'] = flags
            entry['ttl'] = ttl
            entry['seq'] = seq
            data.append(entry)
        except IndexError,e:
            print "Failed to parse "+ str(e)

def dump_targets():
    stats = dict()
    for e in data:
        if stats.has_key(e["ipsrc"]) == False:
            stats[e['ipsrc']] = dict()
            stats[e['ipsrc']]['counter'] = 0
            stats[e['ipsrc']]['firstseen'] = sys.maxint
            stats[e['ipsrc']]['lastseen'] = 0
        stats[e['ipsrc']]['counter']+=1
        if e['timestamp'][0] < stats[e['ipsrc']]['firstseen']:
            stats[e['ipsrc']]['firstseen'] = e['timestamp'][0]
        if e['timestamp'][0] > stats[e['ipsrc']]['lastseen']:
            stats[e['ipsrc']]['lastseen'] = e['timestamp'][0]

    if len(stats) > 0:
        print "#Target IP|Frequency|First seen|Last seen"
        for ip in stats.keys():
            #FIXME does not respect timezones
            f = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(stats[ip]['firstseen']))
            g = time.strftime("%Y-%m-%d %H:%M:%S",time.localtime(stats[ip]['lastseen']))
            sys.stdout.write(str(ip) + "|" + str(stats[ip]['counter']) + "|" +
                             f + "|" + g +"\n")

def packets_per_seconds(ip):
    d = dict()
    for packet in data:
        (t,s) = packet['timestamp']
        if packet['ipsrc']== ip:
            if d.has_key(t)==False:
                d[t] = 0
            d[t]+=1
    values = dict() 

    nt = 0
    cs = 0
    for k in d.keys():
        if values.has_key(d[k]) == False:
            values[d[k]] = 0
        values[d[k]]+=1
        nt+=1

    if len(values) > 0:
        sys.stdout.write("#Packets per seconds | Frequency\n")
        sys.stdout.write("#Number of unique timestamps: "+ str(nt) +"\n")
        for pktsec in sorted(values, key=values.get, reverse=True):
            cs+=pktsec * values[pktsec]
            sys.stdout.write(str(pktsec) + "|" + str(values[pktsec])+"\n")
        sys.stdout.write("#Number of flows " + str(cs) + "\n")
             
def  packets_per_seconds_plot(ip):
    d = dict()
    for packet in data:
        (t,s) = packet['timestamp']
        if packet['ipsrc']== ip:
            if d.has_key(t)==False:
                d[t] = 0
            d[t]+=1
    values = dict() 

    for k in d.keys():
        sys.stdout.write(str(k) + " " + str(d[k]) + "\n")
             
        
#Returns First seen and last seen
def get_boundaries(ip, day=None):
    d = dict()
    firstseen = sys.maxint
    lastseen = 0
    for packet in data:
        if packet['ipsrc'] == ip:
            (t,s) = packet['timestamp']
            if t < firstseen:
                firstseen = t
            if t > lastseen:
                lastseen = t
    #FIXME maxint or 0 could be returned
    return (firstseen, lastseen)


def packets_per_hour_plot(ip, interval):
    (firstseen, lastseen) = get_boundaries(ip)
    duration = lastseen - firstseen
    #Try to get to the closed hour
    #print "[DEBUG]",  time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(firstseen))
    firstseen = (firstseen / interval)*interval
    #print "[DEBUG]",  time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(firstseen))

#    print "[DEBUG] firstseen", firstseen
#    print "[DEBUG] lastseen", lastseen
    #FIXME print duration for targets option
#    print "[DEBUG] Duration", duration
    bins = int(math.ceil(float(duration) / float(interval)))+1
#    print "[DEBUG] Number of bins", bins

    chunks = range(0,bins)

    for packet in data:
        if packet['ipsrc'] == ip:
            (t,s) = packet['timestamp']
            x = t-firstseen
            idx = x / interval
            chunks[idx]+=1

    for i in xrange(0,bins):
        start = i*interval + firstseen
        end  = (i+1)*interval + firstseen
#        print "[DEBUG] Interval [",start, ",", end, "]:",chunks[i]
        #FIXME might be wrong timezone - localtime expects GMT?
        f = time.strftime("%Y-%m-%d %H",time.localtime(start))
        print f, chunks[i]


parser = argparse.ArgumentParser(description="Handle backscatter traffic of tshark")

parser.add_argument("--targets", dest='targets', action='store_true', help="Dump the list of targets")
parser.add_argument("--filename", type=str, help="Output file of custom tshark output that is interpreted", required=True, nargs=1)
parser.add_argument("--ip", type=str, help="Sepcify the IP address of a target")
parser.add_argument("--seconds", dest='seconds', action='store_true', help="Count the number of packets per seconds")
parser.add_argument("--plot", dest='plot', action='store_true', help = "Create plot data. Packets per seconds")
parser.add_argument("--hours", dest='hours', action="store_true", help= "Packets per hours for target specified with ip option")
args = parser.parse_args()
loadfile(args.filename[0])

if args.targets:
    dump_targets()
    sys.exit(0)

if args.seconds:
    if args.ip is None:
        sys.stderr.write("A target IP must be specified with the -i switch\n")
    sys.stdout.write("#Packets per seconds for target " +  str(args.ip) + "\n")
    if args.plot:    
        packets_per_seconds_plot(args.ip)
    else:
        packets_per_seconds(args.ip)
#FIXME give time range as parameter
if args.hours:
    if args.ip is None:
        sys.stderr.write("A target IP must be specified with the -i switch\n")
    sys.stdout.write("#Packets per hour for target " +  str(args.ip) + "\n")
    if args.plot:
        packets_per_hour_plot(args.ip, 3600)
