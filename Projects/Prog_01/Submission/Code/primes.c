#include <stdio.h>
#include <stdlib.h>

//Helper function to determine if a number is prime. Works by checking for divisibility
//of every number between 2 and n/2 (Since primeness can be checked with numbers between 2 and n^(1/2),
//we can confidently check between 2 and n/2).
int isPrime(int num){
    if(num == 1){
        return 0;
    }
    for(int i = 2; i < (int)(num/2); i++){
        if((num % i == 0)){
            return 0;
        }
    }
    return 1;
}

int strLen(const char* str){
    int i = 0;
    while(str[i] != 0){
        i++;
    }
    return i;
}

char* getExecName(const char* directory){
    int sizeOfInput = strLen(directory);
    int cutoffIndex = 0;
    for(int i = 0; i < sizeOfInput; i++){
        if(directory[i] == '/' || directory[i] == '\\'){
            cutoffIndex = i;
        }
    }
    char* execName = malloc((sizeOfInput - cutoffIndex) * sizeof(char));
    int execNameIndex = 0;
    for(int i = cutoffIndex + 1; i < sizeOfInput; i++){
        execName[execNameIndex] = directory[i];
        execNameIndex++;

    }
    return execName;
} 

//Helper function that converts a number less than 20 to its word equivalent. 
//Used for the extra credit requirement. Will crash the program if given an input
//such that 1<=n<20 is not true
char* convertNumber(int number){
    switch (number){
    case 1:
        return "one";
        break;
    case 2:
        return "two";
        break;
    case 3:
        return "three";
        break;
    case 4:
        return "four";
        break;
    case 5:
        return "five";
        break;
    case 6:
        return "six";
        break;
    case 7:
        return "seven";
        break;
    case 8:
        return "eight";
        break;
    case 9:
        return "nine";
        break;
    case 10:
        return "ten";
        break;
    case 11:
        return "eleven";
        break;
    case 12:
        return "twelve";
        break;
    case 13:
        return "thirteen";
        break;
    case 14:
        return "fourteen";
        break;
    case 15:
        return "fifteen";
        break;
    case 16:
        return "sixteen";
        break;
    case 17:
        return "seventeen";
        break;
    case 18:
        return "eighteen";
        break;
    case 19:
        return "nineteen";
        break;
    default:
        printf("Error, '%d' is not a number between 0 and 20, exclusive.\n", number);
        exit(1);
        break;
    }
}

//Calculates the first n primes, by looping up from 2, and checking each number
//for primality. When it finds one, it adds it into a generated array, adds 1 to
//the number of primes found.
void calculateNPrimes(int n){
    if(n == 1){
        printf("The first prime number is 2.\n");
    }else{
        int* primes = malloc(n * sizeof(int));
        int position = 0;
        int primeIterator = 2;

        while(position < n){
            if(isPrime(primeIterator) == 1){
                primes[position] = primeIterator;
                position++;
                primeIterator++;
            }else{
                primeIterator ++;
            }
        }
        
        if(n < 20){
            printf("The first %s primes are: ", convertNumber(n));

            for(int i = 0; i < n - 1; i++){
                printf("%d, ", primes[i]);
            }

            printf("%d.\n", primes[n - 1]);
        }else{
            printf("The first %d primes are: ", n);

            for(int i = 0; i < n - 1; i++){
                printf("%d, ", primes[i]);
            }

            printf("%d.\n", primes[n - 1]);
        }

        free(primes);
    }
}

//Calculates the number of primes and their identity between numbers m and n. 
//It then allocates an array that is equal to the absolute value of the difference
//between the two, then starts an iteration process similar to that of 
//calculateNPrimes(), but rather than starting at 2, it starts at the smaller of 
//m and n.
void calculatePrimesBetween(int m, int n){
    int* primes;
    if(n > m){
        primes = malloc(n - m + 2 * sizeof(int));
    }else{
        primes = malloc(m - n + 2 * sizeof(int));
    }
    int position = 0;
    int primeIterator;
    int limit;

    if(m > n){
        primeIterator = n;
        limit = m;
    }else{
        primeIterator = m;
        limit = n;
    }

    while(primeIterator <= limit){
        if(isPrime(primeIterator) == 1){
            primes[position] = primeIterator;
            position++;
            primeIterator++;
        }else{
            primeIterator++;
        }
    }
    if(position > 1){
        if(position < 20){
            printf("There are %s prime numbers between %d and %d: ", convertNumber(position), m, n);
        }else{
            printf("There are %d prime numbers between %d and %d: ", position, m, n);
        }

        for(int i = 0; i < position - 1; i++){
            printf("%d, ", primes[i]);
        }

        printf("%d.\n", primes[position - 1]);
    }else if(position == 1){
        printf("There is only one prime number between %d and %d: ", m, n);
        printf("%d.\n", primes[0]);
    }else{
        printf("There are no primes between %d and %d.\n", m, n);
    }

    free(primes);
}

//Main function, to drive the program. First, checks if there is only one argument
//(i.e., the name of the program) If so, it complains and exits. If there are only two, 
//it calls the calculateNPrimes() function, if the argument given is a strictly positive
//integer. If there are 3 args, then it calls calculatePrimesBetween(), in the order the two
//args were provided(handling for the case where m > n is done in the function). And, if argc
//is greater than 3, we report to the user that there were too MANY arguments.
int main(int argc, char* argv[]){
    if(argc == 1){
        char* exec = getExecName(argv[0]);
        printf("%s launched with no argument\n", exec);
        printf("Proper usage: %s m [n]\n", exec);
        free(exec);
    }else if(argc == 2){
        int n;
        if(sscanf(argv[1], "%d", &n) == 1 && n >= 1){
                calculateNPrimes(n);
        }else{
            char* exec = getExecName(argv[0]);
            printf("%s's argument is not a strictly positive integer.\n", exec);
            free(exec);
        }
    }else if(argc == 3){
        int m, n;
        if(sscanf(argv[1], "%d", &m) == 1 && m >= 1){
            if(sscanf(argv[2], "%d", &n) == 1 && n >= 1){
                    calculatePrimesBetween(m, n);
            }else{
                char* exec = getExecName(argv[0]);
                printf("%s's second argument is not a strictly positive integer.\n", exec);
                free(exec);
            }
        }else{
            char* exec = getExecName(argv[0]);
            printf("%s's first argument is not a strictly positive integer.\n", exec);
            free(exec);
        }
    }else if(argc > 3){
        char* exec = getExecName(argv[0]);
        printf("%s launched with too many arguments.\n", exec);
        printf("Proper usage: %s m [n]\n", exec);
        free(exec);
    }

    return 0;
}
