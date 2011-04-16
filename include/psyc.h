#include <stdint.h>
#include <string.h>

#define PSYC_EPOCH      1440444041      // 2015-08-24 21:20:41 CET (Monday)

/** @brief Checks if short keyword string matches long keyword string
 */
int PSYC_matches(uint8_t* sho, unsigned int slen,
		 uint8_t* lon, unsigned int llen);

