/* tools.h
 * ==========================================
 * Misc. methods for doing useful stuff.
 */

#include <stdint.h>

void  cast_uint8(uint8_t* out, void* var, uint16_t size);
float getTime();
void  getTimestamp(uint8_t * time);
