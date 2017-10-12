#include "InOderThread.h"

void test(){
    int a[] = {1,2,3,'#','#',4,'#','#',5,6};
    int b[] = {1,2,4,'#','#',5,8,'#',9,10,'#','#','#','#',3,6,'#',11,'#',12,13,'#',14,'#','#','#',7};
    
    __BinaryTreeThread<int> t1(a,sizeof(a)/sizeof(a[0]),'#');
    __BinaryTreeThread<int> t2(b,sizeof(b)/sizeof(b[0]),'#');
	t1.InOderThread();
	t2.InOderThread();
    __BinaryTreeThread<int>::Iterator it1 = t1.begin();
    for (; it1 != t1.end(); ++it1){
        std::cout<<*it1<<" ";
    }

    std::cout<<std::endl;

    __BinaryTreeThread<int>::Iterator it2 = t2.begin();
    for (; it2 != t2.end(); ++it2){
        std::cout<<*it2<<" ";
    }
    std::cout<<std::endl;
}

int main(){
	test();
	return 0;
}
