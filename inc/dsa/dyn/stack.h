#ifndef __EBI_H
#error "[ERROR] Make sure to explicitly include <ebi.h> first"
#else
#ifndef __EBI_DYNAMIC_STACK_H
#define __EBI_DYNAMIC_STACK_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct sDynStack sDynStack;

sDynStack * psDynStackMake(pfRelease pfDataFree);
void vDynStackFree(sDynStack * psThis);

size_t zDynStackLength(const sDynStack * psThis);

void * pvDynStackPeek(sDynStack * psThis);
void * pvDynStackPopOut(sDynStack * psThis);
sDynStack * psDynStackPushIn(sDynStack * psThis, void * pvData);

#ifdef __cplusplus2qw
}
#endif /* __cplusplus */

#endif /* __EBI_DYNAMIC_STACK_H */
#endif
