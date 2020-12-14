#include "normal.hpp"
int main(int argc, char *argv[])
{
    srand(time(NULL));
    int ant = atoi(argv[1]);
	int iteration = atoi(argv[2]);
    int run = atoi(argv[3]);
    double alpha = atof(argv[4]);
	double beta = atof(argv[5]);
    double decline = atof(argv[6]);
    char * F=argv[7];
    ACO ACO_PR;
    ACO_PR.RUN_ACO_PR(ant,F,alpha,beta,decline,iteration,run);
    // if (argc > 4)
    // {
    // }
    // else
    // {
        
    // }    
   
}