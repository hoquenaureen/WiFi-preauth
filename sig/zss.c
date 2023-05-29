// ZSS signature

#include <pbc/pbc.h>
#include <pbc/pbc_test.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv) {

  pairing_t pairing;
  pbc_demo_pairing_init(pairing, argc, argv);
  if (!pairing_is_symmetric(pairing)) pbc_die("pairing must be symmetric");
  
  double time1 = 0, time2 = 0;
  element_t P, Ppub, x, S, H, t1, t2, t3, t4;
  element_init_Zr(x, pairing);
  element_init_Zr(H, pairing);
  element_init_Zr(t1, pairing);

  element_init_G1(S, pairing);
  element_init_G1(P, pairing);
  element_init_G1(Ppub, pairing);
  element_init_G1(t2, pairing);

  element_init_GT(t3, pairing);
  element_init_GT(t4, pairing);

  printf("ZSS\n");
  //printf("KEYGEN\n");
  //time1 = pbc_get_time();

  time1 = 0;
  time2 = 0;
  time1 = pbc_get_time();
  element_random(x);
  element_random(P);
  // Public key generation
  element_mul_zn(Ppub, P, x);
  //element_printf("P = %B\n", P);
  //element_printf("x = %B\n", x);
  //element_printf("Ppub = %B\n", Ppub);
  
  time2 = pbc_get_time();
  // element_printf("private key = %B\n", secret_key);
  printf("Time to Generate Public Key = %.6f sec\n", (time2 - time1));
  
  int pubk = element_length_in_bytes_x_only(Ppub);
  //printf("PubKey SIZE\n");
  //printf("%lu \n", sizeof(pubk));
  //printf("Public key Size = %d bytes\n", pubk);
 
  double tt = 0;
  for (int j = 0; j < 100; j++) {
  time1 = 0;
  time2 = 0;
  time1 = pbc_get_time();
  // printf("SIGN\n");
  element_from_hash(H, "9801a7bf5c1f9801a7bf5c1fffff", 7);

  element_add(t1, H, x);
  element_invert(t1, t1);
  element_mul_zn(S, P, t1);
  time2 = pbc_get_time();
  tt = tt+(time2 - time1);
  }
  
  printf("Average Time to Generate Sign = %.4f sec\n", tt/100);	
  // printf("Signature of message \"Message\" is:\n");
  //element_printf("S = %B\n", S);
  
  int sigg = element_length_in_bytes_x_only(S);
  int sigbit = sigg*8;
  //printf("SIGNATURE SIZE as sigg\n");
  //printf("%lu \n", sizeof(sigg));
  //printf("Public key Size = %d bytes\n", pubk);
  //printf("Signature size = %d byte or %d bits\n", sigg, sigbit);

  //printf("VERIFY\n");
  double t = 0;
  for (int j = 0; j < 100; j++) {
  	time1 = 0;
  	time2 = 0;
  	time1 = pbc_get_time();
  	element_from_hash(H, "9801a7bf5c1f9801a7bf5c1fffff", 7);
  	//for (int jj = 0; jj < 100; jj++) {
  	element_mul_zn(t2, P, H);
  	element_add(t2, t2, Ppub);
  	element_pairing(t3, t2, S);
  	element_pairing(t4, P, P);
  	
  	// element_printf("e(H(m)P + Ppub, S) = %B\n", t3);
 	// element_printf("e(P, P) = %B\n", t4);
 	
  	if (!element_cmp(t3, t4)) {}//printf("Signature is valid\n");
  		else {}//printf("Signature is invalid\n");
  		
  	time2 = pbc_get_time();
  	t = t+(time2 - time1);
}

  printf("Average Time to Verify Sign = %.4f sec\n", t/100);
  //time2 = pbc_get_time();
  //printf("End-to-end = %fsec\n", (time2 - time1));
  printf("Public key Size = %d bytes\n", pubk);
  printf("Signature size = %d byte or %d bits\n", sigg, sigbit);
  
  element_clear(P);
  element_clear(Ppub);
  element_clear(x);
  element_clear(S);
  element_clear(H);
  element_clear(t1);
  element_clear(t2);
  element_clear(t3);
  element_clear(t4);
  pairing_clear(pairing);

  return 0;
}
