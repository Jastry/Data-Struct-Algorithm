#include "sort.h"

int main()
{
    int a[5000];
    srand((unsigned)time(NULL));
    for (int i = 0; i < 5000; ++i) {
        a[i] = rand() % 13131;
    }
    InsertSort2(a, sizeof (a) / sizeof (int));
    //ShellSort(a, sizeof (a) / sizeof (int));
    //SelectSort(a, sizeof (a) / sizeof (int));
//    for (size_t i = 0; i < 100; ++i) {
//        std::cout << a[i] << " ";
//    }
//    std::cout << std::endl;
    //HeapSort(a, sizeof (a) / sizeof (int)) ;   
    IsSorted(a, sizeof (a) / sizeof (int));
    for (int i = 0 ; i < 1000; ++i) {
        std::cout << a[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}
