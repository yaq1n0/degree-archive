#!/bin/bash
cd $1
for file in *; do
  echo $(grep -c "<Author>" $file) # count instances of <Author>
done
