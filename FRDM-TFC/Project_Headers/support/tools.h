/* tools.h
 * ==========================================
 * Misc. methods for doing useful stuff.
 */

#include <stdint.h>

void  cast_uint8(uint8_t* out, void* var, uint16_t size);
uint32_t getTime();
void  getTimestamp(void* time);
