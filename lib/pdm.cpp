// g++ pdmod4.cpp 
// ./a.out        

#include <iostream>
#include <math.h>
#include <cstring>
#include <cmath>
#include <time.h>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <ratio>
#include <chrono>

#define PI 3.14159265359

using namespace std;

int main() {

	using namespace std::chrono;
	
	double total = 0.0;
    
    int M = 0, Q = 0, pb = 0, rb = 0;
    
    // pattern bits
    Q = 2;
    pb = pow(2,Q);

    // rotation bits
    M = 11;
    rb = pow(2,M);
    
    double p[4][23] = {{0, 0, 0, PI, 0, PI, PI, 0, PI, PI, PI, 0, 0, 0, 0, PI, 0, PI, PI, 0, PI, PI, PI}, 
    {PI/2, PI/2, PI/2, -PI/2, PI/2, 0, -PI/2, PI/2, -PI/2, -PI/2, -PI/2, 0, PI/2, PI/2, PI/2, -PI/2, PI/2, 0, -PI/2, PI/2, -PI/2, -PI/2, -PI/2},
    {PI, PI, PI, 0, PI, PI, 0, PI, 0, 0, 0, 0, PI, PI, PI, 0, PI, PI, 0, PI, 0, 0, 0},
    {-PI/2, -PI/2, -PI/2, PI/2, -PI/2, 0, PI/2, -PI/2, PI/2, PI/2, PI/2, 0, -PI/2, -PI/2, -PI/2, PI/2, -PI/2, 0, PI/2, -PI/2, PI/2, PI/2, PI/2}};

	double r[4][24] = {{PI/4, PI/4, PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, PI/4, PI/4, PI/4, PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, PI/4},
	{PI/4, 3*PI/4, -3*PI/4, -PI/4, -3*PI/4, -PI/4, -PI/4, -3*PI/4, -PI/4, -3*PI/4, 3*PI/4, PI/4, PI/4, 3*PI/4, -3*PI/4, -PI/4, -3*PI/4, -PI/4, -PI/4, -3*PI/4, -PI/4, -3*PI/4, 3*PI/4, PI/4},
	{PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, PI/4, PI/4, PI/4, PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, PI/4, PI/4, PI/4},
	{PI/4, -PI/4, -3*PI/4, 3*PI/4, -3*PI/4, 3*PI/4, 3*PI/4, -3*PI/4, 3*PI/4, -3*PI/4, -PI/4, PI/4, PI/4, -PI/4, -3*PI/4, 3*PI/4, -3*PI/4, 3*PI/4, 3*PI/4, -3*PI/4, 3*PI/4, -3*PI/4, -PI/4, PI/4}};

	double error[23], avgerr[4] = {0, 0, 0, 0};
	
	double err = 0, minerr = 0, phasediffrad = 0, phasediffdeg = 0, angle = 0;
	
	double wrappedphase[23], squarederror[pb];
	
	double rotationval[24];
	
	// Phase differences between received symbols
	//double rx[23] = {2.86598,2.9291,2.58842,-0.063846,2.47186,2.8853,
					//0.090288,2.84925,0.037292,0.87529,0.093727,
					//0.091378,
					//3.141574, 2.76071,3.0256,0.82989,2.994173,2.80657,
					//0.05355,2.99405,0.311574,0.10004,0.11004};
	double rx[23] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
					

	// Phases of received symbols
    //double rxangle[24] = {PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, PI/4, PI/4, PI/4, 
    					//PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, PI/4, PI/4, PI/4};
	// PI/4 = 0.78539
	// -3*PI/4 = -2.35619
	double rxangle[24] = {0.68539,-2.79812,0.88239,-2.93819,-2.75649,0.70539,
						 -2.71218,-3.14015,0.55539,0.80039,0.88539,0.88539,
						 0.88539,-3.08823,0.88539,-2.90011,-3.10038,0.88539,
						 -2.88819,-3.05067,0.88539,0.88539,0.88539,0.88539};					 
	
	int at;// = 0;
	
	int rotationgray= 0, patterngray = 0;
	
	double sumrotation = 0.0, avgrot = 0.0;

  	high_resolution_clock::time_point t1 = high_resolution_clock::now();
	
	int idx = 0;
	for(int c=0; c<24; c++){
		if(c>0){
			rx[idx] = rxangle[c]-rxangle[c-1];
			//std::cout << "phasediff "<< rx[idx]<< " at " << idx <<std::endl;
			idx++;
			}
	}

	int i, j, k;
	for (i = 0; i<pb; i++){
	    err = 0;
	    for (j = 0; j<23; j++){
	        if(j == 5 || j == 11 || j == 17){
	            
	            phasediffrad = 0;
	        }
	        else {
	        phasediffrad = p[i][j]-rx[j];
	        phasediffdeg = phasediffrad * (180/PI);
	        }
	        angle = fmod(phasediffdeg + 180,360);
            if (angle < 0)
                angle += 360;
            wrappedphase[j] = angle - 180;
            //std::cout << "phasediff "<< wrappedphase[j]<< " at " <<j <<std::endl;
            //if(wrappedphase[j] < -2.40){wrappedphase[j] = -1*wrappedphase[j];}
	        err = err + wrappedphase[j];
	}
	
	//avgerr[i] = abs(err)/23.0;
	avgerr[i] = ((err)*(err)/(23.0*9.0))*(PI/180);
	}
	
	minerr = avgerr[0];
	at = 0;
	for (i = 0; i<pb; i++){
    	if(minerr > avgerr[i]){
             minerr = avgerr[i];
             at = i;
             std::cout << "minerr "<< minerr<< " at " <<i <<std::endl;
	}
}

  	//high_resolution_clock::time_point t2 = high_resolution_clock::now();

    patterngray = at ^ (at >> 1);
    cout << "pattern bits: "<< "\n";
    // Size of an integer is assumed to be Q bits
    for (int ij = 1; ij >= 0; ij--) {
        int kk = patterngray >> ij;
        if (kk & 1)
            cout << "1";
        else
            cout << "0";
}

  	high_resolution_clock::time_point t3 = high_resolution_clock::now();
  	
	for(i = 0; i<pb; i++){
    	if (at==i){
        	for(j = 0; j<23; j++){
            	rotationval[j] = r[i][j]-rxangle[j];
				sumrotation = sumrotation + rotationval[j];
        	}
        	avgrot = (sumrotation/24.0)*(180/PI);
    	}
	}

    double rotation = 0;
    int d = 0;
    int b = 0;
    double val = 0, sideval = 0.0;
    double full = 360.0;
    
    rotation = avgrot;
    val = full/M;
    sideval = val/2.0;

    for (int i = 0; i < M; i++) {
            if (i != 0){
                while(((sideval+i*val)+sideval) == full){
                    if ((rotation > (sideval+(i-1)*val)) && (rotation <= (sideval+i*val))){
                            d = i;
                            break;
                    }
                }
            }
            //if (i == 0){
            else{
                if (((rotation >= 0) && (rotation <= sideval)) || ((rotation > (full - sideval)) && (rotation <= full))){
                        d = 0;
                        break;
                }
            }
    } 
    
     
  	high_resolution_clock::time_point t4 = high_resolution_clock::now();
  	
   	rotationgray= d ^ (d >> 1);
    cout << "\nrotation bits: " << "\n";
    // Size of an integer is assumed to be M bits
    for (int i = M-1; i >= 0; i--) {
        int k =rotationgray>> i;
        if (k & 1)
            cout << "1";
        else
            cout << "0";
    }
    
 
  	duration<double, std::milli> time_span =  t4 - t1;// t2 - t1 + t4 - t3;
  	
 
  	std::cout << "\nExtraction Time: " << time_span.count() << " ms.";
  	
  	duration<double, std::milli> pat =  t3 - t1;// t2 - t1 + t4 - t3;
  	
  	std::cout << "\nPattern Extraction Time: " << pat.count() << " ms.";
  	
  	std::cout << std::endl;
}