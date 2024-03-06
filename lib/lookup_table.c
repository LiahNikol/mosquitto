#include "lookup_table.h"

// TODO: Design a protocol for removing entries from the table

#define KEY_SIZE 128

unsigned char ukey[KEY_SIZE / 8] = "muP7DFrEC#ch3(6P"; // TODO: share with subscriber, who will use AES_set_decrypt_key to generate the key schedule and AES_cbc_decrypt()

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
    struct plaintext_ciphertext_pair *tmp;

    // TODO: confirm that we won't get this far if there are no subscribers
    HASH_FIND(hh, entries, payload, p_len, tmp);
    if (tmp == NULL) { // no entry was found
        tmp = (struct plaintext_ciphertext_pair *)mosquitto__malloc(sizeof(struct plaintext_ciphertext_pair));

        uint8_t *p_buf = (uint8_t *)mosquitto__malloc(sizeof(uint8_t) * p_len);
        memcpy(p_buf, payload, p_len); 
        tmp->plaintext = p_buf;
        
        uint8_t *c_buf = (uint8_t *)mosquitto__malloc(sizeof(uint8_t) * p_len);
        unsigned char *iv = (unsigned char *)mosquitto__malloc(AES_BLOCK_SIZE * sizeof(unsigned char));
        assert(getrandom(iv, AES_BLOCK_SIZE, GRND_NONBLOCK) != -1);

        encrypt(p_buf, c_buf, iv, p_len);

        tmp->ciphertext = c_buf;
        HASH_ADD(hh, entries, plaintext, p_len, tmp);
    }
    return tmp->ciphertext;
}

void debug (uint8_t *out, size_t len, int plaintext) {
  uint8_t buf[len + 1];
  for (int i = 0; i < len; i++) {
    uint8_t curChar = out[i];
    if (curChar == '\0') 
      curChar = '$';
    buf[i] = curChar;
  }
  buf[len] = '\0';
  
  if (plaintext) {
    printf("The plaintext in human readable format: %s", buf);
  } else {
    printf("The ciphertext in human readable format: %s", buf);
  }
}
