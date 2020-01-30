dataDir=$(realpath "$1")
if [[ ! $dataDir == *"/" ]] ;
    then
    dataDir="${dataDir}/"
    fi

outFile=$(realpath "$2")
if [[ ! $outFile == *".c" ]] ;
    then
    outFile="${outFile}.c"
    fi

mapfile files -t < <(find "$dataDir" | grep -v '/$')
numbers=()

for file in "${files[@]}"
    do
        tmp=$(mktemp)
        tr -d '\r' < "${file//$'\n'/}" > "$tmp"
        cat "$tmp" > "${file//$'\n'/}"
        rm $tmp
        numbers+=($(cat "${file//$'\n'/}" | grep ^[0-9]* -o))
    done

max=${numbers[0]}

for number in ${numbers[@]}
    do 
        ((number > max)) && max=$number
    done


max=$((max+1))

g++ -o "../Source/Version 5/main" "../Source/Version 5/main.cpp"

cd "../Source/Version 5/" && "./main" "$max" "$dataDir" "$outFile"

rm main