#include "sort.h"

int main()
{
    int a[] = {9, 0, 6, 7, 3, 1, 8, 4, 2, 5};
    //int a[5000];
    //srand((unsigned)time(NULL));
    //for (int i = 0; i < 5000; ++i) {
    //    a[i] = rand() % 13131;
    //}
    //InsertSort2(a, sizeof (a) / sizeof (int));
    //ShellSort(a, sizeof (a) / sizeof (int));
    //SelectSort(a, sizeof (a) / sizeof (int));
    //HeapSort(a, sizeof (a) / sizeof (int)) ;   
    //BubbleSort(a, sizeof (a) / sizeof (int)) ;   
    //QuickSortNonR(a, 0, sizeof (a) / sizeof (int) - 1);
    //QuickSort(a, 0, sizeof (a) / sizeof (int) - 1)
    CountSort(a, sizeof (a) / sizeof (int));
    IsSorted(a, sizeof (a) / sizeof (int));
    return 0;
}
