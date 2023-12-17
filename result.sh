#!/bin/bash

num_runs=5
total_time_serial=0
input="../Sample/DUCK.bmp"

cd ./serial
make

for ((i=1; i<=$num_runs; i++)); do
    # Use time command to measure the execution time
    runtime=$( { time -p ./Serial.out "$input" > /dev/null 2>&1; } 2>&1 | grep real | awk '{print $2}' )

    # Add to the total time
    total_time_serial=$(echo "$total_time_serial + $runtime" | bc)
done

# Calculate the average runtime
average_time_serial=$(echo "scale=4; $total_time_serial / $num_runs" | bc)

echo "SERIAL : Average runtime over $num_runs runs: $average_time_serial seconds"



total_time_parallel=0
cd ..
cd ./parallel
make

for ((i=1; i<=$num_runs; i++)); do
    # Use time command to measure the execution time
    runtime=$( { time -p ./Parallel.out "$input" > /dev/null 2>&1; } 2>&1 | grep real | awk '{print $2}' )

    # Add to the total time
    total_time_parallel=$(echo "$total_time_parallel + $runtime" | bc)
done

# Calculate the average runtime
average_time_parallel=$(echo "scale=4; $total_time_parallel / $num_runs" | bc)

echo "PARALLEL : Average runtime over $num_runs runs: $average_time_parallel seconds"





speedup=$(echo "scale=4; $average_time_serial / $average_time_parallel" | bc)
echo "SPEED UP over $num_runs runs: $speedup "






