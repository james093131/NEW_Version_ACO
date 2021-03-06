#define dim 3
#define initial_pher 0.000167
#define Q 120.0


#include<stdio.h>
#include<fstream>
#include<iostream>
#include<sstream>
#include<stdlib.h>
#include <string.h>
#include<time.h>
#include<vector>
#include<math.h>
using namespace std;

typedef vector<char> c1d;
typedef vector<c1d> c2d;
typedef vector<c2d> c3d;
typedef vector<c3d> c4d;
typedef vector<int> i1d;
typedef vector<i1d> i2d;
typedef vector<i2d> i3d;
typedef vector<i3d> i4d;
typedef vector<double>d1d;
typedef vector<d1d> d2d;
typedef vector<d2d> d3d;
typedef vector<d3d> d4d;


class ACO{
    public:
        d1d EACH_RUN_FIT; 
        i2d ALL_RUN_BEST_PATH;
        i1d RUN_BEST_PATH;
        void RUN_ACO_PR(int ant,const char *F,double alpha ,double beta,double decline,int iteration,int run,int PR_NUM,double PR_LOCK_LIMIT)
        {
            int i=0;
            double START;
            double END;
            EACH_RUN_FIT.resize(run);
            START = clock();
            while(i<run)
            {
                ACO_PR(ant,F,alpha,beta,decline,iteration,PR_NUM,PR_LOCK_LIMIT);
                EACH_RUN_FIT[i] = EACH_RUN_BEST_FIT; 
                ALL_RUN_BEST_PATH.push_back(EACH_RUN_BEST_PATH);
                cout<<"RUN"<<i+1<<' '<<EACH_RUN_FIT[i]<<endl;
                i++;
            }
            END = clock();
            int ind = -1;
            double AVG_FIT = HANDLE_RUN(run,ind);
            finaloutput(ant,run,iteration,alpha,beta,decline,AVG_FIT,ind,START,END,PR_NUM,PR_LOCK_LIMIT);
        }
    
    private:

        int Best_ANT;
        double Best_Fitness;
        d2d distancetable;
        d2d Phermone_Table;
        i2d Lock_Table;
        i2d city;
        i2d ANT_PATH;
        d1d Ant_Fitness;
        i1d EACH_RUN_BEST_PATH;
        i2d PR_TABLE;
        double EACH_RUN_BEST_FIT ;
        i2d PR_LOCK;
        int PR_LOCK_QUAN;

    private:
        void ACO_PR(int ant,const char *F,double alpha ,double beta,double decline,int iteration,int PR_NUM,double PR_LOCK_LIMIT)
        {
            
            ini(ant,F);
            int ITER = 0;
            while (ITER < iteration)
            {
                Transition(ant,alpha,beta);
                evaluate();
                phermoneupdate(ant,decline,PR_NUM,PR_LOCK_LIMIT);
                cout<<ITER<<' '<<EACH_RUN_BEST_FIT<<endl;
                if(EACH_RUN_BEST_FIT > Best_Fitness)
                {
                    EACH_RUN_BEST_FIT = Best_Fitness;
                    EACH_RUN_BEST_PATH.assign(ANT_PATH[Best_ANT].begin(),ANT_PATH[Best_ANT].end());
                    BEST_phermoneupdate(EACH_RUN_BEST_PATH,EACH_RUN_BEST_FIT,decline);//更新最佳的螞蟻費洛蒙
                }
                BEST_phermoneupdate(ANT_PATH[Best_ANT],Best_Fitness,decline);//更新當前iteration最佳的螞蟻費洛蒙
                ITER++;
            }
            print(PR_LOCK);
        }
        int* read(int &sum,const char *F){//讀檔

            fstream file;
            int ind=0;
            file.open(F,ios::in);
            int* temp=new int[8192];
            while(file)
            {
                file>>temp[ind];
                ind++;
            }
            file.close();
            sum=ind-1;
            return temp;
        }
        void makearr(i2d &arr,int *input,int len)//將測資讀為整理好的陣列
        {
            int chc=0;
            for(int i=0;i<len;i++)
            {
                for(int j=0;j<dim;j++)
                {
                    arr[i][j] = *(input+chc);
                    //cout<<*((int*)arr +dim*i+j)<<' ';
                    chc++;
                }
                //cout<<endl<<endl;
            }
        }
        double distance_calculate(int x1 ,int y1,int x2,int y2){//計算兩點的距離
            double dis;
            dis=pow(x2-x1,2)+pow(y2-y1,2);
            dis=sqrt(dis);
            return dis;
        }
        void distancecal(int cityquan)
        {
            for(int i=0;i<cityquan;i++)
            {
                for(int j=0;j<cityquan;j++)
                {
                    if(i<=j)
                        distancetable[i][j] = distance_calculate(city[i][1],city[i][2],city[j][1],city[j][2]);
                        distancetable[j][i] = distancetable[i][j];
                }
            }
        }
        void print(i2d X)
        {
            for(int i=0;i<X.size();i++)
            {
                for(int j=0;j<X[i].size();j++)
                {
                    cout<<X[i][j]<<' ';
                }
                cout<<endl;
            }
        }
        void print(d2d X)
        {
            for(int i=0;i<X.size();i++)
            {
                for(int j=0;j<X[i].size();j++)
                {
                    cout<<X[i][j]<<' ';
                }
                cout<<endl;
            }
        }
        void ini(int ant,const char *F)
        {
            int *a;
            int templen;
            a = read(templen,F); //讀檔用的pointer

            int len = templen / dim;
            city.assign(len, i1d(dim, 0));
            distancetable.assign(len,d1d(len,0));//儲存距離表
            makearr(city, a, len);
            distancecal(len); //製作距離表
            Lock_Table.assign(city.size(), i1d(city.size(), 0));
            Phermone_Table.assign(city.size(), d1d(city.size(),initial_pher));
            PR_TABLE.assign(city.size(), i1d(city.size(),0));
            ANT_PATH.assign(ant,i1d(city.size()+1,0));
            Ant_Fitness.resize(ant);
            EACH_RUN_BEST_FIT = 10000000;
            EACH_RUN_BEST_PATH.resize(city.size()+1);
            PR_LOCK_QUAN = 0;
            PR_LOCK.clear();
        }    
        void evaluate()//評估所有螞蟻適應度
        {
            Best_Fitness=1000000;
            for(int i=0;i<ANT_PATH.size();i++)
            {
                double temp=0;
                for(int j=0;j<ANT_PATH[i].size()-1;j++)
                {
                int Start = ANT_PATH[i][j];
                int End = ANT_PATH[i][j+1];
                temp += distancetable[Start-1][End-1];
                }
                // cout<<temp;
                Ant_Fitness[i] = temp;
                if(Ant_Fitness[i] < Best_Fitness)
                    {
                        Best_ANT = i;
                        Best_Fitness = Ant_Fitness[i];
                    }
            }

        }
        void phermoneupdate(int ant,double decline,int PR_NUM,double PR_LOCK_LIMIT){
            for(int i=0;i<ANT_PATH.size();i++)
            {
                for(int j=0;j<ANT_PATH[i].size()-1;j++)
                {
                    int k = ANT_PATH[i][j]-1;
                    int y = ANT_PATH[i][j+1]-1;
                    if(k<y)
                    {
                        Phermone_Table[k][y] = Phermone_Table[k][y]*(double)(1-decline)+decline*(double)Q/Ant_Fitness[i];
                        Phermone_Table[y][k] = Phermone_Table[k][y];
                        if(Phermone_Table[k][y] < initial_pher){
                            Phermone_Table[k][y] = initial_pher;
                            Phermone_Table[y][k] = initial_pher;
                        }
                    }
                    PR_TABLE[k][y] ++;
                    if(PR_TABLE[k][y] == PR_NUM*ant && PR_LOCK_QUAN < PR_LOCK_LIMIT*city.size()*2)
                    {
                        i1d arr;
                        arr.push_back(k);
                        arr.push_back(y);
                        PR_LOCK.push_back(arr);
                        PR_LOCK_QUAN ++;
                    }
                   
                }
                
            }
        }
        void BEST_phermoneupdate(i1d PATH,double FIT,double decline){
                for(int j=0;j<PATH.size()-1;j++)
                {
                    int k= PATH[j]-1;
                    int y=PATH[j+1]-1;
                    Phermone_Table[k][y] = Phermone_Table[k][y]*(1-decline)+(double)Q/FIT;
                    Phermone_Table[y][k]= Phermone_Table[k][y];
                    if(Phermone_Table[k][y] < initial_pher){
                            Phermone_Table[k][y]= initial_pher;
                            Phermone_Table[y][k] = initial_pher;
                        }
                }
                
            }
        void Transition(int pop,double alpha,double beta)
        {
            int i=0;
            while(i<pop)
            {
                i1d check(city.size(),-1);
                int ini = 1;
                check[ini-1] = 1;
                ANT_PATH[i][0] = ini;
                int j=1;
                int k =ini;
                double NEW_ANT_FIT = 0;
                while(j<city.size())
                {
                    k = solutionconstruct(k,check,alpha,beta);
                    int C = PR_CHECK(k-1);
                    if(C != -1 && checkpathornot(check,C,city.size()))
                    {
                        check[k-1] = 1;
                        check[C] = 1;
                        ANT_PATH[i][j] = k ; 
                        NEW_ANT_FIT += distancetable[ANT_PATH[i][j-1]-1][k-1];
                        
                        ANT_PATH[i][j+1] = C+1 ; 
                        NEW_ANT_FIT += distancetable[ANT_PATH[i][j]-1][C];
                        j += 2;

                    }
                    else{
                        check[k-1] = 1;
                        ANT_PATH[i][j] = k ; 
                        NEW_ANT_FIT += distancetable[ANT_PATH[i][j-1]-1][k-1];
                        j++;
                    }
                    
                }
                NEW_ANT_FIT += distancetable[k-1][ini-1];
                ANT_PATH[i][city.size()] = ANT_PATH[i][0]; 
                Ant_Fitness[i] = NEW_ANT_FIT;
                i++;
            }
        }
        int PR_CHECK(int k)
        {
            for(int i=0;i<PR_LOCK.size();i++)
            {
                if(PR_LOCK[i][0] == k)
                {
                    return PR_LOCK[i][1];
                }
            }
            return -1;
        }
        bool checkpathornot(i1d arr,int loc,int len){//檢查是否有走過，走過為1沒走過為-1
            if(arr[loc]!=1)
                {
                    return true;
                }
            else
                return false;
        }
        int solutionconstruct(int ini,i1d check,double alpha,double beta){//用輪盤的方式找下一點
            double sum = 0;
            d1d temp(city.size(),-1);
            for(int i=0;i<city.size();i++)
            {
                if(i != ini-1 && checkpathornot(check,i,city.size())){
                    double dis = distancetable[ini-1][i];//計算兩點的距離
                    temp[i] = sum + pow(Phermone_Table[ini-1][i],alpha)*pow(1/dis,beta);//用疊加的方式來進行方便後續機率切割作業
                    sum = temp[i];
                }
                else
                    temp[i] = -10000;
            }
            for(int i=0;i<city.size();i++)
            {
                temp[i] = temp[i]/sum;//變成機率
                // cout<<temp[i]<<' ';
            }
            // cout<<endl;

            double x = (1 - 0) * rand() / (RAND_MAX + 1.0) + 0;
            //cout<<'x'<<' '<<x<<endl;
            int choice = -1;
            for(int i=0;i<city.size();i++)
            {
                if(x < temp[i]){
                    choice = i+1;
                    break;
                }
            }
            return choice;
        }
        double HANDLE_RUN(int run,int &ind)
        {
            double sum = 0;
            double BESTONE = 10000000;
            for(int i=0;i<run;i++)
            {
                sum += EACH_RUN_FIT[i];
                if(EACH_RUN_FIT[i] < BESTONE)
                {
                    BESTONE = EACH_RUN_FIT[i];
                    ind = i;
                }
            }
            sum = sum / run;
            RUN_BEST_PATH.assign(ALL_RUN_BEST_PATH[ind].begin(),ALL_RUN_BEST_PATH[ind].end());
            return sum;
        }
        void finaloutput(int ant,int run,int iteration,double alpha,double beta,double decline,double AVG_FIT,int ind,double START,double END,int PR_NUM ,double PR_LIMIT_LOCK)
        {   
            fstream file;//寫檔
	        file.open("PR_ACO.txt",ios::app);
            file<<"Population : "<<ant<<endl;
            file<<"Iteration : "<<iteration<<endl;
            file<<"Alpha : "<<alpha<<endl;
            file<<"Beta : "<<beta<<endl;
            file<<"Decline : "<<decline<<endl;
            file<<"City size : "<<city.size()<<endl;
            file<<"Run :"<<run<<endl;
            file<<"PR_NUM : "<<PR_NUM<<endl;
            file<<"PR LOCK LIMIT : "<<PR_LIMIT_LOCK*city.size()<<endl;
            file<<"Execution Time :"<<(END - START) / CLOCKS_PER_SEC<<"(s)"<<endl;
            file<<"Average Optimum : "<<AVG_FIT<<endl;
            file<<"Best Optimum : "<<EACH_RUN_FIT[ind]<<endl;
            file<<"Best Path : "<<endl;
            
            for(int i=0;i<RUN_BEST_PATH.size();i++)
            {
                file<<RUN_BEST_PATH[i]<<' ';
            }
            file<<endl<<endl;
            cout<<"Average Optimum : "<<AVG_FIT<<endl;
            cout<<"Execution Time :"<<(END - START) / CLOCKS_PER_SEC<<"(s)"<<endl;
        }
};
