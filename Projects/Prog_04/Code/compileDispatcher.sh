gcc -o dispatcher dispatcher.c Image_IO/imageIO_TGA.c Image_IO/RasterImage.c -Wall -Wextra -lm
./dispatcher ../Images/ ../Output/ split clown.tga rotate -rrllr clown.tga crop 23 45 80 100 clown.tga
rm dispatcher