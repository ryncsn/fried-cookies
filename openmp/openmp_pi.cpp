#include <iostream>
#include <math.h>
#include <stdio.h>
#include <sys/time.h>
using namespace::std;
int main(int argc, char *argv[])
{
    int stepi;
    int i;

    struct timeval start, end;

    gettimeofday(&start, NULL);

    std::cout << "Step:" << std::endl;
    std::cin >> stepi;

    double step = 1.0/((double)stepi);
    double ans = 0;
    double d,dt; 

    if(step == 0){
	std::cout << "Step too large." << std::endl;
	return 1;
    }

    int limit = 2.0/step;

    d=-1.0;

#pragma omp for
	for(i=0;i<limit;i++){
		dt=(sqrt(1-pow(d+step*(double)i,2))+sqrt(1-pow(d+((double)i+1)*step,2)))*step;
#pragma omp critical
		ans = ans + dt;
	}

    gettimeofday(&end, NULL);
    
    long useconds = end.tv_usec - start.tv_usec;
    long seconds  = end.tv_sec  - start.tv_sec;
    long mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;

    printf("%.12lf\nTime: %ld ms\n",ans,mtime);

    return 0;
}
