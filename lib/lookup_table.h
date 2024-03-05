#include <openssl/aes.h>
#include <sys/random.h>
#include "memory_mosq.h"

uint8_t *checkLookupTable(uint8_t *payload, size_t p_len /*, uint8_t *topic*/);