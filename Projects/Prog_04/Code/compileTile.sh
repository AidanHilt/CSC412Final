gcc -o tile tile.c Image_IO/imageIO_TGA.c Image_IO/RasterImage.c -Wall -Wextra -lm
./tile ../chris_image_collection/stick_figure_marine/tga_files/sfm_01.tga ../chris_image_collection/stick_figure_marine/tga_files/sfm_02.tga ../chris_image_collection/stick_figure_marine/tga_files/sfm_03.tga ../chris_image_collection/stick_figure_marine/tga_files/sfm_04.tga final.tga
rm tile