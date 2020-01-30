#include <stdio.h>

void floatInverter(float* array, int size){
    for(int i = 0; i < size; i++){
        unsigned char* rep = (unsigned char*) *array + i; 
        rep[3] = rep[3]|0x80;
    }
}

int main(int argc, char* argv[]){
    float test[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    int size = 5;
    floatInverter(test, size);
    for(int i = 0; i < size; i++){
       printf("%f", test[i]); 
    }
}