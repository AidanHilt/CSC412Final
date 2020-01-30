#!/bin/bash
currentMax=$2

#Helper function. Given an array and an item, loops through the array, checking 
#at each index if the item matches. If they do, it returns 0, indicating success,
#and if not, 1, indicating failure.
isInArray(){
    local match=$1
    shift
    for item;
        do
        if [[ "$item" == "$match" ]];
        then 
        return 0
        fi
        done
    return 1
        
}

#Checks if there are less than 2 arguments. If there are, then the user has not provided 
#a path to an executable and at least 1 strictly positive integer, meaning the user used the program
#wrong.
if (($# < 2));
    then
    echo "Too few arguments provided"
    echo "Proper usage: launcherScript.sh <path to executable> m1 [m2 [m3 [...]]]"
    exit 1
    fi

#At this point, we're checking if the user has a './' in their executable arg. If they do, we 
#simulate the expansion of ./ by getting the value of the pwd command, and appending it to the user's script argument.
#If not, then we simply copy the value of the user's argument to that same variable. This
#process is performed so that we don't, in the case where the user IS targeting their
#local directory but didn't provide a './,' have to perform guesswork as to what exactly they meant.
#The user either provides it, or they forget and it fails at the next step, in which
#case they know they'll need to change the argument. It also works for ../<executable>.
directory=$(pwd)
if [ ${1:0:2} == "./" ];
    then
    fullPath=$directory/${1:2}
    elif [ ${1:0:3} == "../" ];
    then
    fullPath=$(dirname $directory)/${1:3}
    else
    fullPath=$1
    fi

#At this point, we're going to cd out to the root, then look for the file, which should be specified by a full, working
#directory. If it's not found, then there's something going on, and we fail here. If we don't, we change the working directory back
#and continue.
cd /
if [ ! -x $fullPath ];
    then 
    echo "No executable file with at the path $1 could be found"
    echo "Proper usage: launcherScript.sh <path to executable> m1 [m2 [m3 [...]]]"
    exit 1
    fi
cd $directory

#To fulfill the extra credit requirement of no repeats, and to check for valid strictly positive integers,
#I opted to use regex and an array. This regex looks for between 0 and infinite leading zeroes, then
#at least one digit between 1 and 9. After that, it looks for between 0 and infinite occurences of 
#digits between 0 and 9, to account for numbers like 10, 100, 1000.
regex="^0*[1-9]+[0-9]*$"
noRepArray=()

#Starts a loop at 0, and goes up to the number of arguments. At each stage, we make a string out of the
#arguments using printf and the %s tag, then compare it to our regex. If it matches, then we check if it 
#is in the noRepArray, and if it is not, add it to the array. If it is in the array, we simply do nothing.
#It is also at this point that we're looping through the array to find the maximum item in the array.
for ((i = 2; i < $# + 1; i++));
    do
    string=$( printf '%s' ${!i} )
    if [[ $string =~ $regex ]];
    then
        isInArray ${!i} ${noRepArray[@]}
        if (($? == 1));
            then
            noRepArray+=( ${!i} )

            if ((${!i} > $currentMax));
                then
                currentMax=${!i}
                fi
        fi
    else
    echo "'${!i}' is not a strictly positive integer. All arguments must be whole numbers > 0."
    echo "Proper usage: launcherScript.sh <path to executable> m1 [m2 [m3 [...]]]"
    exit 1
    fi
    done

#Here, we're calling the executable with the maximum item as its sole argument.
$1 $currentMax

#Looping through the noRepArray, which contains the arguments minus any repeats.
#And then looping past it, to get all combinations, and then calling the executable
#with both arguments.
for((i = 0; i < ${#noRepArray[@]}; i++));
    do
    for((j = i + 1; j < ${#noRepArray[@]}; j++));
        do
        $1 ${noRepArray[i]} ${noRepArray[j]}
        done
    done