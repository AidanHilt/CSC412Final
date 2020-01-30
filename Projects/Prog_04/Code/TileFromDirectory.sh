if [ ! $# -eq 2 ]
    then 
    echo Improper amount of arguments. Correct arguments: inputDirectory outputDirectory
    exit 1
fi

if [ ! -d $1 ]
    then
    echo The first argument needs to be a directory that exists
    exit 2
fi

if [ ! -d $2 ]
    then
    echo The second argument needs to be a directory that exists
    exit 3
fi

mapfile -t imageList < <(find "$1" | grep .tga$ | sort --version-sort)

if [ ! ${#imageList[@]} -ge 1 ]
    then
    echo The directory must contain at least one tga image
    exit 4
fi

gcc -o tile tile.c Image_IO/imageIO_TGA.c Image_IO/RasterImage.c -lm
outputName=${1%/}
./tile ${imageList[@]} "$2"/${outputName##*/}_tiled.tga
rm tile