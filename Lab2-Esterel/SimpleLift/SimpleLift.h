#ifndef SIMPLELIFT_DATA_H
#define SIMPLELIFT_DATA_H

#define N 4
#define SIZE 100
#define UNDEFDATA -100

typedef struct {
    char array[N];
} BoolArray;

void ResolvePriority(int*, BoolArray, int, int);
void send(int);
void recv(int*);
void add(int);
int remov(void);
int orArray(BoolArray);
BoolArray orArrays2(BoolArray, BoolArray);
BoolArray orArrays3(BoolArray, BoolArray, BoolArray);
void _BoolArray(BoolArray*, BoolArray);
int _eq_BoolArray(BoolArray, BoolArray);
char* _BoolArray_to_text(BoolArray);
void _text_to_BoolArray(BoolArray*, char*);
int _check_BoolArray(char*);
void clearBit(BoolArray*, int);

#endif
