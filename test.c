#include "proj3.h"

int main() {
	simclock temp1;
	simclock temp2;

	temp1.sec = 4;
	temp1.nano = 4;
	temp2.sec = 2;
	temp2.nano = 2;
	
	printf("difference is %d\n", simdiff(temp1, temp2));

	return 0;
}
