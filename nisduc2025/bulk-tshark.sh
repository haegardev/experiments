#!/bin/bash


# Example ./bulk-bpf.sh ~/pcaps/disk1/d4/8724lnrfnajnc2/1/2024/10/17/8724lnrfnajnc2-2024-10-17-094341.cap.gz  ~/toto/ "udp or icmp"
# Created in folder ~/toto/2024/10/17/8724lnrfnajnc2-2024-10-17-094341.cap.gz
# Need to have tcpdump installed
FILENAME=$1
TARGET=$2
BPF_FILTER=$3

is_number() {
    local input="$1"
    if [[ "$input" =~ ^[0-9]+$ ]]; then
        return 0  # true
    else
        return 1  # false
    fi
}

if [ -z "$FILENAME" ]; then
    echo "A filename must be specified" >&2
    exit 1
fi

if [ -z "$TARGET" ];  then
    echo "A target must be specified" >&2
    exit
fi

if [  ! -d "$TARGET" ]; then
    echo "Target directory does not exist" >&2
    exit 1
fi


if [[ "$TARGET" == */ ]]; then
    TARGET="${TARGET::-1}"
fi

# Assume file structure
# some/directories/year/month/day/filename.cap.gz
# want to keep year/month/day/filename

filename=$(basename "$FILENAME")
IFS='/' read -r -a parts <<< "$FILENAME"
len=${#parts[@]}
year=${parts[$((len - 4))]}
month=${parts[$((len - 3))]}
day=${parts[$((len - 2))]}

if is_number "$year"; then
    if is_number "$month"; then
        if is_number "$day"; then
            TD="$TARGET/$year/$month/$day/"
            TF="$TD/$filename"
            # Create the directory structure and  apply BPF filter
            if [ ! -d "$TF" ]; then
                mkdir -p $TD
            fi
            if [ -e $TF ]; then
                echo "Target file $TF already exist, skip" >&2
                exit 0
            else
		    TF2=$(echo $TF | sed '#s.cap.gz#.txt#g')
                    tshark -T fields -e frame.protocols -n -r $FILENAME > $TF2
            fi
        fi
    fi
fi
