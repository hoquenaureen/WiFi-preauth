// BLS signature

#include <pbc/pbc.h>
#include <pbc/pbc_test.h>
#include <string.h>
#include <time.h>

int main(int argc, char **argv) {
  pairing_t pairing;
  element_t g, h;
  element_t public_key, sig;
  element_t secret_key;
  element_t temp1, temp2;
  
  double time1 = 0, time2 = 0;

  pbc_demo_pairing_init(pairing, argc, argv);

  element_init_G2(g, pairing);
  element_init_G2(public_key, pairing);
  element_init_G1(h, pairing);
  element_init_G1(sig, pairing);
  element_init_GT(temp1, pairing);
  element_init_GT(temp2, pairing);
  element_init_Zr(secret_key, pairing);

  printf("BLS\n");

  //generate system parameters
  element_random(g);
  // element_printf("system parameter g = %B\n", g);
  
  time1 = 0;
  time2 = 0;
  
  //generate private key
  time1 = pbc_get_time();
  element_random(secret_key);
  time2 = pbc_get_time();
  // element_printf("private key = %B\n", secret_key);
  // printf("Time to Generate Private Key = %.6f sec\n", (time2 - time1));
  
  int prik = element_length_in_bytes_x_only(secret_key);
  //printf("PubKey SIZE\n");
  //printf("%lu \n", sizeof(pubk));
  //printf("Private key Size = %d bytes\n", prik);

  time1 = 0;
  time2 = 0;
  time1 = pbc_get_time();
  
  //compute corresponding public key 
  element_pow_zn(public_key, g, secret_key);
  
  time2 = pbc_get_time();
  //element_printf("public key = %B\n", public_key);
  
  printf("Time to Generate Public Key = %.6f sec\n", (time2 - time1));

  int pubk = element_length_in_bytes_x_only(public_key);
  //printf("PubKey SIZE\n");
  //printf("%lu \n", sizeof(pubk));
  //printf("Public key Size = %d bytes\n", pubk);

  time1 = 0;
  time2 = 0;
  time1 = pbc_get_time();
  
  //generate element from a hash
  //for toy pairings, should check that pairing(g, h) != 1
  element_from_hash(h, "9801a7bf5c1f9801a7bf5c1fffff", 13);
  //element_printf("message hash = %B\n", h);

  //h^secret_key is the signature
  //in real life: only output the first coordinate
  double t = 0;
  for (int j = 0; j < 100; j++) {
  	time1 = 0;
  	time2 = 0;
  	time1 = pbc_get_time();
  	element_pow_zn(sig, h, secret_key);
  	time2 = pbc_get_time();
  	t = t+(time2 - time1);
  }
  
  printf("Average Time to Generate Sign = %.4f sec\n", t/100);
  //element_printf("signature = %B\n", sig);
  //printf("SIGNATURE SIZE\n");
  //printf("%lu \n", sizeof(sig));
  //printf("%s \n", strlen(sig));


  {
    int n = pairing_length_in_bytes_compressed_G1(pairing);
    int i;
    unsigned char *data = pbc_malloc(n);
    element_to_bytes_compressed(data, sig);
    // printf("compressed = ");


    element_from_bytes_compressed(sig, data);
    // element_printf("decompressed = %B\n", sig);

    pbc_free(data);
  }

  //verification part 1
  double tt = 0;
  for (int jj = 0; jj < 100; jj++) {
  time1 = 0;
  time2 = 0;
  time1 = pbc_get_time();
  element_pairing(temp1, sig, g);
  // element_printf("f(sig, g) = %B\n", temp1);

  // verification part 2
  // should match above
  element_pairing(temp2, h, public_key);
  // element_printf("f(message hash, public_key) = %B\n", temp2);

  if (!element_cmp(temp1, temp2)) {
  	// printf("signature verifies\n");
  } else {
  	// printf("*BUG* signature does not verify *BUG*\n");
  }
  
  time2 = pbc_get_time();
  tt = tt+(time2 - time1);
  
  }
  printf("Average Time to Verify Sign = %.4f sec\n", tt/100);
  // float t = (float)((t_ver2-t_ver1)/(1000000.0F))*10000;
  // printf("Time to Verify = %.6f\n and test = %.6f\n", t_ver, t);
  // printf("Public key Size = %d bytes\n", pubk);
  // printf("Signature size = %d byte or %d bits\n", sigg, sigg*8);
  

  {
    int n = pairing_length_in_bytes_x_only_G1(pairing);
    int sigg = element_length_in_bytes_x_only(sig);
    int sigbit = sigg*8;
    //printf("SIGNATURE SIZE as sigg\n");
    //printf("%lu \n", sizeof(sigg));
    printf("Public key Size = %d bytes\n", pubk);
    printf("Signature size = %d byte or %d bits\n", sigg, sigbit);

    int i;
    unsigned char *data = pbc_malloc(n);

    element_to_bytes_x_only(data, sig);
    //printf("x-coord = ");

    element_from_bytes_x_only(sig, data);
    // element_printf("de-x-ed = %B\n", sig);


    element_pairing(temp1, sig, g);
    if (!element_cmp(temp1, temp2)) {
    // printf("signature verifies on first guess\n");
    } else {
      element_invert(temp1, temp1);
      if (!element_cmp(temp1, temp2)) {
      //  printf("signature verifies on second guess\n");
      } else {
      //  printf("*BUG* signature does not verify *BUG*\n");
      }
    }

    pbc_free(data);
  }

  //a random signature shouldn't verify
  element_random(sig);
  element_pairing(temp1, sig, g);
  if (element_cmp(temp1, temp2)) {
  	// printf("random signature doesn't verify\n");
  } else {
  	// printf("*BUG* random signature verifies *BUG*\n");
  }
  
  element_clear(sig);
  element_clear(public_key);
  element_clear(secret_key);
  element_clear(g);
  element_clear(h);
  element_clear(temp1);
  element_clear(temp2);
  pairing_clear(pairing);
  return 0;
}
