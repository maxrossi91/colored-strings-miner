#!/bin/bash

#TODO: Include usage
N_REP=$1
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
BASELINE_ALL_COLORS_EXE=$EXE_DIR"/test-baseline-all-real-data-mangrove"
BASELINE_EXE=$EXE_DIR"/test-baseline-real-data-mangrove"
SKIPPING_EXE=$EXE_DIR"/test-real-data-mangrove"
TRACES=("../../traces/Mangrove/bl_master_100000.vcd -c wb_dat_o -v 155"
        "../../traces/Mangrove/serial_transmitter.vcd -c val -v 1"
        "../../TracesMangrove1/b03/b03_trace_100k.vcd -c grant_o -v 1"
        "../../TracesMangrove1/b06/b06_trace.vcd -c cc_mux -v 1"
        "../../TracesMangrove1/s386/s386_trace.vcd -c v13_D_9 -v 2"
        "../../TracesMangrove1/camellia/camellia_trace.vcd -c Dout -v 0")
TRACES_ALL=("../../traces/Mangrove/bl_master_100000.vcd -c wb_dat_o "
        "../../traces/Mangrove/serial_transmitter.vcd -c val "
        "../../TracesMangrove1/b03/b03_trace_100k.vcd -c grant_o "
        "../../TracesMangrove1/b06/b06_trace.vcd -c cc_mux "
        "../../TracesMangrove1/s386/s386_trace.vcd -c v13_D_9 "
        "../../TracesMangrove1/camellia/camellia_trace.vcd -c Dout ")
for ((k = 1; k <= $N_REP; k++))
do
  for trace in "${TRACES[@]}"
  do
    echo $trace" baseline"
    $BASELINE_EXE $trace -o false &
    while [ `jobs -r | wc -l | xargs` -ge 3 ]; do
      wait -n
    done

    echo $trace" skipping no boost"
    $SKIPPING_EXE $trace -o false -b false &
    while [ `jobs -r | wc -l | xargs` -ge 3 ]; do
      wait -n
    done

    echo $trace" skipping boost"
    $SKIPPING_EXE $trace -o false -b true &
    while [ `jobs -r | wc -l | xargs` -ge 3 ]; do
      wait -n
    done

    # wait
  done
done

for ((k = 1; k <= $N_REP; k++))
do
  for trace in "${TRACES_ALL[@]}"
  do

    echo $trace" baseline all colors"
    $BASELINE_ALL_COLORS_EXE $trace -o false &
    while [ `jobs -r | wc -l | xargs` -ge 3 ]; do
      wait -n
    done
    # wait
  done
done
wait
