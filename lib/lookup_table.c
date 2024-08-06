#include "lookup_table.h"

// TODO: Design a protocol for removing entries from the table

#define KEY_SIZE 128

//unsigned char ukey[] = {0x91, 0x1e, 0x65, 0xae, 0x7e, 0x5e, 0x84, 0xe3, 0xcf, 0x84, 0xe9, 0x3c, 0x49, 0xb2, 0x00, 0x73}; // generate 128 bit AES_CBC encryption key

uint8_t ukey[] = "123456789123456";

struct plaintext_ciphertext_pair {
    uint8_t *plaintext;
    uint8_t *ciphertext;
    //uint8_t *topic;
    UT_hash_handle hh; // makes the struct hashable so that we can create a hashtable
};

struct plaintext_ciphertext_pair *entries = NULL; 

void encrypt(uint8_t *in, uint8_t *out, unsigned char *iv, size_t len) {
    AES_KEY key;
    AES_set_encrypt_key(ukey, KEY_SIZE, &key);
    unsigned char ivcpy[AES_BLOCK_SIZE];
    memcpy(ivcpy, iv, AES_BLOCK_SIZE);
    AES_cbc_encrypt(in, out, len, &key, ivcpy, AES_ENCRYPT); 
}

uint8_t *checkLookupTable(uint8_t *payload, size_t p_len /*, uint8_t *topic*/) {
    //struct plaintext_ciphertext_pair *tmp;
	
    printf("Payload length: %lu\n", p_len);

    //HASH_FIND_STR(entries, payload, tmp);
    //if (tmp == NULL) { // no entry was found
        //tmp = (struct plaintext_ciphertext_pair *)mosquitto__malloc(sizeof(struct plaintext_ciphertext_pair));

    	// determine how much we need to pad the plaintext before decrypting
        uint8_t n = AES_BLOCK_SIZE - (p_len % AES_BLOCK_SIZE);
	printf("n is 0x%x\n", n);
	uint8_t padded_len = p_len + n;
	printf("padded len 0x%x\n", padded_len);

        uint8_t *p_buf = (uint8_t *)mosquitto__malloc(p_len * sizeof(uint8_t));
	uint8_t *padded_p_buf = (uint8_t *)mosquitto__malloc(padded_len * sizeof(uint8_t));
	
        memcpy(p_buf, payload, p_len); 
        //tmp->plaintext = p_buf; // store the UNpadded version
        
	memset(padded_p_buf, n, padded_len);
	memcpy(padded_p_buf, payload, p_len);
	for (int i = 0; i < padded_len; i++) {
		printf("%02x", padded_p_buf[i]);
	}
	printf("\n");

        uint8_t *c_buf = (uint8_t *)mosquitto__malloc(padded_len * sizeof(uint8_t));
        //unsigned char iv[] = {0xf2, 0xef, 0x72, 0x7b, 0xa6, 0x32, 0xfa, 0xc0, 0xe0, 0xaf, 0xbc, 0x96, 0xf2, 0xa3, 0xa0, 0xd0}; //generate 128 bit IV
        
	uint8_t iv[] = "abcdefghijklmno";
	//assert(getrandom(iv, AES_BLOCK_SIZE, GRND_NONBLOCK) != -1);

        encrypt(p_buf, c_buf, iv, p_len);
       	printf("Done encrypting\n");

        //tmp->ciphertext = c_buf;
        //HASH_ADD_KEYPTR(hh, entries, tmp->plaintext, p_len, tmp);
   //}
   //return tmp->ciphertext;
   return c_buf;
}

void debug (uint8_t *out, size_t len, int plaintext) {
  uint8_t buf[len + 1];
  for (int i = 0; i < len; i++) {
    uint8_t curChar = out[i];
    if (curChar == '\0') 
      curChar = '!';
    buf[i] = curChar;
  }
  buf[len] = '\0';
  
  if (plaintext) {
    printf("The plaintext in human readable format: %s\n", buf);
  } else {
    printf("The ciphertext in human readable format: %s", buf);
  }
}
