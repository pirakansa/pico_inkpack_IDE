#include <errno.h>
#include <stddef.h>

extern "C" int _getentropy(void *buffer, size_t length) {
    (void)buffer;
    (void)length;
    errno = ENOSYS;
    return -1;
}
