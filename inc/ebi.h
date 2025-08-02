#ifndef __EBI_H
#define __EBI_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

typedef void (*pfRelease)(void *);
typedef int (*pfCompare)(void *, void *);

#endif /* __EBI_H */
