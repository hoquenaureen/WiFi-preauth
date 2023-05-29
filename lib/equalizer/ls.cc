/*
 * Copyright (C) 2016 Bastian Bloessl <bloessl@ccs-labs.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ls.h"
#include <cstring>
#include <iostream>
#include <math.h>
#include <cmath>
#include <time.h>
#include <fstream>
#include <cstdlib>

#define PI 3.14159265359
// Choose Q between: 4 and 8
#define Q 4 //log2 Q bits = pattern bits
// Choose M among 0 to anything: integer number
#define M 2 //log2 k bits = M (rotation) bits
#define md 360/(2*M)

using namespace gr::ieee802_11::equalizer;


double wrapAngle(double m){
    double x = m*180/PI;
    x = fmod(x + 180,360);
    if (x < 0)
        x += 360;
    return (x - 180)*(PI/180);
}

void ls::equalize(gr_complex *in, int n, gr_complex *symbols, uint8_t *bits, boost::shared_ptr<gr::digital::constellation> mod) {

	if(n == 0) {
		
		//std::memcpy(d_H, in, 64 * sizeof(gr_complex));
		std::memcpy(d_H, in, 128 * sizeof(gr_complex)); // for ac
		//for(int i = 0; i < 128; i++) {
		//	std::cout << "ls - n=0 at "<< i<< ", in: "<< in[i]<< ", H: "<< d_H[i] << std::endl;
		//}

	} else if(n == 1) {
		double signal = 0;
		double noise = 0;
		//for(int i = 0; i < 64; i++) {
		for(int i = 0; i < 128; i++) { // for ac
			//std::cout << "ls - in when n=1 at "<< i<< " : " << in[i] << std::endl;
			//if((i == 32) || (i < 6) || ( i > 58)) {
			if((i == 32) || (i < 6) || (i > 58 && i < 70) || (i == 96) || ( i > 122)) {// for ac
			//if((i != 8) && (i != 12) && ( i != 16)&& (i != 20) && (i != 24)&& (i != 28) && (i != 36)
			//	&& (i != 40) && (i != 44)&& (i != 48) && (i != 52) && (i != 56)){
				continue;
			}
			noise += std::pow(std::abs(d_H[i] - in[i]), 2);
			signal += std::pow(std::abs(d_H[i] + in[i]), 2);
			d_H[i] += in[i];
			d_H[i] /= LONG[i] * gr_complex(2, 0);
			//std::cout << "ls - n=1 at "<< i<< ", in: "<< in[i]<< ", H: "<< d_H[i]<< ", LONG: "<< LONG[i] << std::endl;
		}

		d_snr = 10 * std::log10(signal / noise / 2);
		std::cout << "snr: "<< d_snr << std::endl;
		//std::cout << "snr:"<< d_snr;

	} else {

		

		double p0[23] = {0, 0, 0, PI, 0, PI, PI, 0, PI, PI, PI, 0, 0, 0, 0, PI, 0, PI, PI, 0, PI, PI, PI}; //p0 for Q4
		double p1[23] = {PI/4, PI/4, PI/4, -PI/4, PI/4, 0, -PI/4, PI/4, -PI/4, -PI/4, -PI/4, 0, PI/4, PI/4, PI/4, -PI/4, PI/4, 0, -PI/4, PI/4, -PI/4, -PI/4, -PI/4};
		double p2[23] = {PI/2, PI/2, PI/2, -PI/2, PI/2, 0, -PI/2, PI/2, -PI/2, -PI/2, -PI/2, 0, PI/2, PI/2, PI/2, -PI/2, PI/2, 0, -PI/2, PI/2, -PI/2, -PI/2, -PI/2}; //p1 for Q4
		double p3[23] = {3*PI/4, 3*PI/4, 3*PI/4, -3*PI/4, 3*PI/4, 0, -3*PI/4, 3*PI/4, -3*PI/4, -3*PI/4, -3*PI/4, 0, 3*PI/4, 3*PI/4, 3*PI/4, -3*PI/4, 3*PI/4, 0, -3*PI/4, 3*PI/4, -3*PI/4, -3*PI/4, -3*PI/4};
		double p4[23] = {PI, PI, PI, 0, PI, PI, 0, PI, 0, 0, 0, 0, PI, PI, PI, 0, PI, PI, 0, PI, 0, 0, 0}; //p2 for Q4
		double p5[23] = {-3*PI/4, -3*PI/4, -3*PI/4, 3*PI/4, -3*PI/4, 0, 3*PI/4, -3*PI/4, 3*PI/4, 3*PI/4, 3*PI/4, 0, -3*PI/4, -3*PI/4, -3*PI/4, 3*PI/4, -3*PI/4, 0, 3*PI/4, -3*PI/4, 3*PI/4, 3*PI/4, 3*PI/4};
		double p6[23] = {-PI/2, -PI/2, -PI/2, PI/2, -PI/2, 0, PI/2, -PI/2, PI/2, PI/2, PI/2, 0, -PI/2, -PI/2, -PI/2, PI/2, -PI/2, 0, PI/2, -PI/2, PI/2, PI/2, PI/2}; //p3 for Q4
		double p7[23] = {-PI/4, -PI/4, -PI/4, PI/4, -PI/4, 0, PI/4, -PI/4, PI/4, PI/4, PI/4, 0, -PI/4, -PI/4, -PI/4, PI/4, -PI/4, 0, PI/4, -PI/4, PI/4, PI/4, PI/4};

		double r0[24] = {PI/4, PI/4, PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, PI/4, PI/4, PI/4, PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, PI/4}; //r0 for Q4
		double r1[24] = {PI/4, PI/2, 3*PI/4, -PI, 3*PI/4, -PI, -PI, 3*PI/4, -PI, 3*PI/4, PI/2, PI/4, PI/4, PI/2, 3*PI/4, -PI, 3*PI/4, -PI, -PI, 3*PI/4, -PI, 3*PI/4, PI/2, PI/4};
		double r2[24] = {PI/4, 3*PI/4, -3*PI/4, -PI/4, -3*PI/4, -PI/4, -PI/4, -3*PI/4, -PI/4, -3*PI/4, 3*PI/4, PI/4, PI/4, 3*PI/4, -3*PI/4, -PI/4, -3*PI/4, -PI/4, -PI/4, -3*PI/4, -PI/4, -3*PI/4, 3*PI/4, PI/4};//r1 for Q4
		double r3[24] = {PI/4, -PI, -PI/4, PI/2, -PI/4, PI/2, PI/2, -PI/4, PI/2, -PI/4, -PI, PI/4, PI/4, -PI, -PI/4, PI/2, -PI/4, PI/2, PI/2, -PI/4, PI/2, -PI/4, -PI, PI/4};
		double r4[24] = {PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, PI/4, PI/4, PI/4, PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, -3*PI/4, -3*PI/4, PI/4, PI/4, PI/4, PI/4};//r2 for Q4
		double r5[24] = {PI/4, -PI/2, 3*PI/4, 0, 3*PI/4, 0, 0, 3*PI/4, 0, 3*PI/4, -PI/2, PI/4, PI/4, -PI/2, 3*PI/4, 0, 3*PI/4, 0, 0, 3*PI/4, 0, 3*PI/4, -PI/2, PI/4};
		double r6[24] = {PI/4, -PI/4, -3*PI/4, 3*PI/4, -3*PI/4, 3*PI/4, 3*PI/4, -3*PI/4, 3*PI/4, -3*PI/4, -PI/4, PI/4, PI/4, -PI/4, -3*PI/4, 3*PI/4, -3*PI/4, 3*PI/4, 3*PI/4, -3*PI/4, 3*PI/4, -3*PI/4, -PI/4, PI/4};//r3 for Q4
		double r7[24] = {PI/4, 0, -PI/4, -PI/2, -PI/4, -PI/2, -PI/2, -PI/4, -PI/2, -PI/4, 0, PI/4, PI/4, 0, -PI/4, -PI/2, -PI/4, -PI/2, -PI/2, -PI/4, -PI/2, -PI/4, 0, PI/4};
		
		double phsdif[23], p[23], rotdif[24];
		int pbits, rbits, x = 0;
		double sumrot = 0, avgrot = 0;
		clock_t tv1, tv2;
  		double time = 0, error0, error1, error2, error3, error4, error5, error6, error7;
		double sum_sq4[4] = {0, 0, 0, 0}, mse4[4] = {0, 0, 0, 0};
		double sum_sq[8] = {0, 0, 0, 0, 0, 0, 0, 0}, mse[8] = {0, 0, 0, 0, 0, 0, 0, 0};
		double mmse = 0, mini = 0;
  		double ratio, theta[24];
		double phsdiff[23];


		tv1 = clock();
		int c = 0, d = 0;
		//for(int i = 0; i < 64; i++) {
		for(int i = 0; i < 128; i++) { //for ac

			//std::cout << "ls - n>1 at "<< i<< ", in: "<< in[i]<< std::endl;

			//std::cout << "in when n>1 at "<< i<< " : " << in[i] << std::endl;
			//if( (i == 11) || (i == 25) || (i == 32) || (i == 39) || (i == 53) || (i < 6) || ( i > 58)) {
			//	continue;
			if((i != 8) && (i != 12) && ( i != 16)&& (i != 20) && (i != 24)&& (i != 28) && 
			  (i != 36) && (i != 40) && (i != 44)&& (i != 48) && (i != 52) && (i != 56) && 
			  (i != 72) && (i != 76) && ( i != 80)&& (i != 84) && (i != 88)&& (i != 92) && 
			  (i != 100) && (i != 104)&& (i != 108) && (i != 112) && (i != 116) && (i != 120)){
				continue;
			} else {
				symbols[c] = in[i]/d_H[i];
			
				//std::cout << "ls - n>1 at "<< i<< ", in: "<< in[i]<< ", H: "<< d_H[i]<< std::endl;

				//std::cout<<"input, H, symbol at "<<i<<": "<<in[i]<<", "<<d_H[i]<<", "<<symbols[c]<< std::endl;
				//std::cout << "symbols at "<< i<< " : " << symbols[c]<< std::endl;

				bits[c] = mod->decision_maker(&symbols[c]);
				//std::cout << "symbols as bits at "<< c<< " : " << bits[c]<< std::endl;

				ratio = (symbols[c].imag())/(symbols[c].real());
				//std::cout << "ratio of symbol"<< c<< ": "<< ratio << std::endl;
				
				if((symbols[c].imag())<0 && (symbols[c].real())<0){
					theta[c] = atan(ratio)-PI;	
				}
				else{
					theta[c] = atan(ratio);
				}
				//std::cout << "difference at "<< c<< ": "<<45-theta[c]*180/PI<< std::endl;
				//std::cout << "theta of symbols"<< c<< ": "<< theta[c] <<" in deg: "<<theta[c]*180/PI<< std::endl;
					if (c>=1){
						//std::cout << "theta difference"<< c<< ": "<< (theta[c]-theta[c-1])*180/PI << std::endl;
		//https://stackoverflow.com/questions/40286650/how-to-get-the-anti-clockwise-angle-between-two-2d-vectors
						//a=ax+i*ay to b=bx+i*by: phsdif = atan2( ax*by-ay*bx, ax*bx+ay*by ).

						gr_complex b = symbols[c];
						gr_complex a = symbols[c-1];
						phsdif[d] = atan2((b.real())*(a.imag())-(b.imag())*(a.real()),
							(b.real())*(a.real())+(b.imag())*(a.imag()));
						
						phsdiff[d] = wrapAngle(phsdif[d]);

		
						if(Q==8){
						error0 = abs(p0[d] - phsdif[d]);
						error1 = abs(p1[d] - phsdif[d]);
						error2 = abs(p2[d] - phsdif[d]);
						error3 = abs(p3[d] - phsdif[d]);
						error4 = abs(p4[d] - phsdif[d]);
						error5 = abs(p5[d] - phsdif[d]);
						error6 = abs(p6[d] - phsdif[d]);
						error7 = abs(p7[d] - phsdif[d]);
						//std::cout << "printing error4... "<< error4 << std::endl;
						//std::cout << "printing error7... "<< error7 << std::endl;
						sum_sq[0] = sum_sq[0] + (error0 * error0);
						sum_sq[1] = sum_sq[1] + (error1 * error1);
						sum_sq[2] = sum_sq[2] + (error2 * error2);
						sum_sq[3] = sum_sq[3] + (error3 * error3);
						sum_sq[4] = sum_sq[4] + (error4 * error4);
						sum_sq[5] = sum_sq[5] + (error5 * error5);
						sum_sq[6] = sum_sq[6] + (error6 * error6);
						sum_sq[7] = sum_sq[7] + (error7 * error7);
						//std::cout << "printing sum squared error... "<< sum_sq << std::endl;
						}
						
						if(Q==4){
						error0 = abs(p0[d] - phsdif[d]);
						error1 = abs(p2[d] - phsdif[d]);
						error2 = abs(p4[d] - phsdif[d]);
						error3 = abs(p6[d] - phsdif[d]);
						//std::cout << "printing error2... "<< error2 << std::endl;
						sum_sq4[0] = sum_sq4[0] + (error0 * error0);
						sum_sq4[1] = sum_sq4[1] + (error1 * error1);
						sum_sq4[2] = sum_sq4[2] + (error2 * error2);
						sum_sq4[3] = sum_sq4[3] + (error3 * error3);
						//std::cout << "printing sum squared error... "<< sum_sq << std::endl;
						}
						d++;
						}			
				c++;
			}
		}

		if(Q==8){
		for (int m = 0; m <8 ; m++){
			mse[m] = (sum_sq[m] /23.0);
			//std::cout <<"sum sq: " << sum_sq[m]<<" and mse: "<< mse[m] <<", " ;
			
		}
		mmse = mse[0];
		for (int m = 0; m <8 ; m++){
			if ( mse[m] < mmse ){
		     		mmse = mse[m] ;
				mini = m;
				//std::cout <<"mmse: " << mmse <<", mse[m]: "<<mse[m]<<" and m: "<< m <<" mini: "<< mini<< ", " ;
				
			} 	
		}
		// Gray Coding for 8 bits: 000 001 011 010 110 111 101 100
		if(mini == 0){pbits = 0;std::cout <<"pattern bits: 000"<< std::endl;}
		if(mini == 1){pbits = 1;std::cout <<"pattern bits: 001"<< std::endl;}
		if(mini == 2){pbits = 2;std::cout <<"pattern bits: 011"<< std::endl;}
		if(mini == 3){pbits = 3;std::cout <<"pattern bits: 010"<< std::endl;}
		if(mini == 4){pbits = 4;std::cout <<"pattern bits: 110"<< std::endl;}
		if(mini == 5){pbits = 5;std::cout <<"pattern bits: 111"<< std::endl;}
		if(mini == 6){pbits = 6;std::cout <<"pattern bits: 101"<< std::endl;}
		if(mini == 7){pbits = 7;std::cout <<"pattern bits: 100"<< std::endl;}
		
		//GNURadio DPSK http://www.mcn.ece.ufl.edu/public/zhifeng/project/GNU_Radio/documents/Project%20Report_James%20Chen.pdf

		if(pbits == 0){
			while(x<sizeof(r0)){
				rotdif[x] = r0[x]-theta[x];
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: "<< avgrot <<std::endl;
		}
		if(pbits == 1){
			while(x<sizeof(r1)){
				rotdif[x] = r1[x]-theta[x];
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: "<< avgrot <<std::endl;
		}
		if(pbits == 2){
			while(x<sizeof(r2)){
				rotdif[x] = r2[x]-theta[x];
				//std::cout <<"r at "<< x << ": "<< r[x]*180/PI <<std::endl;
				//std::cout <<"theta at "<< x << ": "<< theta[x]*180/PI <<std::endl;
				//std::cout <<"rotation difference at "<< x << ": "<< rotdif[x] <<std::endl;
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: "<< avgrot <<std::endl;
		}
		if(pbits == 3){
			while(x<sizeof(r3)){
				rotdif[x] = r3[x]-theta[x];
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: "<< avgrot <<std::endl;
		}
		if(pbits == 4){
			while(x<sizeof(r4)){
				rotdif[x] = r4[x]-theta[x];
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: la la "<< avgrot <<std::endl;
		}
		if(pbits == 5){
			while(x<sizeof(r5)){
				rotdif[x] = r5[x]-theta[x];
				//std::cout <<"r at "<< x << ": "<< r[x]*180/PI <<std::endl;
				//std::cout <<"theta at "<< x << ": "<< theta[x]*180/PI <<std::endl;
				//std::cout <<"rotation difference at "<< x << ": "<< rotdif[x] <<std::endl;
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: "<< avgrot <<std::endl;
		}
		if(pbits == 6){
			while(x<sizeof(r6)){
				rotdif[x] = r6[x]-theta[x];
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: "<< avgrot <<std::endl;
		}
		if(pbits == 7){
			while(x<sizeof(r7)){
				rotdif[x] = r7[x]-theta[x];
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: "<< avgrot <<std::endl;
		}
		}

		if(Q==4){
		for (int m = 0; m <4 ; m++){
			mse4[m] = (sum_sq4[m] /23.0);
			//std::cout <<"sum sq: " << sum_sq[m]<<" and mse: "<< mse[m] <<".. " ;
			
		}
		mmse = mse4[0];
		for (int m = 0; m <4 ; m++){
			if ( mse4[m] < mmse ){
		     		mmse = mse4[m] ;
				mini = m;
				//std::cout <<"mmse: " << mmse <<", mse[m]: "<<mse[m]<<" and m: "<< m <<" mini: "<< mini<< ", " ;
				
			} 	
		}
		// Gray Coding for 4 bits: 00 01 11 10
		if(mini == 0){pbits = 0;std::cout <<"pattern bits: 00"<< std::endl;}
		if(mini == 1){pbits = 1;std::cout <<"pattern bits: 01"<< std::endl;}
		if(mini == 2){pbits = 2;std::cout <<"pattern bits: 11"<< std::endl;}
		if(mini == 3){pbits = 3;std::cout <<"pattern bits: 10"<< std::endl;}
		
		//GNURadio DPSK http://www.mcn.ece.ufl.edu/public/zhifeng/project/GNU_Radio/documents/Project%20Report_James%20Chen.pdf

		if(pbits == 0){
			while(x<sizeof(r0)){
				rotdif[x] = r0[x]-theta[x];
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: "<< avgrot <<std::endl;
		}
		if(pbits == 1){
			while(x<sizeof(r2)){
				rotdif[x] = r2[x]-theta[x];
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: "<< avgrot <<std::endl;
		}
		if(pbits == 2){
			while(x<sizeof(r4)){
				rotdif[x] = r4[x]-theta[x];
				//std::cout <<"r at "<< x << ": "<< r[x]*180/PI <<std::endl;
				//std::cout <<"theta at "<< x << ": "<< theta[x]*180/PI <<std::endl;
				//std::cout <<"rotation difference at "<< x << ": "<< rotdif[x] <<std::endl;
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: "<< avgrot <<std::endl;
			//std::cout <<"rotation in radian: "<< (sumrot/12) <<std::endl;
		}
		if(pbits == 3){
			while(x<sizeof(r6)){
				rotdif[x] = r6[x]-theta[x];
				x++;
				if(x==24){break;}	
				sumrot = sumrot + rotdif[x];
			} 
			avgrot = (sumrot/24)*(180/PI);
			//std::cout <<"rotation in degree: "<< avgrot <<std::endl;
		}
		}


    double rotation = 0;
    int mbits = 3; // meaning, 2^mbits
    int decimalval = 0;
    int g = 0;
    double val = 0, sideval = 0.0;
    double full = 360.0;
  
    rotation = avgrot;
    mbits = M;
    val = full/mbits;
    sideval = val/2.0;
    //std::cout << full - sideval << "\n";

    for (int ii = 0; ii < mbits; ii++) {
       //std::cout << "inside ii loop" << "\n";
            if (ii != 0){
               // cout << "inside ii not zero" << "\n";
                while(((sideval+ii*val)+sideval) == full){
                //while(i != (mbits-1)){
                    if ((rotation > (sideval+(ii-1)*val)) && (rotation <= (sideval+ii*val))){
                           // cout << "for ii = "<< ii <<"\n";
                           std::cout << (sideval+(ii-1)*val) << " < angle <= "<< (sideval+ii*val) <<"\n";
                            decimalval = ii;
                            break;
                    }
                }
            }
            if (ii == 0){
               // cout << "now inside i is zero" << "\n";
                if (((rotation >= 0) && (rotation <= sideval)) || ((rotation > (full - sideval)) && (rotation <= full))){
                        //cout << "for ii = "<< ii <<"\n";
                        //std::cout << "(0 or "<< (full - sideval) << ") < angle <= ("<< sideval << " or " << full <<")\n";
                        decimalval = 0;
                        break;
                }
            }
    } 
    //std::cout << "location: "<<decimalval << "\n";
    g = decimalval ^ (decimalval >> 1);
    //std::cout << "rotation bits (gray): " <<g << "\n";
    std::cout << "rotation bits (gray): ";
    // Size of an integer is assumed to be mbits bits
    for (int ii = mbits-1; ii >= 0; ii--) {
        int k = g >> ii;
        if (k & 1)
            std::cout << "1";
        else
            std::cout << "0";
    }
    //std::cout << "\n";

		
		tv2 = clock();
  		time = (tv2 - tv1)/(CLOCKS_PER_SEC / (double) 1000.0);
		//std::cout <<"Expected pattern bits = 110 and rotation bits: 000"<<std::endl;

		std::cout << "\nExtraction time in ms: " << time <<std::endl;
	}
}

double ls::get_snr() {
	return d_snr;
}
		
