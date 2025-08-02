#ifndef __EBI_H
#error "[ERROR] Make sure to explicitly include <ebi.h> first"
#else
#ifndef __EBI_STATIC_QUEUE_H
#define __EBI_STATIC_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct sStaQueue sStaQueue;

sStaQueue * psStaQueueMake(size_t zTotalSize, pfRelease pfDataFree);
void vStaQueueFree(sStaQueue * psThis);

size_t zStaQueueTotal(const sStaQueue * psThis);
size_t zStaQueueUsage(const sStaQueue * psThis);

void * pvStaQueuePeekHead(sStaQueue * psThis);
void * pvStaQueuePeekTail(sStaQueue * psThis);

void * pvStaQueuePopOutHead(sStaQueue * psThis);
void * pvStaQueuePopOutTail(sStaQueue * psThis);

sStaQueue * psStaQueuePushInHead(sStaQueue * psThis, void * pvData);
sStaQueue * psStaQueuePushInTail(sStaQueue * psThis, void * pvData);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EBI_STATIC_QUEUE_H */
#endif
