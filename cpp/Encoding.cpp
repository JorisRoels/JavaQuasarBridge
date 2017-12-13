#include "Encoding.h"

std::string UTF16toModifiedUTF8(const TCHAR* in)
{
	// The *modified* UTF-8 encoding that JNI uses is slightly different
	// from regular UTF-8 encoding. See thee JNI specification for details.
	// https://docs.oracle.com/javase/8/docs/technotes/guides/jni/spec/types.html#modified_utf_8_strings

	static_assert(sizeof(TCHAR) == 2, "Expected TCHAR to be 2 bytes");

	std::string out;
	while (*in != 0)
	{
		TCHAR w = *in++;
		if (w >= 0x0001 && w <= 0x007f)
		{
			out.push_back(static_cast<char>(w));
		}
		else if (w == 0x0000 || w <= 0x07ff)
		{
			out.push_back(static_cast<char>(0xc0 | ((w >> 6) & 0x1f)));
			out.push_back(static_cast<char>(0x80 | ( w       & 0x3f)));
		}
		else if (w <= 0xffff)
		{
			out.push_back(static_cast<char>(0xe0 | ((w >> 12) & 0x0f)));
			out.push_back(static_cast<char>(0x80 | ((w >>  6) & 0x3f)));
			out.push_back(static_cast<char>(0x80 | ( w        & 0x3f)));
		}
	}
	return out;
}
