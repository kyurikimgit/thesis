#!/bin/bash

#rm output.csv
# Run CT01.sh
#./MES23.sh
#./CT01.sh
# Check if CT01.sh was successful before running ./pattern
if [ $? -eq 0 ]; then
    # Run ./pattern
    # output and outputNew are independent
    ./deduplicate_csv.sh output.csv
    ./deduplicate_csv.sh outputNew.csv
    mv output_.csv output.csv
    mv outputNew_.csv outputNew.csv
    ./reducedCostCheck
   # mv outputNew.csv output.csv
else
    echo "Error: CT01.sh failed. Aborting."
fi

