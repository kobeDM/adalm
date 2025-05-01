#!/bin/bash

# Output file name
output_file="merge.dat"

# initializes
> "$output_file"

# merge data files
for file in out_*.dat; do
    if [ -f "$file" ]; then
        cat "$file" >> "$output_file"
    fi
done

echo "Merge was completed. Outputfile: $output_file"