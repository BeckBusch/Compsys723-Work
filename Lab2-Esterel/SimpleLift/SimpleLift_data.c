#include "SimpleLift.h"

#include <stdio.h>
#include <stdlib.h>

#define xil_printf printf
#define print printf
// #define xil_printf(...)
// #define print(...)

int cq[SIZE];
int front = 0;
int rear = 0;
int cnt = 0;

#if N == 4
BoolArray initialBoolArray = {"\0\0\0\0"};
#else
#  error SimpleLift_data.c Line 17: initialBoolArray needs to be adjusted
#endif

void send(int data)
{
    add(data);
    xil_printf("Added data %d\n", data);
}

void recv(int *data)
{
    int tmp;
    tmp = remov();
    if (tmp != -1)
        *data = tmp;
    else
        *data = UNDEFDATA;
    xil_printf("Removed data %d\n", *data);
}

void add(int data)
{
    if (cnt == SIZE)
        print("Queue Full... \n");
    else {
        cq[front] = data;
        front = (front + 1) % SIZE;
        ++cnt;
    }
}

int remov(void)
{
    int val;
    if (cnt == 0) {
        print("Queue Empty... Exiting \n");
        return -1;
    } else {
        val = cq[rear];
        rear = (rear+1) % SIZE;
        cnt--;
        return val;
    }
}


void ResolvePriority(int *priority, BoolArray reqArray, int direction, int current)
{
    int bitmask;
    int PendingReq = 0;
    int i = 0;
    for (i = 0; i < N; ++i) {
        PendingReq = PendingReq | (reqArray.array[i] << i);
    }
    xil_printf("Pending Request, direction, current: %d %d %d\n", PendingReq, direction, current);
    // initial condition => priority hasn't been changed
    *priority = -1;
    switch (direction) {
    case 1:
        bitmask = 0x01;
        for (i = 0 ; i < N; ++i) {
            if (i > current && (PendingReq & bitmask)) {
                *priority = i;
                xil_printf("Up Priority = %d\n", *priority);
                break;
            }
            bitmask = bitmask << 1;
        }
        break;
    case 2:
        bitmask = 0x08;
        for (i = N - 1; i > -1; --i) {
            if (i < current && (PendingReq & bitmask)) {
                *priority = i;
                xil_printf("Down Priority = %d\n", *priority);
                break;
            }
            bitmask = bitmask >> 1;
        }
        break;
    case 3:
        xil_printf("Current %d \n", current);
        if (current == 0) {
            print("Idle case when on bottom floor\n");
            bitmask = 0x01;
            for (i = 0; i < N; ++i) {
                if (i > current && (PendingReq & bitmask)) {
                    *priority = i;
                    xil_printf("Init Up Priority = %d\n", *priority);
                    break;
                }
                bitmask = bitmask << 1;
            }
        } else if (current == N - 1) {
            print("Idle case when on top floor\n");
            bitmask = 0x08;
            for (i = N - 1; i > -1; --i) {
                if (i < current && (PendingReq & bitmask)) {
                    *priority = i;
                    xil_printf("Init Down Priority= %d\n", *priority);
                    break;
                }
                bitmask = bitmask >> 1;
            }
        } else {
            //if lift is somewhere in the middle it will go down if
            //there is any requests below the current floor
            print("Idle case when on some middle floor\n");
            bitmask = 0x01;
            for (i = 0; i < N; ++i) {
                if (i > current && (PendingReq & bitmask)) {
                    *priority = i;
                    xil_printf("Middle Up Priority = %d\n", *priority);
                    break;
                }
                bitmask = bitmask << 1;
            }
            // if there are no up requests > current
            if (*priority == -1) {
                bitmask = 0x08;
                for(i = N - 1; i > -1; --i) {
                    if (i < current && (PendingReq & bitmask)) {
                        *priority = i;
                        xil_printf("Middle Down Priority = %d\n", *priority);
                        break;
                    }
                    bitmask = bitmask >> 1;
                }
            }
        }
        break;
    }
}

int orArray(BoolArray array)
{
    int i, result = 0;
    for (i = 0; i < N && !result; ++i) {
        result = result || array.array[i];
    }
    return result;
}

BoolArray orArrays2(BoolArray array1, BoolArray array2)
{
    int i;
    BoolArray result;
    for (i = 0; i < N; ++i) {
        result.array[i] = array1.array[i] || array2.array[i];
    }
    return result;
}

BoolArray orArrays3(BoolArray array1, BoolArray array2, BoolArray array3)
{
    int i;
    BoolArray result;
    for (i = 0; i < N; ++i) {
        result.array[i] = array1.array[i] || array2.array[i] || array3.array[i];
    }
    return result;
}

void _BoolArray(BoolArray *lhs, BoolArray rhs)
{
    *lhs = rhs;
}

int _eq_BoolArray(BoolArray lhs, BoolArray rhs)
{
    int i, eq = 1;
    for (i = 0; i < N; ++i) {
        if (lhs.array[i] != rhs.array[i]) {
            eq = 0;
            break;
        }
    }
    return eq;
}

char* _BoolArray_to_text(BoolArray array)
{
    int i;
    static char bool[N + 1];
    for (i = 0; i < N; ++i) {
        bool[i] = array.array[i] ? '1' : '0';
    }
    bool[i] = 0;
    return bool;
}

void _text_to_BoolArray(BoolArray *array, char *text)
{
    int i;
    for (i = 0; i < N; ++i) {
        array->array[i] = text[i] == '1' ? 1 : 0;
    }
}

int _check_BoolArray(char *text)
{
    int i;
    for (i = 0; i < N; ++i) {
        if (text[i] != '0' || text[i] != '1')
            return 1;
    }
    return 0;
}

void clearBit(BoolArray* array, int i)
{
    array->array[i] = 0;
}
