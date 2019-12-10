#!/bin/bash

#TODO: Include usage
N_FROM=$1
N_TO=$2
N_REP=$3
TODAY=`date +'%d%m%Y'`
LOG_FILE_NAME="log_"$TODAY
LOG_DIR="../logs/"$LOG_FILE_NAME
BASELINE_LOG_FILE_NAME=$LOG_DIR"/"$LOG_FILE_NAME"_baseline.log"
BASELINE_ALL_LOG_FILE_NAME=$LOG_DIR"/"$LOG_FILE_NAME"_baseline_all_colors.log"
SKIPPING_LOG_FILE_NAME=$LOG_DIR"/"$LOG_FILE_NAME"_skipping.log"
mkdir -p $LOG_DIR
# touch $BASELINE_LOG_FILE_NAME
# touch $SKIPPING_LOG_FILE_NAME
EXE_DIR="../build/"
BASELINE_ALL_COLORS_EXE=$EXE_DIR"/test-baseline-all-colors-random-gen"
BASELINE_EXE=$EXE_DIR"/test-baseline-random-gen"
SKIPPING_EXE=$EXE_DIR"/test-miner-random-gen"
SEEDS=(0 9843 27837 19341 29044)
for seed in "${SEEDS[@]}"
do
  for ((n = $N_FROM; n <= $N_TO; n=n*10 ))
  do
    for ((i = 2; i <=32; i*=4));
    do
      for ((j = 2; j <=32; j*=4));
      do
        for ((k = 1; k <= $N_REP; k++))
        do
          echo "n: "$n" alpha_x: "$i" alpha_c: "$j" seed: "$seed" baseline"
          $BASELINE_EXE -x $i -c $j -n $n -o false -s $seed >> $BASELINE_LOG_FILE_NAME"_"$n"_"$seed &
          while [ `jobs -r | wc -l | xargs` -ge 3 ]; do
            wait -n
          done

          echo "n: "$n" alpha_x: "$i" alpha_c: "$j" seed: "$seed" skipping no boost"
          $SKIPPING_EXE -x $i -c $j -n $n -o false -s $seed -b false >> $SKIPPING_LOG_FILE_NAME"_"$n"_"$seed &
          while [ `jobs -r | wc -l | xargs` -ge 3 ]; do
            wait -n
          done

          echo "n: "$n" alpha_x: "$i" alpha_c: "$j" seed: "$seed" skipping boost"
          $SKIPPING_EXE -x $i -c $j -n $n -o false -s $seed -b true >> $SKIPPING_LOG_FILE_NAME"_boost_"$n"_"$seed &
          while [ `jobs -r | wc -l | xargs` -ge 3 ]; do
            wait -n
          done

          echo "n: "$n" alpha_x: "$i" alpha_c: "$j" seed: "$seed" baseline all colors"
          $BASELINE_ALL_COLORS_EXE -x $i -c $j -n $n -o false -s $seed >> $BASELINE_ALL_LOG_FILE_NAME"_"$n"_"$seed &
          while [ `jobs -r | wc -l | xargs` -ge 3 ]; do
            wait -n
          done
          # wait
        done
      done
    done
  done
done
wait
