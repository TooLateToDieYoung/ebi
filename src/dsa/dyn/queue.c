#include <ebi.h>
#include <dsa/dyn/queue.h>

typedef struct sNode
{
    struct sNode * psPrev;
    struct sNode * psNext;
    void * pvData;
} sNode;

struct sDynQueue 
{
    pfRelease pfDataFree;

    size_t zLength;
    sNode * psHead;
    sNode * psTail;
};

sDynQueue * psDynQueueMake(pfRelease pfDataFree)
{
    sDynQueue * const psThis = (sDynQueue *)calloc(1, sizeof(sDynQueue));
    if ( psThis )
    {
        psThis->pfDataFree = pfDataFree;
    }

    return psThis;
}
void vDynQueueFree(sDynQueue * psThis)
{
    if ( psThis )
    {
        if ( psThis->pfDataFree )
        {
            while ( zDynQueueLength(psThis) ) 
            {
                psThis->pfDataFree( pvDynQueuePopOutTail(psThis) ); 
            }
        }

        free(psThis); psThis = NULL;
    }
}

size_t zDynQueueLength(const sDynQueue * psThis)
{
    return ( psThis ) ? ( psThis->zLength ) : ( 0 ) ;
}

void * pvDynQueuePeekHead(sDynQueue * psThis)
{
    return ( zDynQueueLength(psThis) ) ? ( psThis->psHead->pvData ) : ( NULL ) ;
}
void * pvDynQueuePeekTail(sDynQueue * psThis)
{
    return ( zDynQueueLength(psThis) ) ? ( psThis->psTail->pvData ) : ( NULL ) ;
}

void * pvDynQueuePopOutHead(sDynQueue * psThis)
{
    void * pvRet = NULL;
    sNode * psDrop = NULL;

    if ( zDynQueueLength(psThis) )
    {
        psDrop = psThis->psHead;
        pvRet = psDrop->pvData;
        psThis->psHead = psDrop->psNext;
        if ( psThis->psHead ) { psThis->psHead->psPrev = NULL; }
        psThis->zLength--;
        free(psDrop); psDrop = NULL;
    }

    return pvRet;
}
void * pvDynQueuePopOutTail(sDynQueue * psThis)
{
    void * pvRet = NULL;
    sNode * psDrop = NULL;

    if ( zDynQueueLength(psThis) )
    {
        psDrop = psThis->psTail;
        pvRet = psDrop->pvData;
        psThis->psTail = psDrop->psPrev;
        if ( psThis->psTail ) { psThis->psTail->psNext = NULL; }
        psThis->zLength--;
        free(psDrop); psDrop = NULL;
    }

    return pvRet;
}

sDynQueue * psDynQueuePushInHead(sDynQueue * psThis, void * pvData)
{
    sNode * psTemp;

    if ( !psThis ) { return NULL; }

    psTemp = (sNode *)calloc(1, sizeof(sNode));
    if ( !psTemp ) { return NULL; }

    psTemp->pvData = pvData;

    if ( zDynQueueLength(psThis) )
    {
        psThis->psHead->psPrev = psTemp;
        psTemp->psNext = psThis->psHead;
        psThis->psHead = psTemp;
    }
    else
    {
        psThis->psHead = psThis->psTail = psTemp;
    }

    psThis->zLength++;

    return psThis;
}
sDynQueue * psDynQueuePushInTail(sDynQueue * psThis, void * pvData)
{
    sNode * psTemp;

    if ( !psThis ) { return NULL; }

    psTemp = (sNode *)calloc(1, sizeof(sNode));
    if ( !psTemp ) { return NULL; }

    psTemp->pvData = pvData;

    if ( zDynQueueLength(psThis) )
    {
        psThis->psTail->psNext = psTemp;
        psTemp->psPrev = psThis->psTail;
        psThis->psTail = psTemp;
    }
    else
    {
        psThis->psHead = psThis->psTail = psTemp;
    }

    psThis->zLength++;

    return psThis;
}
