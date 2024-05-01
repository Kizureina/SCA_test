// https://helloacm.com/the-rdtsc-performance-timer-written-in-c/
#include <iostream>
#include <cstdlib>
#include <stdint.h>

//  Windows
#ifdef _WIN32

#include <intrin.h>
uint64_t rdtsc() {
    return __rdtsc();
}

//  Linux/GCC
#else

uint64_t rdtsc() {
    unsigned int lo, hi;
    __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

#endif

using namespace std;

#define PIN_LENGTH 10
#define TRY_TIMES 10000000

// Function to validate PIN
int validatePIN(int pin[]) {
    int correctPIN[PIN_LENGTH] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 }; // Example correct PIN
    for (int i = 0; i < PIN_LENGTH; i++) {
        if (pin[i] != correctPIN[i]) {
            return 0; // Incorrect PIN
        }
    }
    return 1; // Correct PIN
}


// Function to read PIN from user
void readPIN(int pin[]) {
    printf("Enter your PIN (4 digits): ");
    for (int i = 0; i < PIN_LENGTH; i++) {
        scanf_s("%1d", &pin[i]); // Read one digit at a time
    }
}


int main(void) {
    int pin[PIN_LENGTH];
    int correct[PIN_LENGTH] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    int incorrect[PIN_LENGTH][PIN_LENGTH] = {
        { 0, 2, 3, 4, 5, 6, 7, 8, 9, 10 },
        { 1, 0, 3, 4, 5, 6, 7, 8, 9, 10 },
        { 1, 2, 0, 4, 5, 6, 7, 8, 9, 10 },
        { 1, 2, 3, 0, 5, 6, 7, 8, 9, 10 },
        { 1, 2, 3, 4, 0, 6, 7, 8, 9, 10 },
        { 1, 2, 3, 4, 5, 0, 7, 8, 9, 10 },
        { 1, 2, 3, 4, 5, 6, 0, 8, 9, 10 },
        { 1, 2, 3, 4, 5, 6, 7, 0, 9, 10 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 0, 10 },
        { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 },
    };

    int i = 0;
    unsigned long sum = 0;

    while(i < TRY_TIMES) {
        //readPIN(pin);
        uint64_t tick = rdtsc();  // tick before
        validatePIN(correct);
        uint64_t end = rdtsc();

        sum += tick - end;
        /*
        * if (isValid) {
            cout << "PIN is correct. Time taken:" << tick - end << "cycles\n";
        }
        else {
            cout << "PIN is Incorrect. Time taken:" << tick - end << "cycles\n";
        }
        */
        i++;
    }

    cout << "验证正确时执行时间平均为：" << sum / TRY_TIMES << endl;

    
    for (size_t j = 0; j < PIN_LENGTH; j++)
    {
        i = 0, sum = 0;
        while (i < TRY_TIMES) {
            //readPIN(pin);
            uint64_t tick = rdtsc();  // tick before
            validatePIN(incorrect[j]);
            uint64_t end = rdtsc();

            sum += tick - end;
            i++;
        }

        cout << "在第" << PIN_LENGTH - j << "位验证出错时执行时间平均为：" << sum / TRY_TIMES << endl;
    }

    return 0;
}
