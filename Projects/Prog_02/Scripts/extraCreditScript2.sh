#!/bin/bash

#This ended up an abomination, but here it is. For each of the first four, we
#call ls on the directory, with the -p flag, pipe it to grep, so it removes all the
#entries with '/' in them (i.e. directories), then we grep again, this time looking for
#the appropriate patterns in the order specified. Then, we look specifically for
#directories, then, for our last one, we are going to systematically eliminate 
#ALL of the previous categories, leaving only the "others".

#Yes, this is the same script. ls lists its output in alphabetical order
if [[ $# == 1 ]];
    then
    ls -p $1 | grep -e "\.c$" -e "\.cpp$"
    ls -p $1 | grep -e "\.h$" -e "\.hpp$"
    ls -p $1 | grep -e "\.sh$"
    ls -p $1 | grep -e "\.txt$"
    ls -p $1 | grep / | tr -d /
    ls -p $1 | grep -v / | grep -v -e "\.c$" -e "\.cpp$" -e "\.h$" -e "\.hpp$" -e "\.sh$" -e "\.txt$"
    else
    echo Error: You must provide a single, valid directory.
    fi