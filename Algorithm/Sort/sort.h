#ifndef __SORT_H__
#define __SORT_H__

#include <iostream>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <stack>

bool IsSorted(const int *a, size_t sz){
	assert(a);
	for (size_t i = 0; i < sz - 2; ++i){
		if (a[i] > a[i + 1]){
			std::cout << "a[i] = " << a[i] << " a[i + 1] = " << a[i + 1] << std::endl;
			return false;
		}
	}
    for (size_t i = 0; i < sz; ++i) {
        std::cout << a[i] << " ";
    }
    std::cout << std::endl;
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

static void AdjustDown(int *a, size_t parent, size_t n)
{
    assert(a);
    size_t child = parent * 2 + 1;
    while (child < n) {
        if (child + 1 < n && a[child] < a[child + 1]) {
            child++;
        }
        if (a[parent] < a[child]) {
            _Swap(&a[parent], &a[child]);
            parent = child;
            child = parent * 2 + 1;
        } else {
            break;
        }
    }
}


void HeapSort(int *a, size_t sz)
{
    assert(a);
    for (int i = ((sz - 2) >> 1); i >= 0; --i) {
        //n - 2 / 2 find last parent node -- (10 -> 4)
        AdjustDown(a, i, sz);
    }
    int end = sz - 1;
    while (end > 0){
        _Swap(&a[0], &a[end]);
       // AdjustDown(a, 0, end--);
        AdjustDown(a, 0, --end);

    }
}

void BubbleSort(int *a, size_t n)
{
    assert(a);
    int end = n;
    bool exchange = false;
    while (end-1 > 0) {
        for (int i = 1; i < end; ++i) {
            if (a[i-1] > a[i]) {
                _Swap(&a[i-1], &a[i]);
                exchange = true;
            }
        }
        if (!exchange) {
            break;
        }
        --end;
    }
}

static size_t GetMiddleIndex(int *a, size_t left, size_t right)
{
    assert(a);
    int mid = left + ((right - left) >> 1);

    if (a[mid] <= a[right]) {
        if (a[mid] >= a[left]) {
            return mid;
        } else if (a[left] <= a[right]) {
            return left;
        } else 
            return right;
    } else {
        if (a[right] >= a[left]) {
            return right;
        } else if (a[left] >= a[mid]) {
            return a[mid];
        } else 
           return left;
    }
}

//zuo you zhi zhen fa
static size_t PartSort1(int *a, size_t left, size_t right)
{
    assert(a);
    int mid = GetMiddleIndex(a, left, right);
    _Swap(&a[right], &a[mid]);
    int key = a[right];
    int begin = left;
    int end = right;
    while (begin < end) {
        while (begin < end && a[begin] <= key) {
            ++begin;
        }
        while (begin < end && a[end] >= key) {
            --end;
        }
        if (begin < end) {
            _Swap(&a[begin], &a[end]);
        }
    }
    _Swap(&a[begin], &a[right]);
    return begin;
}

#if 0
size_t PartSort1(int* a, size_t left, size_t right)
{
    assert(a);
    int key = a[right];
    int begin = left;
    int end = right;
    while (begin < end) {
        while (begin < end && a[begin] <= key)
            ++begin;
        while (begin < end && a[end] >= key)
            --end;
        if (begin < end) {
            swap(a[begin], a[end]);
        }
    }
    swap(a[begin], a[end]);
    return begin;
}
#endif


//wa keng fa
static size_t PartSort2(int *a, size_t left, size_t right)
{
    assert(a);
    int mid = GetMiddleIndex(a, left, right);
    _Swap(&a[mid], &a[right]);

    int key = a[right];
    while (left < right) {
        while (left < right && a[left] < key) {
            ++left;
        }
        a[right] = a[left];
        
        while (left < right && a[right] > key) {
            --right;
        }
        a[left] = a[right];
    }
    a[left] = key;
    return left;
}

//qian hou zhi zhen fa
static size_t PartSort3(int *a, size_t left, size_t right)
{
    assert(a);
    int prev = left - 1;
    int cur = left;
    int mid = GetMiddleIndex(a, left, right);
    _Swap(&a[mid], &a[right]);
    int key = a[right];
    while (cur < right) {
        while (a[cur] < key && ++prev != cur) {
            _Swap(&a[prev], &a[cur]);
        }
        ++cur;
    }
    _Swap(&a[++prev], &a[right]);
    return prev;
}

void QuickSortNonR(int *a, size_t left, size_t right)
{
    //w
    assert(a);
    std::stack<int> s;
    if (left < right) {
        s.push(right);
        s.push(left);
    }
    while ( !s.empty() ) {
        int begin = s.top();
        s.pop();
        int end = s.top();
        s.pop();
        
        //int div = PartSort1(a, begin, end);
        //int div = PartSort2(a, begin, end);
        int div = PartSort3(a, begin, end);
        if (begin < div-1) {
            s.push(div-1);
            s.push(begin);
        }

        if (div+1 < end) {
            s.push(end);
            s.push(div+1);
        }
    }
}

void QuickSort(int *a, size_t left, size_t right)
{
    assert(a);
    if (right - left < 16) {
        InsertSort(a, right - left + 1);
        return;
    }

    int div = PartSort1(a, left, right);
    QuickSort(a, left, div - 1);
    QuickSort(a, div + 1, right);

}
#if 0
void QuickSort(int* a, size_t left, size_t right)
{
    assert(a);
    if (right - left < 16){
        InsertSort(a, right - left + 1);
        return;
    }

    int div = PartSort1(a, left, right);
    QuickSort(a, left, div - 1);
    QuickSort(a, div + 1, right);
}
#endif
void CountSort(int *a, size_t n)
{
    assert(a);
    int min = a[0];
    int max = a[0];

    for (size_t i = 0; i < n; ++i) {
        if (a[i] > max) {
            max = a[i];
        }
        if (a[i] < min) {
            min = a[i];
        }
    }
    int range = max - min + 1;
    int *tamp = new int[range]();
    for (size_t i = 0; i < n; ++i) {
        tamp[a[i] - min]++;
    }
    int index = 0;
    for (size_t i = 0; i < range; ++i) {
        while (tamp[i]--) {
            a[index++] = i + min;
        }
    }
    delete[] tamp;
}


void BaseSort(int *a, size_t n)
{
    assert(a);

}

//void MergeSort(int *a, size_t n)

#endif //__SORT_H__
