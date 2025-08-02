#include <ebi.h>
#include <dsa/dyn/stack.h>

typedef struct sNode
{
    struct sNode * psNext;
    void * pvData;
} sNode;

struct sDynStack 
{
    pfRelease pfDataFree;

    size_t zLength;
    sNode * psTop;
};

sDynStack * psDynStackMake(pfRelease pfDataFree)
{
    sDynStack * const psThis = (sDynStack *)calloc(1, sizeof(sDynStack));
    if ( psThis )
    {
        psThis->pfDataFree = pfDataFree;
    }

    return psThis;
}
void vDynStackFree(sDynStack * psThis)
{
    if ( psThis )
    {
        if ( psThis->pfDataFree )
        {
            while ( zDynStackLength(psThis) ) 
            {
                psThis->pfDataFree( pvDynStackPopOut(psThis) ); 
            }
        }

        free(psThis); psThis = NULL;
    }
}

size_t zDynStackLength(const sDynStack * psThis)
{
    return ( psThis ) ? ( psThis->zLength ) : ( 0 ) ;
}

void * pvDynStackPeek(sDynStack * psThis)
{
    return ( zDynStackLength(psThis) ) ? ( psThis->psTop->pvData ) : ( NULL ) ;
}
void * pvDynStackPopOut(sDynStack * psThis)
{
    void * pvRet = NULL;
    sNode * psDrop = NULL;

    if ( zDynStackLength(psThis) )
    {
        psDrop = psThis->psTop;
        pvRet = psDrop->pvData;
        psThis->psTop = psDrop->psNext;
        psThis->zLength--;
        free(psDrop); psDrop = NULL;
    }

    return pvRet;
}
sDynStack * psDynStackPushIn(sDynStack * psThis, void * pvData)
{
    sNode * psTemp;

    if ( !psThis ) { return NULL; }

    psTemp = (sNode *)malloc(sizeof(sNode));
    if ( !psTemp ) { return NULL; }

    psTemp->pvData = pvData;
    psTemp->psNext = psThis->psTop;
    psThis->psTop = psTemp;
    psThis->zLength++;

    return psThis;
}
