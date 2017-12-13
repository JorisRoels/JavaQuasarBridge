#ifndef BE_VIB_BITS_QUASARBRIGE_ENCODING_H
#define BE_VIB_BITS_QUASARBRIGE_ENCODING_H

#include <string>

#include "char16_string.h"

// Returns a regular UTF8 encoded string given a pointer
// to an array of characters encoded in *modified* UTF8.
// Modified UTF8 encoding is used to represent strings
// in JNI.
std::string UTF16toModifiedUTF8(const TCHAR * in);

#endif
