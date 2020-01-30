#include <string>
#include <iostream>

using namespace std;

typedef enum TravelerType {
	RED_TRAV = 0,
	GREEN_TRAV,
	BLUE_TRAV,
	//
	NUM_TRAV_TYPES
} TravelerType;

void incrementColor(int &input, TravelerType type){
    if(type == RED_TRAV){
        int addPart = input & 0x000000FF;
        int rest = input & 0xFFFFFF00;

        addPart += 0x20;
        if(addPart > 0xFF) addPart = 0xFF;

        input = rest | addPart;
    }else if(type == GREEN_TRAV){
        int addPart = input & 0x0000FF00;
        int rest = input & 0xFFFF00FF;

        addPart += 0x2000;
        if(addPart > 0xFF00) addPart = 0xFF00;

        input = rest | addPart;
    }else{
        int addPart = input & 0x00FF0000;
        int rest = input & 0xFF00FFFF;

        addPart += 0x200000;
        if(addPart > 0xFF0000) addPart = 0xFF0000;

        input = rest | addPart;
    }
}

int main(){
    int one = 0xFF000000;
    int two = 0xFF000000;
    int three = 0xFF00F000;
    TravelerType t1 = RED_TRAV;
    TravelerType t2 = GREEN_TRAV;
    TravelerType t3 = BLUE_TRAV;
    /*
    for(int i = 0; i < 10; i++){
        incrementColor(one, t1);
        cout << "One = " << hex << one << '\n';
    }*/

    /*
    for(int i = 0; i < 10; i++){
        incrementColor(two, t2);
        cout << "Two = " << hex << two << '\n';
    }*/

    
    for(int i = 0; i < 10; i++){
        incrementColor(three, t3);
        cout << "Three = " << hex << three << '\n';
    }
}