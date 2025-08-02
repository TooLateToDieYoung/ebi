#include <ebi.h>
#include <dsa/sta/stack.h>

struct sStaStack 
{
    pfRelease pfDataFree;

    size_t zTotal;
    size_t zUsage;
    void ** ppvData;
};

sStaStack * psStaStackMake(size_t zTotalSize, pfRelease pfDataFree)
{
    if ( 0 == zTotalSize ) { return NULL; }

    sStaStack * const psThis = (sStaStack *)malloc(sizeof(sStaStack));
    if ( NULL == psThis ) { return NULL; }

    psThis->ppvData = (void **)calloc(zTotalSize, sizeof(void *));
    if ( NULL == psThis->ppvData ) { free(psThis); return NULL; }

    psThis->pfDataFree = pfDataFree;
    psThis->zTotal = zTotalSize;
    psThis->zUsage = 0;

    return psThis;
}
void vStaStackFree(sStaStack * psThis)
{
    if ( psThis )
    {
        if ( psThis->pfDataFree )
        {
            while ( zStaStackUsage(psThis) )
            {
                psThis->pfDataFree( pvStaStackPopOut(psThis) );
            }
        }
        
        free(psThis); psThis = NULL;
    }
}

size_t zStaStackTotal(const sStaStack * psThis)
{
    return ( psThis ) ? ( psThis->zTotal ) : ( 0 ) ;
}
size_t zStaStackUsage(const sStaStack * psThis)
{
    return ( psThis ) ? ( psThis->zUsage ) : ( 0 ) ;
}

void * pvStaStackPeek(sStaStack * psThis)
{
    return zStaStackUsage(psThis) ? ( psThis->ppvData[ psThis->zUsage - 1 ] ) : ( NULL ) ;
}
void * pvStaStackPopOut(sStaStack * psThis)
{
    void * const pvRet = pvStaStackPeek(psThis);

    if ( pvRet )
    {
        psThis->zUsage--;
    }

    return pvRet;
}
sStaStack * psStaStackPushIn(sStaStack * psThis, void * pvData)
{
    if ( zStaStackTotal(psThis) == zStaStackUsage(psThis) )
    {
        return NULL;
    }

    psThis->ppvData[psThis->zUsage] = pvData;
    psThis->zUsage++;

    return psThis;
}
