flags="-Wall -Wextra -Wconversion -Wshadow -Wcast-align -Wmissing-declarations -Winline -pedantic-errors -Woverloaded-virtual -Wold-style-cast -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel"
g++ main.cpp -o main
./main "$1" "$2" "$3"
rm main