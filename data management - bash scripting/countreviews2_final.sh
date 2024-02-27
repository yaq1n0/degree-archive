#!/bin/bash
cd $1 # navigate to folder
for file in *; do # loop through files
  filename=$(basename $file .dat) # process filename without extension
  n=$(grep -c "<Author>" $file) # count instances of <Author>
  v="$v $filename $n \n" # append name and review count to variable
done
echo -e $v | sort -rnk 2 # echo all sorted by second field
