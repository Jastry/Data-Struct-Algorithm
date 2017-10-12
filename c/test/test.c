#include <stdio.h>

int foo(int a, int b)
{
	int k;
	for (int c = a ^ b; c != 0; c = c >> 1){
		k += c & 1;
	}
	return k;
}

int fun(int x)
{
	int r = 0;
	int t = x;
	t = t > 0 ? t : -t;
	for (; t; t/=10){
		r = r * 10 + t % 10;
	}
	return x > 0 ? r : -r;
}

int f(int x){
	if (x <= 0) return 0;
	f(x - 1);
	f(x - 3);
	printf("%d", x);
	return 0;
}
int main()
{
	//printf("%d\n",foo(1024, 4096));
	//printf("%d\n", fun(-12345678));
	f(5);
	return 0;
}
