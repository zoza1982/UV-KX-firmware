#include <errno.h>
#include <stddef.h>

/* Minimal _sbrk to satisfy newlib; heap not used. */
void *_sbrk(ptrdiff_t increment)
{
    (void)increment;
    errno = ENOMEM;
    return (void *)-1;
}
