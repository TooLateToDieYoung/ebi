#include <ebi.h>
#include <dsa/list.h>

typedef struct
{
    size_t zXor;
    void * pvData;
} sNode;

typedef struct
{
    sNode * psPrev;
    sNode * psCurr;
    sNode * psNext;
} sTemp;

struct sList 
{
    pfRelease pfDataFree;

    size_t zLength;
    sNode * psHead;
    sNode * psTail;
    size_t zCached;
    sTemp sCached;
};

static inline sNode * _psGetNode(const sNode * const psNode, const size_t zXor)
{
    return (sNode *)( (size_t)( psNode ) ^ zXor );
}

static void _vQuickSort(sNode * psHeadL, sNode * psHead, sNode * psTail, sNode * psTailR, pfCompare pfDataCmp)
{
    if ( !psHead || !psTail || psHead == psTail ) return;

    void * pvSwap;
    sNode * psPivot = psHead;

    sTemp sL;
    sL.psPrev = psHead;
    sL.psCurr = _psGetNode(psHeadL, psHead->zXor);
    sL.psNext = _psGetNode(sL.psPrev, sL.psCurr->zXor);

    sTemp sR;
    sR.psPrev = psTailR;
    sR.psCurr = psTail;
    sR.psNext = _psGetNode(sR.psPrev, sR.psCurr->zXor);

    while ( sL.psCurr != sR.psPrev )
    {
        if ( pfDataCmp(sL.psCurr->pvData, psPivot->pvData) < 0 )
        {
            pvSwap = psPivot->pvData;
            psPivot->pvData = sL.psCurr->pvData;
            sL.psCurr->pvData = pvSwap;
            psPivot = sL.psCurr;

            sL.psPrev = sL.psCurr;
            sL.psCurr = sL.psNext;
            sL.psNext = sL.psCurr ? _psGetNode(sL.psPrev, sL.psCurr->zXor) : ( NULL ) ;
        }
        else
        {
            pvSwap = sR.psCurr->pvData;
            sR.psCurr->pvData = sR.psCurr->pvData;
            sR.psCurr->pvData = pvSwap;

            sR.psPrev = sR.psCurr;
            sR.psCurr = sR.psNext;
            sR.psNext = sR.psCurr ? _psGetNode(sR.psPrev, sR.psCurr->zXor) : ( NULL ) ;
        }
    }

    _vQuickSort(psHeadL, psHead, sR.psNext, sR.psCurr, pfDataCmp);
    _vQuickSort(sR.psCurr, sR.psPrev, psTail, psTailR, pfDataCmp);
}

sList * psListMake(pfRelease pfDataFree)
{
    sList * const psThis = (sList *)calloc(1, sizeof(sList));
    if ( psThis )
    {
        psThis->pfDataFree = pfDataFree;
    }

    return psThis;
}
void vListFree(sList * psThis)
{
    while ( zListLength(psThis) ) { psListRemove(psThis, 0); }
    free(psThis);
    psThis = NULL;
}

size_t zListLength(const sList * psThis)
{
    return ( psThis ) ? ( psThis->zLength ) : ( 0 ) ;
}

void * pvListAccess(sList * psThis, size_t zIdx)
{
    if ( zIdx >= zListLength(psThis) ) { return NULL; }

    if ( 0 == zIdx )
    {
        psThis->sCached.psPrev = NULL;
        psThis->sCached.psCurr = psThis->psHead;
        psThis->sCached.psNext = (sNode *)( psThis->psHead->zXor ); 
        psThis->zCached = 0;
    }
    else if ( zIdx == zListLength(psThis) - 1 )
    {
        psThis->sCached.psNext = NULL;
        psThis->sCached.psCurr = psThis->psTail;
        psThis->sCached.psPrev = (sNode *)( psThis->psTail->zXor );  
        psThis->zCached = zListLength(psThis) - 1;
    }
    else
    {
        for ( ; zIdx > psThis->zCached; psThis->zCached++ ) 
        { 
            psThis->sCached.psPrev = psThis->sCached.psCurr;
            psThis->sCached.psCurr = psThis->sCached.psNext;
            psThis->sCached.psNext = _psGetNode(psThis->sCached.psPrev, psThis->sCached.psCurr->zXor); 
        }
        for ( ; zIdx < psThis->zCached; psThis->zCached-- )
        { 
            psThis->sCached.psNext = psThis->sCached.psCurr; 
            psThis->sCached.psCurr = psThis->sCached.psPrev;
            psThis->sCached.psPrev = _psGetNode(psThis->sCached.psNext, psThis->sCached.psCurr->zXor); 
        }
    }

    return psThis->sCached.psCurr->pvData;
}
sList * psListInsert(sList * psThis, size_t zIdx, void * pvData)
{
    sNode * psTemp;

    if ( !psThis ) { return NULL; } /* memory leakage: pvData */

    psTemp = (sNode *)calloc(1, sizeof(sNode));
    if ( !psTemp ) { return NULL; } /* memory leakage: pvData */
    psTemp->pvData = pvData;

    if ( 0 == zListLength(psThis) )
    {
        psThis->sCached.psCurr = psThis->psHead = psThis->psTail = psTemp;
        psThis->zCached = 0;
    }
    else if ( 0 == zIdx )
    {
        psTemp->zXor = (size_t)( psThis->psHead );
        psThis->psHead->zXor ^= (size_t)( psTemp );
        psThis->sCached.psNext = psThis->psHead;
        psThis->sCached.psCurr = psThis->psHead = psTemp;
        psThis->sCached.psPrev = NULL;
        psThis->zCached = 0;
    }
    else if ( zIdx >= zListLength(psThis) )
    {
        psTemp->zXor = (size_t)( psThis->psTail );
        psThis->psTail->zXor ^= (size_t)( psTemp );
        psThis->sCached.psPrev = psThis->psTail;
        psThis->sCached.psCurr = psThis->psTail = psTemp;
        psThis->sCached.psNext = NULL;
        psThis->zCached = zListLength(psThis);
    }
    else
    {
        pvListAccess(psThis, zIdx);

        psTemp->zXor = ( (size_t)( psThis->sCached.psPrev ) ^ (size_t)( psThis->sCached.psCurr ) );
        psThis->sCached.psPrev->zXor ^= (size_t)( psThis->sCached.psCurr );
        psThis->sCached.psPrev->zXor ^= (size_t)( psTemp );
        psThis->sCached.psCurr->zXor ^= (size_t)( psThis->sCached.psPrev );
        psThis->sCached.psCurr->zXor ^= (size_t)( psTemp );

        psThis->sCached.psNext = psThis->sCached.psCurr;
        psThis->sCached.psCurr = psTemp;
    }

    psThis->zLength++;
    return psThis;
}
sList * psListRemove(sList * psThis, size_t zIdx)
{
    sNode * psTemp;

    if ( zIdx >= zListLength(psThis) ) { return psThis; }

    pvListAccess(psThis, zIdx);

    psTemp = psThis->sCached.psCurr;

    if ( zListLength(psThis) == 1 )
    {
        psThis->psHead = NULL;
        psThis->psTail = NULL;
        psThis->sCached.psPrev = NULL;
        psThis->sCached.psCurr = NULL;
        psThis->sCached.psNext = NULL;
        psThis->zCached = 0;
    }
    else if ( zIdx == 0 )
    {
        psThis->sCached.psNext->zXor ^= (size_t)( psTemp );

        psThis->psHead = psThis->sCached.psCurr = psThis->sCached.psNext;
        psThis->sCached.psNext = (sNode *)( psThis->sCached.psNext->zXor );
    }
    else if ( zIdx == zListLength(psThis) - 1 )
    {
        psThis->sCached.psPrev->zXor ^= (size_t)( psTemp );

        psThis->psTail = psThis->sCached.psCurr = psThis->sCached.psPrev;
        psThis->sCached.psPrev = (sNode *)( psThis->sCached.psPrev->zXor );

        psThis->zCached--;
    }
    else
    {
        psThis->sCached.psPrev->zXor ^= (size_t)( psTemp );
        psThis->sCached.psPrev->zXor ^= (size_t)( psThis->sCached.psNext );

        psThis->sCached.psNext->zXor ^= (size_t)( psTemp );
        psThis->sCached.psNext->zXor ^= (size_t)( psThis->sCached.psPrev );

        psThis->sCached.psCurr = psThis->sCached.psNext;
        psThis->sCached.psNext = _psGetNode(psTemp, psThis->sCached.psNext->zXor);
    }

    if ( psThis->pfDataFree ) { psThis->pfDataFree(psTemp->pvData); }
    free(psTemp);
    psTemp = NULL;

    psThis->zLength--;
    return psThis;
}
sList * psListChange(sList * psThis, size_t zIdx, void * pvData)
{
    if ( zIdx >= zListLength(psThis) ) { return NULL; }

    pvListAccess(psThis, zIdx);

    if ( psThis->pfDataFree ) { psThis->pfDataFree(psThis->sCached.psCurr->pvData); }
    psThis->sCached.psCurr->pvData = pvData;

    return psThis;
}

sList * psListInvert(sList * psThis)
{
    sNode * psTemp;

    if ( zListLength(psThis) > 1 )
    {
        psTemp = psThis->psHead;
        psThis->psHead = psThis->psTail;
        psThis->psTail = psTemp;

        psTemp = psThis->sCached.psPrev;
        psThis->sCached.psPrev = psThis->sCached.psNext;
        psThis->sCached.psNext = psTemp;
        psThis->zCached = psThis->zLength - psThis->zCached - 1;
    }

    return psThis;
}
sList * psListSortBy(sList * psThis, pfCompare pfDataCmp)
{
    if ( pfDataCmp && zListLength(psThis) > 1 )
    {
        _vQuickSort(NULL, psThis->psHead, psThis->psTail, NULL, pfDataCmp);
    }

    return psThis;
}
