#include <ebi.h>
#include <dsa/sta/queue.h>

struct sStaQueue 
{
    pfRelease pfDataFree;

    size_t zTotal;
    size_t zUsage;
    void ** ppvData;
    size_t zHead;
    size_t zTail;
};

sStaQueue * psStaQueueMake(size_t zTotalSize, pfRelease pfDataFree)
{
    if ( 0 == zTotalSize ) { return NULL; }

    sStaQueue * const psThis = (sStaQueue *)malloc(sizeof(sStaQueue));
    if ( NULL == psThis ) { return NULL; }

    psThis->ppvData = (void **)calloc(zTotalSize, sizeof(void *));
    if ( NULL == psThis->ppvData ) { free(psThis); return NULL; }

    psThis->pfDataFree = pfDataFree;
    psThis->zTotal = zTotalSize;
    psThis->zUsage = 0;
    psThis->zHead = psThis->zTail = ( zTotalSize >> 1 );

    return psThis;
}
void vStaQueueFree(sStaQueue * psThis)
{
    if ( psThis )
    {
        if ( psThis->pfDataFree )
        {
            while ( zStaQueueUsage(psThis) )
            {
                psThis->pfDataFree( pvStaQueuePopOutTail(psThis) );
            }
        }
        
        free(psThis); psThis = NULL;
    }
}

size_t zStaQueueTotal(const sStaQueue * psThis)
{
    return ( psThis ) ? ( psThis->zTotal ) : ( 0 ) ;
}
size_t zStaQueueUsage(const sStaQueue * psThis)
{
    return ( psThis ) ? ( psThis->zUsage ) : ( 0 ) ;
}

void * pvStaQueuePeekHead(sStaQueue * psThis)
{
    return zStaQueueUsage(psThis) ? ( psThis->ppvData[ psThis->zHead ] ) : ( NULL ) ;
}
void * pvStaQueuePeekTail(sStaQueue * psThis)
{
    return zStaQueueUsage(psThis) ? ( psThis->ppvData[ psThis->zTail ] ) : ( NULL ) ;
}

void * pvStaQueuePopOutHead(sStaQueue * psThis)
{
    void * const pvRet = pvStaQueuePeekHead(psThis);

    if ( pvRet )
    {
        psThis->zHead = ( psThis->zHead == psThis->zTotal - 1 ) ? ( 0 ) : ( psThis->zHead + 1 ) ;
        psThis->zUsage--;
    }

    return pvRet;
}
void * pvStaQueuePopOutTail(sStaQueue * psThis)
{
    void * const pvRet = pvStaQueuePeekTail(psThis);

    if ( pvRet )
    {
        psThis->zTail = ( psThis->zTail == 0 ) ? ( psThis->zTotal - 1 ) : ( psThis->zTail - 1 ) ;
        psThis->zUsage--;
    }

    return pvRet;
}

sStaQueue * psStaQueuePushInHead(sStaQueue * psThis, void * pvData)
{
    if ( zStaQueueTotal(psThis) == zStaQueueUsage(psThis) )
    {
        return NULL;
    }

    if ( zStaQueueUsage(psThis) )
    {
        psThis->zHead = ( psThis->zHead == 0 ) ? ( psThis->zTotal - 1 ) : ( psThis->zHead - 1 ) ;
    }
    
    psThis->ppvData[ psThis->zHead ] = pvData;
    psThis->zUsage++;

    return psThis;
}
sStaQueue * psStaQueuePushInTail(sStaQueue * psThis, void * pvData)
{
    if ( zStaQueueTotal(psThis) == zStaQueueUsage(psThis) )
    {
        return NULL;
    }

    if ( zStaQueueUsage(psThis) )
    {
        psThis->zTail = ( psThis->zTail == psThis->zTotal - 1 ) ? ( 0 ) : ( psThis->zTail + 1 ) ;
    }
    
    psThis->ppvData[ psThis->zTail ] = pvData;
    psThis->zUsage++;

    return psThis;
}
