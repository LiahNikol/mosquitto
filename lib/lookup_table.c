#include "lookup_table.h"

#define KEY_SIZE 128

unsigned char ukey[KEY_SIZE / 8] = "muP7DFrEC#ch3(6P" // TODO: share with subscriber, who will use AES_set_decrypt_key to generate the key schedule and AES_cbc_decrypt()

struct plaintext_ciphertext_pair {
    uint8_t *plaintext;
    uint8_t *ciphertext;
    //uint8_t *topic;
    UT_hash_handle hh; // makes the struct hashable so that we can create a hashtable
};

struct plaintext_ciphertext_pair *entries = NULL; 

void encrypt(uint8_t *in, uint8_t *out, size_t len) {
    AES_KEY key;
    AES_set_encrypt_key(ukey, KEY_SIZE, &key);
    unsigned char iv[AES_BLOCK_SIZE];
    assert(getrandom(iv, AES_BLOCK_SIZE,GRND_NONBLOCK) != -1);
    AES_cbc_encrypt(in, out, len, key, iv, AES_ENCRYPT); // length preserving encryption
}

uint8_t *checkLookupTable(uint8_t *payload, size_t p_len /*, uint8_t *topic*/) {
    struct plaintext_ciphertext_pair *tmp;

    HASH_FIND(hh, entries, payload, p_len, tmp);
    if (tmp == null) { // no entry was found
        tmp = (struct plaintext_ciphertext_pair *)mosquitto__malloc(sizeof(struct plaintext_ciphertext_pair));

        uint8_t *p_buf = (uint8_t *)mosquitto__malloc(sizeof(uint8_t) * p_len);
        strncpy(p_buf, payload, p_len); // TODO: memcopy or memmv
        tmp->plaintext = p_buf;
        
        uint8_t *c_buf = (uint8_t *)mosquitto__malloc(sizeof(uint8_t) * p_len);
        encrypt(p_buf, c_buf, p_len);
        tmp->ciphertext = c_buf;

        HASH_ADD(hh, entries, plaintext, p_len, tmp);

        debug(size_t p_len)
    }
    return tmp->ciphertext;
}

/**
void debug(size_t count) {
    unsigned char *readable_payload = (unsigned char *)mosquitto__malloc(count * sizeof(unsigned char) + 1);
	for(int i=0; i<(int)count; i++) {
		if(((unsigned char)(buf[i])) != '\0') {
			readable_payload[i] = (unsigned char)(buf[i]);
		} else { readable_payload[i] = '$';}	
	}
	readable_payload[count]='\0';
		
	printf("Broker writing content to %s: %s, with size %d\n", mosq->id, readable_payload, (int)count);
    fflush(stdout);*/
}*/
// TODO: Design a protocol for removing entries from the table