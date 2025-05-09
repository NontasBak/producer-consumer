#!/bin/bash

CSV_FILE="benchmark_results.csv"
echo "Producers,Consumers,Average_Waiting_Time_ms" > $CSV_FILE

PRODUCER_VALUES=(1 2 4 6 8 10 14 18 22)
CONSUMER_VALUES=(1 2 4 6 8 10 14 18 22)

for p in "${PRODUCER_VALUES[@]}"; do
  for c in "${CONSUMER_VALUES[@]}"; do
    echo "Running with $p producers and $c consumers..."

    output=$(../prod-cons $p $c)
    waiting_time=$(echo "$output" | grep "Average waiting time:" | awk '{print $4}')

    if [ -z "$waiting_time" ]; then
      echo "Error: Could not extract waiting time for $p producers and $c consumers"
      waiting_time="ERROR"
    fi

    echo "$p,$c,$waiting_time" >> $CSV_FILE
    echo "Average waiting time: $waiting_time ms"
    sleep 1
  done
done

echo "Benchmark completed. Results saved to $CSV_FILE"
