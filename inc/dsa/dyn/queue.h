#ifndef __EBI_H
#error "[ERROR] Make sure to explicitly include <ebi.h> first"
#else
#ifndef __EBI_DYNAMIC_QUEUE_H
#define __EBI_DYNAMIC_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct sDynQueue sDynQueue;

sDynQueue * psDynQueueMake(pfRelease pfDataFree);
void vDynQueueFree(sDynQueue * psThis);

size_t zDynQueueLength(const sDynQueue * psThis);

void * pvDynQueuePeekHead(sDynQueue * psThis);
void * pvDynQueuePeekTail(sDynQueue * psThis);

void * pvDynQueuePopOutHead(sDynQueue * psThis);
void * pvDynQueuePopOutTail(sDynQueue * psThis);

sDynQueue * psDynQueuePushInHead(sDynQueue * psThis, void * pvData);
sDynQueue * psDynQueuePushInTail(sDynQueue * psThis, void * pvData);

#ifdef __cplusplus2qw
}
#endif /* __cplusplus */

#endif /* __EBI_DYNAMIC_QUEUE_H */
#endif
