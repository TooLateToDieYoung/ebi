#include <ebi.h>
#include <dsa/dict.h>
#include <str/str.h>

typedef struct sNode
{
    xStr xKey;
    void * pvData;
    struct sNode * psLeafP;
    struct sNode * psLeafL;
    struct sNode * psLeafR;
    signed char nHeight;
} sNode;

struct sDict
{
    pfRelease pfDataFree;

    size_t zLength;
    sNode * psRoot;
    sNode * psTemp;
};

static signed int _nHeight(const sNode * psThis)
{
    return ( psThis ) ? ( psThis->nHeight ) : ( 0 ) ;
}
static sNode * _psLRotateR(sNode * psThis)
{
    psThis->psLeafL->psLeafP = psThis->psLeafP;
    psThis->psLeafP = psThis->psLeafL;
    psThis->psLeafL = psThis->psLeafP->psLeafR; /* maybe NULL */
    if ( psThis->psLeafL ) { psThis->psLeafL->psLeafP = psThis; }
    psThis->psLeafP->psLeafR = psThis;

    psThis->nHeight = ( psThis->psLeafL ) ? ( 1 + psThis->psLeafL->nHeight ) : ( psThis->psLeafR ) ? ( 1 + psThis->psLeafR->nHeight ) : ( 1 ) ;

    return psThis->psLeafP;
}
static sNode * _psLRotateL(sNode * psThis)
{
    sNode * const psTemp = psThis->psLeafL;

    psThis->psLeafL = psTemp->psLeafR;
    psThis->psLeafL->psLeafP = psThis;
    psTemp->psLeafR = psThis->psLeafL->psLeafL; /* maybe NULL */
    if ( psTemp->psLeafR ) { psTemp->psLeafR->psLeafP = psTemp; }
    psThis->psLeafL->psLeafL = psTemp;
    psTemp->psLeafP = psThis->psLeafL;

    psTemp->nHeight = ( psTemp->psLeafR ) ? ( 1 + psTemp->psLeafR->nHeight ) : ( psTemp->psLeafL ) ? ( 1 + psTemp->psLeafL->nHeight ) : ( 1 ) ; 
    psThis->psLeafL->nHeight = ( 1 + psTemp->nHeight ) ;
    psThis->nHeight = ( 1 + psThis->psLeafL->nHeight ) ;

    return _psLRotateR(psThis);
}

static sNode * _psRRotateL(sNode * psThis)
{
    psThis->psLeafR->psLeafP = psThis->psLeafP;
    psThis->psLeafP = psThis->psLeafR;
    psThis->psLeafR = psThis->psLeafP->psLeafL; /* maybe NULL */
    if ( psThis->psLeafR ) { psThis->psLeafR->psLeafP = psThis; }
    psThis->psLeafP->psLeafL = psThis;

    psThis->nHeight = ( psThis->psLeafR ) ? ( 1 + psThis->psLeafR->nHeight ) : ( psThis->psLeafL ) ? ( 1 + psThis->psLeafL->nHeight ) : ( 1 ) ; 

    return psThis->psLeafP;
}
static sNode * _psRRotateR(sNode * psThis)
{
    sNode * const psTemp = psThis->psLeafR;

    psThis->psLeafR = psTemp->psLeafL;
    psThis->psLeafR->psLeafP = psThis;
    psTemp->psLeafL = psThis->psLeafR->psLeafR; /* maybe NULL */
    if ( psTemp->psLeafL ) { psTemp->psLeafL->psLeafP = psTemp; }
    psThis->psLeafR->psLeafR = psTemp;
    psTemp->psLeafP = psThis->psLeafR;

    psTemp->nHeight = ( psThis->psLeafL ) ? ( 1 + psThis->psLeafL->nHeight ) : ( psThis->psLeafR ) ? ( 1 + psThis->psLeafR->nHeight ) : ( 1 ) ;
    psThis->psLeafR->nHeight = ( 1 + psTemp->nHeight ) ;
    psThis->nHeight = ( 1 + psThis->psLeafR->nHeight ) ;

    return _psRRotateL(psThis);
}
static sNode * _psReHeight(sNode * psNode)
{
    signed char nHeightL;
    signed char nHeightR;

    do {
        nHeightL = _nHeight(psNode->psLeafL);
        nHeightR = _nHeight(psNode->psLeafR);

        switch ( nHeightL - nHeightR )
        {
            case +2:
            {
                if ( _nHeight(psNode->psLeafL->psLeafL) > _nHeight(psNode->psLeafL->psLeafR) ) 
                {
                    if ( !psNode->psLeafP )
                    {
                        psNode = _psLRotateR(psNode);
                    }
                    else if ( psNode == psNode->psLeafP->psLeafL )
                    {
                        psNode->psLeafP->psLeafL = _psLRotateR(psNode);
                    }
                    else
                    {
                        psNode->psLeafP->psLeafR = _psLRotateR(psNode);
                    }
                }
                else
                {
                    if ( !psNode->psLeafP )
                    {
                        psNode = _psLRotateL(psNode);
                    }
                    else if ( psNode == psNode->psLeafP->psLeafL )
                    {
                        psNode->psLeafP->psLeafL = _psLRotateL(psNode);
                    }
                    else
                    {
                        psNode->psLeafP->psLeafR = _psLRotateL(psNode);
                    }
                }
                break;
            }
            case -2:
            {
                if ( _nHeight(psNode->psLeafR->psLeafR) > _nHeight(psNode->psLeafR->psLeafL) ) 
                {
                    if ( !psNode->psLeafP )
                    {
                        psNode = _psRRotateL(psNode);
                    }
                    else if ( psNode == psNode->psLeafP->psLeafL )
                    {
                        psNode->psLeafP->psLeafL = _psRRotateL(psNode);
                    }
                    else
                    {
                        psNode->psLeafP->psLeafR = _psRRotateL(psNode);
                    }
                }
                else
                {
                    if ( !psNode->psLeafP )
                    {
                        psNode = _psRRotateR(psNode);
                    }
                    else if ( psNode == psNode->psLeafP->psLeafL )
                    {
                        psNode->psLeafP->psLeafL = _psRRotateR(psNode);
                    }
                    else
                    {
                        psNode->psLeafP->psLeafR = _psRRotateR(psNode);
                    }
                }
                break;
            }
            default:
            {
                psNode->nHeight = ( nHeightL > nHeightR ) ? ( 1 + nHeightL ) : ( 1 + nHeightR );
                break;
            }
        }

        if ( !psNode->psLeafP ) { break; }
        else { psNode = psNode->psLeafP; }
    } while ( 1 );

    return psNode;
}
static sNode * _psMinNode(sNode * psNode)
{
    while ( psNode->psLeafL ) { psNode = psNode->psLeafL; }
    return psNode;
}
static sNode * _psMaxNode(sNode * psNode)
{
    while ( psNode->psLeafR ) { psNode = psNode->psLeafR; }
    return psNode;
}

sDict * psDictMake(pfRelease pfDataFree)
{
    sDict * const psThis = (sDict *)calloc(1, sizeof(sDict));
    if ( psThis )
    {
        psThis->pfDataFree = pfDataFree;
    }

    return psThis;
}
void vDictFree(sDict * psThis)
{
    while ( zDictLength(psThis) ) { psDictRemove(psThis, pnDictRootKey(psThis)); }
    free(psThis);
    psThis = NULL;
}

size_t zDictLength(const sDict * psThis)
{
    return ( psThis ) ? ( psThis->zLength ) : ( 0 ) ;
}
size_t zDictHeight(const sDict * psThis)
{
    return (size_t)_nHeight(psThis->psRoot);
}

void * pvDictAccess(sDict * psThis, const char * pnKey)
{
    int nRet;

    if ( !psThis || !pnKey ) { return NULL; }

    psThis->psTemp = psThis->psRoot;
    do {
        nRet = nStrCmp(psThis->psTemp->xKey, pnKey);
        if ( 0 == nRet ) { return psThis->psTemp->pvData; }
        psThis->psTemp = ( nRet > 0 ) ? ( psThis->psTemp->psLeafL ) : ( psThis->psTemp->psLeafR ) ;
    } while ( psThis->psTemp );

    return NULL;
}
sDict * psDictInsert(sDict * psThis, const char * pnKey, void * pvData)
{
    int nRet;

    if ( !psThis || !pnKey ) { return NULL; }

    if ( NULL == psThis->psRoot )
    {
        psThis->psRoot = psThis->psTemp = (sNode *)calloc(1, sizeof(sNode));
        if ( NULL == psThis->psRoot ) { return NULL; } /* memory leakage: pvData */
    }
    else
    {
        psThis->psTemp = psThis->psRoot;
        do {
            nRet = nStrCmp(psThis->psTemp->xKey, pnKey);
            if ( 0 == nRet ) { return psDictChange(psThis, pnKey, pvData); }
            if ( nRet > 0 )
            {
                if ( psThis->psTemp->psLeafL ) { psThis->psTemp = psThis->psTemp->psLeafL; }
                else
                {
                    psThis->psTemp->psLeafL = (sNode *)calloc(1, sizeof(sNode));
                    if ( NULL == psThis->psTemp->psLeafL ) { return NULL; } /* memory leakage: pvData */
                    psThis->psTemp->psLeafL->psLeafP = psThis->psTemp;
                    psThis->psTemp = psThis->psTemp->psLeafL;
                    break;
                }
            }
            else
            {
                if ( psThis->psTemp->psLeafR ) { psThis->psTemp = psThis->psTemp->psLeafR; }
                else
                {
                    psThis->psTemp->psLeafR = (sNode *)calloc(1, sizeof(sNode));
                    if ( NULL == psThis->psTemp->psLeafR ) { return NULL; } /* memory leakage: pvData */
                    psThis->psTemp->psLeafR->psLeafP = psThis->psTemp;
                    psThis->psTemp = psThis->psTemp->psLeafR;
                    break;
                }
            }
        } while ( psThis->psTemp );
    }

    psThis->psTemp->xKey = xStrMake(pnKey);
    if ( NULL == psThis->psTemp->xKey ) 
    { 
        free(psThis->psTemp);
        psThis->psTemp = NULL;
        return NULL; 
    } /* memory leakage: pvData */
    psThis->psTemp->pvData = pvData;
    psThis->psRoot = _psReHeight(psThis->psTemp);
    psThis->zLength++;
    return psThis;
}
sDict * psDictRemove(sDict * psThis, const char * pnKey)
{
    sNode * psDrop;

    if ( !psThis || !pnKey ) { return NULL; }

    pvDictAccess(psThis, pnKey);

    if ( psThis->psTemp )
    {
        vStrFree(psThis->psTemp->xKey);
        if ( psThis->pfDataFree ) { psThis->pfDataFree(psThis->psTemp->pvData); }

        psDrop = psThis->psTemp;
        while ( psDrop->psLeafL || psDrop->psLeafR )
        {
            if ( psDrop->psLeafL && psDrop->psLeafR )
            {
                psDrop = ( psDrop->psLeafL->nHeight > psDrop->psLeafR->nHeight ) ? _psMaxNode(psDrop->psLeafL) : _psMinNode(psDrop->psLeafR) ;
            }
            else if ( psDrop->psLeafL )
            {
                psDrop = _psMaxNode(psDrop->psLeafL);
            }
            else
            {
                psDrop = _psMinNode(psDrop->psLeafR);
            }

            psThis->psTemp->xKey = psDrop->xKey;
            psThis->psTemp->pvData = psDrop->pvData;
            psThis->psTemp = psDrop;
        }
    }

    if ( psDrop == psThis->psRoot )
    {
        psThis->psRoot = NULL;
    }
    else if ( psDrop == psDrop->psLeafP->psLeafL )
    {
        psDrop->psLeafP->psLeafL = NULL;
        psThis->psRoot = _psReHeight(psDrop->psLeafP);
    }
    else
    {
        psDrop->psLeafP->psLeafR = NULL;
        psThis->psRoot = _psReHeight(psDrop->psLeafP);
    }

    psThis->zLength--;
    free(psDrop); psDrop = NULL;
    psThis->psTemp = NULL;
    return psThis;
}
sDict * psDictChange(sDict * psThis, const char * pnKey, void * pvData)
{
    if ( !psThis || !pnKey ) { return NULL; }

    pvDictAccess(psThis, pnKey);

    if ( NULL == psThis->psTemp )
    {
        return psDictInsert(psThis, pnKey, pvData);
    }

    if ( psThis->pfDataFree ) 
    { 
        psThis->pfDataFree(psThis->psTemp->pvData); 
    }
    psThis->psTemp->pvData = pvData;

    return psThis;
}

const char * pnDictRootKey(sDict * psThis)
{
    if ( 0 == zDictLength(psThis) ) { return NULL; }
    psThis->psTemp = psThis->psRoot;
    return psThis->psRoot->xKey;
}
const char * pnDictNextKey(sDict * psThis, const char * pnKey)
{
    if ( !psThis || !pnKey ) { return NULL; }

    if ( NULL == psThis->psTemp ) { psThis->psTemp = psThis->psRoot; }
    else if ( psThis->psTemp->psLeafL ) { psThis->psTemp = psThis->psTemp->psLeafL; }
    else if ( psThis->psTemp->psLeafR ) { psThis->psTemp = psThis->psTemp->psLeafR; }
    else
    {
        while ( psThis->psTemp->psLeafP ) 
        {
            if ( 
                ( psThis->psTemp->psLeafP->psLeafL == psThis->psTemp ) && 
                ( psThis->psTemp->psLeafP->psLeafR != NULL )
            ) {
                psThis->psTemp = psThis->psTemp->psLeafP->psLeafR;
                break;
            }

            psThis->psTemp = psThis->psTemp->psLeafP;
        }

        if ( NULL == psThis->psTemp->psLeafP )
        {
            psThis->psTemp = NULL;
        }
    }

    return ( psThis->psTemp ) ? ( psThis->psTemp->xKey ) : ( NULL ) ;
}
