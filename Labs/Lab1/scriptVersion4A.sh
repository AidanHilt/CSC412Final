#!/bin/bash
seen=()
for value in $@;
do
    if[$value = @($seen)];
        echo $value
done