// KGP signature

#include <pbc/pbc.h>
#include <pbc/pbc_test.h>

int main(int argc, char **argv) {

  pairing_t pairing;
  double time1 = 0, time2 = 0;
  element_t Ppub, s, P, R, k, S, Did, Qid, t1, t2, t4, t5, t6, t7, t8, t9, t10, t11;
  
  mpz_t t3;
  mpz_init(t3);
  
  pbc_demo_pairing_init(pairing, argc, argv);
  
  if (!pairing_is_symmetric(pairing)) pbc_die("pairing must be symmetric");

  element_init_G1(P, pairing);
  element_init_G1(Ppub, pairing);
  element_init_G1(Qid, pairing);
  element_init_G1(Did, pairing);
  element_init_G1(R, pairing);
  element_init_G1(S, pairing);
  element_init_G1(t2, pairing);
  element_init_G1(t4, pairing);
  element_init_G1(t5, pairing);
  element_init_G1(t7, pairing);

  element_init_Zr(s, pairing);
  element_init_Zr(k, pairing);
  element_init_Zr(t1, pairing);

  element_init_GT(t6, pairing);
  element_init_GT(t8, pairing);
  element_init_GT(t9, pairing);
  element_init_GT(t10, pairing);
  element_init_GT(t11, pairing);

  //time1 = pbc_get_time();
  printf("Paterson's ID-based Digital Signature\n");
  //printf("KEYGEN\n");
  time1 = 0;
  time2 = 0;
  
  // Public key generation
  element_random(P);
  element_random(s);
  element_mul_zn(Ppub, P, s);
  //element_printf("P = %B\n", P);
  //element_printf("Ppub = %B\n", Ppub);
  
  time1 = pbc_get_time();
  element_from_hash(Qid, "9801a7bf5c1f9801a7bf5c1f", 2);
  //element_printf("Qid = %B\n", Qid);
  
  element_mul_zn(Did, Qid, s);
  time2 = pbc_get_time();
  element_printf("Private Key = %B\n", Did);
  
  //printf("Time to Generate Private Key = %.6f sec\n", (time2 - time1));
  
  int prik = element_length_in_bytes_x_only(Did);

  //printf("SIGN\n");
  int sigg = 0, siggR = 0, siggS = 0;
  int sigbit = 0;
  double t = 0;
  
  for (int j = 0; j < 100; j++) {
  	time1 = 0;
  	time2 = 0;
  	time1 = pbc_get_time();
  	element_random(k);
  	element_mul_zn(R, P, k);
  	element_from_hash(t1, "9801a7bf5c1f9801a7bf5c1fffff", 7);
  	element_mul_zn(t2, P, t1);
    //H3(R)=mpz(R);
    // int n = element_length_in_bytes(R);
	// unsigned char *data=malloc(n);
	// element_to_bytes(data, R);
	// printf("data = %s\n", data);
  	element_to_mpz(t3, R);
  	element_mul_mpz(t4, Did, t3);
 	element_add(t5, t4, t2);
  	element_invert(k, k);
  	element_mul_zn(S, t5, k);
  	
  	siggR = element_length_in_bytes_x_only(R);
  	siggS = element_length_in_bytes_x_only(S);
  	sigg = siggR+siggS;
  	sigbit = sigg*8;
  	
  	//printf("Signature of message \"Message\" is:\n");
  	//element_printf("R = %B\n", R);
  	//element_printf("S = %B\n", S);
  	
  	time2 = pbc_get_time();
  	t = t+(time2 - time1);
  }
  	//printf("Time to Generate Sign = %.4f sec\n", t/100);

  	//printf("VERIFY\n");
  
  double tt = 0;
  for (int jj = 0; jj < 100; jj++) {
	time1 = 0;
	time2 = 0;
	time1 = pbc_get_time();
 	element_from_hash(t1, "9801a7bf5c1f9801a7bf5c1fffff", 7);
 	element_mul_zn(t7, P, t1);
  	element_pairing(t6, P, t7);
  	element_pairing(t8, Ppub, Qid);
 	element_to_mpz(t3, R);
 	element_pow_mpz(t9, t8, t3);
  	//	element_printf("t8 = %B\n", t8);
  	//	element_printf("t9 = %B\n", t9);
  	element_mul(t10, t6, t9);
  	//element_printf("t10 = %B\n", t10);
  	element_pairing(t11, R, S);
 	// element_printf("[e(P, P)^H2(M)][e(Ppub, Qid)^H3(R)] = %B\n", t10);
  	//element_printf("e(R, S) = %B\n", t11);
  	if (!element_cmp(t10, t11)) {
    	//printf("Signature is valid!\n");
  } else {
    	//printf("Signature is invalid!\n");
  }
  time2 = pbc_get_time();
  tt = tt+(time2 - time1);
  }
  
  printf("Signature of message:\n");
  
  element_printf("R = %B\n", R);
  element_printf("S = %B\n", S);
  
  printf("Signature size = %d byte or %d bits\n", sigg, sigbit);
  printf("R size = %d byte\nS size = %d byte\n", siggR, siggS);
  printf("Time to Generate Sign = %.4f sec\n", t/100);	
  printf("Time to Verify Sign = %.4f sec\n", tt/100);
  
  element_clear(P);
  element_clear(Ppub);
  element_clear(Qid);
  element_clear(Did);
  element_clear(R);
  element_clear(t2);
  element_clear(t4);
  element_clear(t5);
  element_clear(s);
  element_clear(k);
  element_clear(t1);
  element_clear(t6);
  element_clear(t7);
  element_clear(t8);
  element_clear(t9);
  element_clear(t10);
  element_clear(t11);
  pairing_clear(pairing);

  return 0;
}
