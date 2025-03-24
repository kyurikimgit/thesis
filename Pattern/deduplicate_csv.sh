#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <input_csv_file>"
    exit 1
fi

input_file=$1

if [ ! -f "$input_file" ]; then
    echo "Error: File $input_file not found."
    exit 1
fi

# Use awk to eliminate redundant rows and print the result
awk -F, '!seen[$0]++' "$input_file" > "${input_file%.csv}_.csv"

echo "Deduplication completed. Results saved to ${input_file%.csv}_.csv"

