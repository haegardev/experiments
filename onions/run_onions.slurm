#!/bin/bash
#SBATCH --job-name=onions
#SBATCH --account=<your project>
#SBATCH --partition=gpu
#SBATCH --nodes=4
#SBATCH --ntasks-per-node=1  # Number of GPUs to use
#SBATCH --gpus-per-node=0
#SBATCH --cpus-per-task=4
#SBATCH --mem=64G
#SBATCH --time=02:00:00
#SBATCH --output=logs/onions_%j.out
#SBATCH --error=logs/onions_%j.err
#SBATCH --qos=default

# -------------------------------
# Distributed training variables
# -------------------------------
export MASTER_ADDR=$(scontrol show hostname $SLURM_NODELIST | head -n 1)
export MASTER_PORT=29500
export WORLD_SIZE=$SLURM_NTASKS

srun --ntasks=$SLURM_NTASKS \
     --cpus-per-task=$SLURM_CPUS_PER_TASK \
     --gpus-per-task=0 \
     --export=ALL \
     bash -c '
export RANK=$SLURM_PROCID
export LOCAL_RANK=$SLURM_LOCALID
export CODECARBON_OFF=1
./process-hpc.sh
'
