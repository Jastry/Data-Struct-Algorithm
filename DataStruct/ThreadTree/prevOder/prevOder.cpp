#include "prevOder.h"

void test(){
    int a[] = {1,2,3,'#','#',4,'#','#',5,6};
    int b[] = {1,2,4,'#','#',5,8,'#',9,10,'#','#','#','#',3,6,'#',11,'#',12,13,'#',14,'#','#','#',7};
    
    __BinaryTreeThread<int> t1(a,sizeof(a)/sizeof(a[0]),'#');
    __BinaryTreeThread<int> t2(b,sizeof(b)/sizeof(b[0]),'#');
	t1.prevOderThread();
	t2.prevOderThread();
    __BinaryTreeThread<int>::prevOderIterator it1 = t1.prevOderbegin();
    for (; it1 != t1.prevOderend(); ++it1){
        std::cout<<*it1<<" ";
    }

    std::cout<<std::endl;

    __BinaryTreeThread<int>::prevOderIterator it2 = t2.prevOderbegin();
    for (; it2 != t2.prevOderend(); ++it2){
        std::cout<<*it2<<" ";
    }
    std::cout<<std::endl;
}

int main(){
	test();
	return 0;
}
