#!/bin/bash
# Process onion files in slurm environment. This environment gives a rank and
# number of tasks which are used to partition the data in the FILELIST. Each
# process knows which chunk it should process

rank="${SLURM_PROCID}"
ntasks="${SLURM_NTASKS}"

rank=$1
ntasks=4

mapfile -t files < files.txt
n="${#files[@]}"

# ceil(n/ntasks)
chunk=$(( (n + ntasks - 1) / ntasks ))
start=$(( rank * chunk ))
end=$(( start + chunk ))
(( end > n )) && end=$n

for ((i=start; i<end; i++)); do
  f="${files[$i]}"
  echo "rank $rank processing $f"
  #./process_one_file "$f"
done
