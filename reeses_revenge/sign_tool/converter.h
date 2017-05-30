#ifndef __CONVERTER_H__
#define __CONVERTER_H__

#include <stdint.h>
#include <string>
#include "crypto.h"

bool convert_elf_to_meb( uint8_t *pELFData, uint32_t elfSize, uint8_t **pOutData, uint32_t &outDataSize, std::string &sErrorText, Crypto &oCrypto );

#endif // __CONVERTER_H__
