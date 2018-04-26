#include <iostream>
#include "semaphore.h"

using namespace std;
using namespace RSY_TOOL::OS_Experiment;

int main()
{

	Semaphore<int, int> s(5);

	shared_ptr<int> pi(new int(4));

	s.wait(pi);

	s.signal();


	system("pause");
	return 0;
}