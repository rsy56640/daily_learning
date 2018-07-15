#include <stdio.h>
//This program is to test the ending of the architecture
//either Big-ending or Little-ending.
union T
{
	char c[4];
	int i;
};
void test_ending()
{
	union T t;
	t.i = 0x12345678;
	int i=0;
	for(; i < 4; i++)	
		printf("%d\t", t.c[i]);//output 0x78 0x56 0x34 0x12, so my processor is Little-ending
	printf("\n");
}


//This program is to test type-cast in P188 in <<CSAPP>>(Cn 3rd Edition)
typedef unsigned long long ull;
ull double2ul(double d)
{
	union{
		double d;
		ull ul;
	} temp;
	temp.d = d;
	return temp.ul;	
}

void test_cast()
{
	double d = 15.5;
	ull ul1 = (ull) d;
	ull ul2 = double2ul(d);
	printf("%llu, %llu\n", ul1, ul2);
	//15.5 = 0 10000000010 111100...0  double 1-11-52
	ul2 -= (((ull)1)<<62) + (((ull)1)<<53) + (((ull)1)<<52) - (((ull)1)<<48);
	printf("%llu, %llu\n", ul1, ul2);
}

int main(int argc, char* argv[])
{
	test_ending();
	test_cast();
	return 0;
}
