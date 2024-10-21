#  Parse pcap files with tshark
<pre>
 cat files2024.lst  | parallel -j 20 /home/gerard/experiments/lunog/overview/proc.sh {}
</pre>
# Create filelists of tshark output per day
<pre>
 ./create-file-lists-per-day.sh /disk2/blackhole /disk2/filelists/
</pre>
# Count in parallel
<pre>
find /disk2/filelists/ | sort | parallel -j 25 ~/experiments/lunog/overview/basic-stats -l  {} > ~/counted-packets.dat
</pre>
