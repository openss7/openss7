#include <java/lang/UnsupportedOperationException.h>

#define JvFail(string) throw new ::java::lang::UnsupportedOperationException (JvNewStringLatin1 (string))
