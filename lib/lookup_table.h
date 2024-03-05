#include <openssl/aes.h>
#include <sys/random.h>
#include <assert.h>
#include <string.h>
#include "memory_mosq.h"
#include "mosquitto_internal.h"
#include "uthash.h"

uint8_t *checkLookupTable(uint8_t *payload, size_t p_len /*, uint8_t *topic*/);