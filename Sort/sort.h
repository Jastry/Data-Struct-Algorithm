#ifndef __SORT_H__
#define __SORT_H__

#include <iostream>
#include <assert.h>
#include <time.h>
#include <stdlib.h>


bool IsSorted(const int *a, size_t sz){
	assert(a);
	for (size_t i = 0; i < sz - 2; ++i){
		if (a[i] > a[i + 1]){
			std::cout << "a[i] = " << a[i] << " a[i + 1] = " << a[i + 1] << std::endl;
			return false;
		}
	}
	return true;
}

static void _Swap(int *left, int *right)
{
    assert(left);
    assert(right);
    int tamp = *left;
    *left = *right;
    *right = tamp;
}
void InsertSort(int* a, size_t sz)
{
    assert(a);
    for (size_t i = 0; i < sz - 1; ++i){
        int end = i;
        int tamp = a[end + 1];
        while (end >= 0){
            if (a[end] > tamp){
                a[end + 1] = a[end];
                end--;
            } else {
                break;
            }
        }
        a[end + 1] = tamp;
    }
}

void ShellSort(int *a, size_t sz)
{
    assert(a);
    int gap = sz;//bi jiao jian ge
    while (gap > 1) {
        gap = gap / 3 + 1;
        for (size_t i = 0; i < sz - gap; ++i){
            int end = i;
            int tamp = a[end + gap];
            while (end >= 0) {
                if (a[end] > tamp) {
                    a[end + gap] = a[end];
                    end -= gap;
                } else
                    break;
            }
            a[end + gap] = tamp;
        }
    }
}

void SelectSort(int *a, size_t sz)
{
    assert(a);
    size_t left = 0; size_t right = sz - 1;
    while (left < right) {
        size_t max = left;
        size_t min = left;
        for (size_t i = left; i <= right; ++i) {
            if (a[max] < a[i]) {
                max = i;
            }
            if (a[min] > a[i]) {
                min = i;
            }
        }
        _Swap(&a[min], &a[left]);
        if (max == left) {
            max = min;
        }
        _Swap(&a[max], &a[right]);
        --right;
        ++left;
    }
}

static void InsertSort2(int *a, size_t sz)
{
    assert(a);
    for (size_t i = 0; i < sz - 1; ++i){
        int end = i;
        int tmp = a[end + 1];
        while (end >= 0) {
            if (a[end] > tmp) {
                a[end + 1] = a[end];
                end--;
            } else {
                break;
            }
        }
        a[end + 1] = tmp;
    }
}
#endif //__SORT_H__
