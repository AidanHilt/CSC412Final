if [ $# -ne 2 ];
then 
    echo You provided an invalid number of arguments. Please provide two directories, the first of which contains TGA images. 
    exit -1
fi

if [ ! -d "$1" ];
then   
    echo The first argument is not a valid directory
    exit -1
fi

if [ ! -d "$2" ];
then
    echo The second argument is not a valid directory
    exit -1
fi

compiler="gcc"
flags="-Wall -Wextra"
dimName="dimensions"
cropName="crop"
splName="split"
rotName="rotate"

$compiler dimensions.c -o $dimName $flags
$compiler crop.c -o $cropName $flags
$compiler split.c -o $splName $flags
$compiler rotate.c -o $rotName $flags

files=($(ls "$1" | grep .tga$))

for i in "${files[@]}"
do
    width=`./$dimName -w "$1/$i"`
    height=`./$dimName -h "$1/$i"`
    middleWidth=$(( $width/2 ))
    middleHeight=$(( $height/2 ))
    `./$cropName "$1/$i" "$2"  0  0 $middleWidth $middleHeight`
    `./$cropName "$1/$i" "$2" $(($middleWidth-1)) 0 $middleWidth $middleHeight`
    `./$cropName "$1/$i" "$2" 0 $(($middleHeight-1)) $middleWidth $middleHeight`
    `./$cropName "$1/$i" "$2" $(($middleWidth-1)) $(($middleHeight-1)) $middleWidth $middleHeight`
    `./$splName "$1/$i" "$2"`
    `./$rotName l "$1/$i" "$2"`
done

rm $dimName
rm $cropName
rm $splName
rm $rotName