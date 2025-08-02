#ifndef __EBI_H
#error "[ERROR] Make sure to explicitly include <ebi.h> first"
#else
#ifndef __EBI_STATIC_STACK_H
#define __EBI_STATIC_STACK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct sStaStack sStaStack;

sStaStack * psStaStackMake(size_t zTotalSize, pfRelease pfDataFree);
void vStaStackFree(sStaStack * psThis);

size_t zStaStackTotal(const sStaStack * psThis);
size_t zStaStackUsage(const sStaStack * psThis);

void * pvStaStackPeek(sStaStack * psThis);
void * pvStaStackPopOut(sStaStack * psThis);
sStaStack * psStaStackPushIn(sStaStack * psThis, void * pvData);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EBI_STATIC_STACK_H */
#endif
