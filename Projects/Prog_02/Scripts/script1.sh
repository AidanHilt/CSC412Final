#!/bin/bash

#Ok, so this one isn't too bad. First, we're checking if we have enough args,
#and if not, printing out an error message and aborting. If we DO, however, we're
#gonna call ls on the argument with the -p flag, which appends a '/' to directories.
#Then, we pipe that output to grep, which with the -v flag, inverts its matches. Give
#it a '/' to look for, and we've neatly eliminated the directories.
if [[ $# == 1 ]];
    then
    ls -p $1 | grep -v /
    else
    echo Error: You must provide a single, valid directory.
    fi