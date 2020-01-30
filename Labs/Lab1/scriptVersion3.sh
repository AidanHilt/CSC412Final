#!/bin/bash
let total=0
for i in "$@"
do 
    ((total+=i))
done
echo $total