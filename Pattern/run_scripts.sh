#!/bin/bash

#rm output.csv
# Run CT01.sh
#./MES23.sh
#./CT01.sh
# Check if CT01.sh was successful before running ./pattern
if [ $? -eq 0 ]; then
    # Run ./pattern
    ./deduplicate_csv.sh output.csv
    mv output_.csv output.csv
    ./pattern
#    rm output.csv
else
    echo "Error: CT01.sh failed. Aborting."
fi

