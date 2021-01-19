#include "normal.hpp"
int main(int argc,const char *argv[])
{
    srand(time(NULL));
    int ant = atoi(argv[1]);
	int iteration = atoi(argv[2]);
    int run = atoi(argv[3]);
    double alpha = atof(argv[4]);
	double beta = atof(argv[5]);
    double decline = atof(argv[6]);
    int PR_NUM = atoi(argv[7]);
    double PR_LOCK_LIMIT = atof(argv[8]);
    const char * F = argv[9];

    if (argc > 1)
    {
        ACO ACO_PR;
        ACO_PR.RUN_ACO_PR(ant,F,alpha,beta,decline,iteration,run,PR_NUM,PR_LOCK_LIMIT);
    }
    else
    {
        ACO ACO_PR;
        const char * K = "readfile.txt";
        ACO_PR.RUN_ACO_PR(51,K,3.0,9.0,0.185,1000,1,50,0.45);
    }    
   
}