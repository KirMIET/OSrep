#!/bin/bash

count=$#
sum=0

for num in "$@"; do
    sum=$(echo "$sum + $num" | bc -l)
done

average=$(echo "$sum / $count" | bc -l)

echo "Количество: $count"
echo "Среднее: $average"
