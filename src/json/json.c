#include <ebi.h>
#include <json/json.h>
#include <str/str.h>
#include <dsa/list.h>
#include <dsa/dict.h>
#include <dsa/dyn/stack.h>

#include <ctype.h>
#include <stdio.h>

struct sJson 
{
    eJson eType;
    union {
        bool ubBoo;
        long unInt;
        double unFlt;
        xStr uxStr;
        sList * upsArr;
        sDict * upsObj;
    };
};

static sJson * _psMake(eJson eType)
{
    sJson * const psThis = (sJson *)malloc(sizeof(sJson));

    if ( psThis )
    {
        psThis->eType = eType;
    }

    return psThis;
}
static int _nStringifySize(const sJson * psThis)
{
    int nRet = 0;
    size_t zIdx = 0;
    const char * pnKey = NULL;

    switch ( eJsonType(psThis) )
    {
    case JBoo: return ( psThis->ubBoo ) ? snprintf(NULL, 0, "true") : snprintf(NULL, 0, "false") ;
    case JInt: return snprintf(NULL, 0, "%ld", psThis->unInt);
    case JFlt: return snprintf(NULL, 0, "%lf", psThis->unFlt);
    case JStr: return snprintf(NULL, 0, "\"%s\"", psThis->uxStr);
    case JArr: 
    {
        nRet = 2; /* snprintf(NULL, 0, "[]"); */
        for ( zIdx = 0; zListLength(psThis->upsArr) > zIdx; ++zIdx )
        {
            nRet += _nStringifySize(pvListAccess(psThis->upsArr, zIdx));
        }
        if ( zListLength(psThis->upsArr) ) /* ',' */
        {
            nRet += ( zListLength(psThis->upsArr) - 1 );
        }
        return nRet;
    }
    case JObj:
    {
        nRet = 2; /* snprintf(NULL, 0, "{}"); */
        for (
            pnKey = pnDictRootKey(psThis->upsObj);
            pnKey;
            pnKey = pnDictNextKey(psThis->upsObj, pnKey)
        ) {
            nRet += snprintf(NULL, 0, "\"%s\":", pnKey);
            nRet += _nStringifySize(pvDictAccess(psThis->upsObj, pnKey));
        }
        if ( zDictLength(psThis->upsObj) ) /* ',' */
        {
            nRet += ( zDictLength(psThis->upsObj) - 1 );
        }
        return nRet;
    }
    
    default:
        break;
    }

    return snprintf(NULL, 0, "null");
}
static sJson * _psParse(char * pnSrc, char ** ppnEnd)
{
    sJson * psRet = NULL;
    char * pnEnd;
    char * pnKeyHead;
    char * pnKeyTail;

    switch ( *pnSrc )
    {
    case 'f':
        if ( 0 != strncmp(pnSrc, "false", 5) ) { goto __error; }
        psRet = psJBooMake(false);
        *ppnEnd = &pnSrc[5];
        break;

    case 't':
        if ( 0 != strncmp(pnSrc, "true", 4) ) { goto __error; }
        psRet = psJBooMake(true);
        *ppnEnd = &pnSrc[4];
        break;

    case '+':
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        for ( pnEnd = &pnSrc[1]; isdigit(*pnEnd); ++pnEnd ) { }
        switch ( *pnEnd )
        {
        case '.':
        case 'e':
        case 'E':
            psRet = psJFltMake(strtof(pnSrc, ppnEnd));
            break;
        
        default:
            psRet = psJIntMake(strtol(pnSrc, ppnEnd, 10));
            break;
        }
        break;
        
    case '"':
        *ppnEnd = strchr(&pnSrc[1], '"');
        // if ( NULL == *ppnEnd ) { return NULL; }
        **ppnEnd = '\0';
        psRet = psJStrMake(&pnSrc[1]);
        **ppnEnd = '"';
        *ppnEnd = *ppnEnd + 1;
        break;

    case '[':
        *ppnEnd = strchr(&pnSrc[1], ']');
        // if ( NULL == *ppnEnd ) { return NULL; }
        psRet = psJArrMake();
        for ( pnEnd = &pnSrc[1]; *ppnEnd > pnEnd; ++pnEnd )
        {
            psJArrInsert(psRet, 0, _psParse(pnEnd, &pnEnd));
            if ( NULL == pnEnd ) { goto __error; }
        }
        psRet = psJArrInvert(psRet);
        *ppnEnd = *ppnEnd + 1;
        break;

    case '{':
        *ppnEnd = strchr(&pnSrc[1], '}');
        // if ( NULL == *ppnEnd ) { return NULL; }
        psRet = psJObjMake();
        for ( pnEnd = &pnSrc[1]; *ppnEnd > pnEnd; ++pnEnd )
        {
            pnKeyHead = pnEnd;
            if ( *pnKeyHead != '"' ) { goto __error; }
            pnKeyTail = strchr(pnKeyHead + 1, '"');
            // if ( NULL == pnKeyTail ) { goto __error; }
            pnEnd = ( pnKeyTail + 1 );
            if ( *pnEnd != ':' ) { goto __error; } 
            else { ++pnEnd; }
            *pnKeyTail = '\0';
            psJObjInsert(psRet, pnKeyHead + 1, _psParse(pnEnd, &pnEnd));
            if ( NULL == pnEnd ) { goto __error; }
            *pnKeyTail = '"';
        }
        *ppnEnd = *ppnEnd + 1;
        break;
    
    default: /* 'n' */
        break;
    }

    return psRet;

__error:
    vJsonFree(psRet); psRet = NULL;
    *ppnEnd = NULL;

    return psRet;
}

int nJsonStringify(const sJson * psThis, char * pnDst)
{
    int nRet = 0;
    size_t zIdx = 0;
    const char * pnKey = NULL;

    if ( NULL == pnDst ) { return _nStringifySize(psThis); }

    switch ( eJsonType(psThis) )
    {
    case JBoo: return ( psThis->ubBoo ) ? sprintf(pnDst, "true") : sprintf(pnDst, "false") ;
    case JInt: return sprintf(pnDst, "%ld", psThis->unInt);
    case JFlt: return sprintf(pnDst, "%lf", psThis->unFlt);
    case JStr: return sprintf(pnDst, "\"%s\"", psThis->uxStr);
    case JArr: 
    {
        pnDst[nRet] = '['; nRet++;
        for ( zIdx = 0; zListLength(psThis->upsArr) > zIdx; ++zIdx )
        {
            nRet += nJsonStringify(pvListAccess(psThis->upsArr, zIdx), &pnDst[nRet]);
            pnDst[nRet] = ','; nRet++;
        }
        if ( zListLength(psThis->upsArr) ) { nRet--; }
        pnDst[nRet] = ']'; nRet++;
        return nRet;
    }
    case JObj:
    {
        pnDst[nRet] = '{'; nRet++;
        for (
            pnKey = pnDictRootKey(psThis->upsObj);
            pnKey;
            pnKey = pnDictNextKey(psThis->upsObj, pnKey)
        ) {
            nRet += sprintf(&pnDst[nRet], "\"%s\":", pnKey);
            nRet += nJsonStringify(pvDictAccess(psThis->upsObj, pnKey), &pnDst[nRet]);
            pnDst[nRet] = ','; nRet++;
        }
        if ( zDictLength(psThis->upsObj) ) { nRet--; }
        pnDst[nRet] = '}'; nRet++;
        return nRet;
    }
    
    default:
        break;
    }

    return sprintf(pnDst, "null");
}
sJson * psJsonParse(const char * pnSrc)
{
    sJson * psRet;
    size_t zCur, zIdx, zLen;
    char * pnCpy;
    char * pnEnd;
    sDynStack * psChk;

    psChk = psDynStackMake(NULL);
    for ( zLen = 0; pnSrc[zLen]; ++zLen )
    {
        switch ( pnSrc[zLen] )
        {
        case '"':
            if ( zDynStackLength(psChk) && '"' == *(const char *)pvDynStackPeek(psChk) )
            {
                pvDynStackPopOut(psChk);
            }
            else
            {
                psDynStackPushIn(psChk, (void *)&pnSrc[zLen]);
            }
            break;

        case '[':
        case '{':
            if ( 0 == zDynStackLength(psChk) || '"' != *(const char *)pvDynStackPeek(psChk) )
            {
                psDynStackPushIn(psChk, (void *)&pnSrc[zLen]);
            }
            break;

        case ']':
            if ( zDynStackLength(psChk) )
            {
                if ( '[' == *(const char *)pvDynStackPeek(psChk) )
                {
                    pvDynStackPopOut(psChk);
                }
                else if ( '"' != *(const char *)pvDynStackPeek(psChk) )
                {
                    vDynStackFree(psChk);
                    return NULL;
                }
            }
            else
            {
                vDynStackFree(psChk);
                return NULL;
            }
            break;

        case '}':
            if ( zDynStackLength(psChk) )
            {
                if ( '{' == *(const char *)pvDynStackPeek(psChk) )
                {
                    pvDynStackPopOut(psChk);
                }
                else if ( '"' != *(const char *)pvDynStackPeek(psChk) )
                {
                    vDynStackFree(psChk);
                    return NULL;
                }
            }
            else
            {
                vDynStackFree(psChk);
                return NULL;
            }
            break;

        default:
            break;
        }
    }
    if ( 0 != zDynStackLength(psChk) ) 
    { 
        vDynStackFree(psChk);
        return NULL; 
    }
    
    vDynStackFree(psChk);
    if ( !zLen ) { return NULL; }

    pnCpy = (char *)malloc(1 + zLen);
    if ( !pnCpy ) { return NULL; }

    for ( zIdx = 0, zCur = 0; zLen > zIdx; ++zIdx )
    {
        if ( isprint(pnSrc[zIdx]) && ' ' != pnSrc[zIdx] )
        {
            pnCpy[zCur] = pnSrc[zIdx];
            zCur++;
        }
    }
    pnCpy[zCur] = '\0';

    psRet = _psParse(pnCpy, &pnEnd);

    if ( NULL == pnEnd )
    {
        vJsonFree(psRet); psRet = NULL;
    }

    free(pnCpy);

    return psRet;
}
sJson * psJsonClone(const sJson * psSrc)
{
    int nSize;
    xStr xBuf;
    sJson * psRet;

    if ( NULL == psSrc ) { return NULL; }
    nSize = _nStringifySize(psSrc);
    xBuf = xStrAlloc(nSize + 1);
    if ( NULL == xBuf ) { return NULL; }
    xBuf[nSize] = '\0';
    nJsonStringify(psSrc, xBuf);
    psRet = psJsonParse(xBuf);
    vStrFree(xBuf);
    return psRet;
}
sJson * psJBooMake(bool bVal)
{
    sJson * const psThis = _psMake(JBoo);

    if ( psThis )
    {
        psThis->ubBoo = bVal;
    }

    return psThis;
}
sJson * psJIntMake(long nVal)
{
    sJson * const psThis = _psMake(JInt);

    if ( psThis )
    {
        psThis->unInt = nVal;
    }

    return psThis;
}
sJson * psJFltMake(double nVal)
{
    sJson * const psThis = _psMake(JFlt);

    if ( psThis )
    {
        psThis->unFlt = nVal;
    }

    return psThis;
}
sJson * psJStrMake(const char * pnVal)
{
    sJson * psThis = _psMake(JStr);

    if ( psThis ) 
    {
        psThis->uxStr = xStrMake(pnVal);
        if ( NULL == psThis->uxStr )
        {
            vJsonFree(psThis);
            psThis = NULL;
        }
    }

    return psThis;
}
sJson * psJArrMake(void)
{
    sJson * psThis = _psMake(JArr);

    if ( psThis ) 
    {
        psThis->upsArr = psListMake(( void (*)(void *) )vJsonFree);
        if ( NULL == psThis->upsArr )
        {
            vJsonFree(psThis);
            psThis = NULL;
        }
    }

    return psThis;
}
sJson * psJObjMake(void)
{
    sJson * psThis = _psMake(JObj);

    if ( psThis ) 
    {
        psThis->upsObj = psDictMake(( void (*)(void *) )vJsonFree);
        if ( NULL == psThis->upsObj )
        {
            vJsonFree(psThis);
            psThis = NULL;
        }
    }

    return psThis;
}
void vJsonFree(sJson * psThis)
{
    if ( NULL == psThis ) { return; }

    switch ( eJsonType(psThis) )
    {
        case JStr: { vStrFree(psThis->uxStr); psThis->uxStr = NULL; break; }
        case JArr: { vListFree(psThis->upsArr); psThis->upsArr = NULL; break; }
        case JObj: { vDictFree(psThis->upsObj); psThis->upsObj = NULL; break; }
        default: { break; }
    }

    free(psThis);
}
eJson eJsonType(const sJson * psThis)
{
    return ( psThis ) ? ( psThis->eType ) : ( JNul ) ;
}

sJson * psJBooSet(sJson * psThis, bool bVal)
{
    switch ( eJsonType(psThis) )
    {
        case JBoo: { psThis->ubBoo = bVal; break; }
        case JInt: { psThis->unInt = (long)bVal; break; }
        case JFlt: { psThis->unFlt = (double)bVal; break; }
        case JStr: { psThis->uxStr = xStrCpy(psThis->uxStr, ( ( bVal ) ? ( "true" ) : ( "false" ) )); break; }
        default: { break; }
    }

    return psThis;
}
bool bJBooGet(const sJson * psThis)
{
    switch ( eJsonType(psThis) )
    {
        case JBoo: { return psThis->ubBoo; }
        case JInt: { return (bool)( psThis->unInt ); }
        case JFlt: { return (bool)( psThis->unFlt ); }
        case JStr: { return ( 0 == nStrCmp(psThis->uxStr, "true") ); }
        case JArr: { return ( 0 != zListLength(psThis->upsArr) ); }
        case JObj: { return ( 0 != zDictLength(psThis->upsObj) ); }
        default: { return false; }
    }
}

sJson * psJIntSet(sJson * psThis, long nVal)
{
    switch ( eJsonType(psThis) )
    {
        case JInt: { psThis->unInt = nVal; break; }
        case JBoo: { psThis->ubBoo = (bool)nVal; break; }
        case JFlt: { psThis->unFlt = (double)nVal; break; }
        case JStr: { psThis->uxStr[ snprintf(psThis->uxStr, zStrBufferSize(psThis->uxStr), "%ld", nVal) ] = '\0'; break; }
        default: { break; }
    }

    return psThis;
}
long nJIntGet(const sJson * psThis)
{
    switch ( eJsonType(psThis) )
    {
        case JInt: { return psThis->unInt; }
        case JBoo: { return (long)( psThis->ubBoo ); }
        case JFlt: { return (long)( psThis->unFlt ); }
        case JStr: { return atol(psThis->uxStr); }
        case JArr: { return (long)zListLength(psThis->upsArr); }
        case JObj: { return (long)zDictLength(psThis->upsObj); }
        default: { break; }
    }

    return 0;
}

sJson * psJFltSet(sJson * psThis, double nVal)
{
    switch ( eJsonType(psThis) )
    {
        case JFlt: { psThis->unFlt = nVal; break; }
        case JBoo: { psThis->ubBoo = (double)nVal; break; }
        case JInt: { psThis->unInt = (double)nVal; break; }
        case JStr: { psThis->uxStr[ snprintf(psThis->uxStr, zStrBufferSize(psThis->uxStr), "%lf", nVal) ] = '\0'; break; }
        default: { break; }
    }

    return psThis;
}
double nJFltGet(const sJson * psThis)
{
    switch ( eJsonType(psThis) )
    {
        case JFlt: { return psThis->unFlt; }
        case JBoo: { return (double)( psThis->ubBoo ); }
        case JInt: { return (double)( psThis->unInt ); }
        case JStr: { return atof(psThis->uxStr); }
        case JArr: { return (double)zListLength(psThis->upsArr); }
        case JObj: { return (double)zDictLength(psThis->upsObj); }
        default: { break; }
    }

    return 0.0f;
}

sJson * psJStrSet(sJson * psThis, const char * pnVal)
{
    switch ( eJsonType(psThis) )
    {
        case JStr: { psThis->uxStr = xStrCpy(psThis->uxStr, pnVal); }
        default: { break; }
    }

    return psThis;
}
const char * nJStrGet(const sJson * psThis)
{
    switch ( eJsonType(psThis) )
    {
        case JStr: { return psThis->uxStr; }
        case JBoo: { return ( psThis->ubBoo ) ? ( "true" ) : ( "false" ); }
        default: { break; }
    }

    return "";
}

size_t zJArrLength(const sJson * psThis)
{
    return ( JArr == eJsonType(psThis) ) ? ( zListLength(psThis->upsArr) ) : ( 0 ) ;
}
sJson * psJArrAccess(sJson * psThis, size_t zIdx)
{
    return ( JArr == eJsonType(psThis) ) ? ( pvListAccess(psThis->upsArr, zIdx) ) : ( NULL ) ;
}
sJson * psJArrInsert(sJson * psThis, size_t zIdx, sJson * psVal)
{
    return ( 
        ( JArr == eJsonType(psThis) ) && 
        ( NULL != psVal ) &&
        ( psThis->upsArr == psListInsert(psThis->upsArr, zIdx, psVal) )
    ) ? ( psThis ) : ( NULL ) ;
}
sJson * psJArrRemove(sJson * psThis, size_t zIdx)
{
    return ( 
        ( JArr == eJsonType(psThis) ) && 
        ( psThis->upsArr == psListRemove(psThis->upsArr, zIdx) )
    ) ? ( psThis ) : ( NULL ) ;
}
sJson * psJArrChange(sJson * psThis, size_t zIdx, sJson * psVal)
{
    return ( 
        ( JArr == eJsonType(psThis) ) && 
        ( NULL != psVal ) &&
        ( psThis->upsArr == psListChange(psThis->upsArr, zIdx, psVal) )
    ) ? ( psThis ) : ( NULL ) ;
}
sJson * psJArrInvert(sJson * psThis)
{
    return ( 
        ( JArr == eJsonType(psThis) ) && 
        ( psThis->upsArr == psListInvert(psThis->upsArr) )
    ) ? ( psThis ) : ( NULL ) ;
}
sJson * psJArrSortBy(sJson * psThis, pfCompare pfValCmp)
{
    return ( 
        ( JArr == eJsonType(psThis) ) && 
        ( psThis->upsArr == psListSortBy(psThis->upsArr, pfValCmp) )
    ) ? ( psThis ) : ( NULL ) ;
}

size_t zJObjLength(const sJson * psThis)
{
    return ( JObj == eJsonType(psThis) ) ? ( zDictLength(psThis->upsObj) ) : ( 0 ) ;
}
size_t zJObjHeight(const sJson * psThis)
{
    return ( JObj == eJsonType(psThis) ) ? ( zDictHeight(psThis->upsObj) ) : ( 0 ) ;
}
sJson * psJObjAccess(sJson * psThis, const char * pnKey)
{
    return ( JObj == eJsonType(psThis) ) ? ( pvDictAccess(psThis->upsObj, pnKey) ) : ( NULL ) ;
}
sJson * psJObjInsert(sJson * psThis, const char * pnKey, sJson * psVal)
{
    return ( 
        ( JObj == eJsonType(psThis) ) && 
        ( NULL != psVal ) &&
        ( psThis->upsObj == psDictInsert(psThis->upsObj, pnKey, psVal) )
    ) ? ( psThis ) : ( NULL ) ;
}
sJson * psJObjRemove(sJson * psThis, const char * pnKey)
{
    return ( 
        ( JObj == eJsonType(psThis) ) && 
        ( psThis->upsObj == psDictRemove(psThis->upsObj, pnKey) )
    ) ? ( psThis ) : ( NULL ) ;
}
sJson * psJObjChange(sJson * psThis, const char * pnKey, sJson * psVal)
{
    return ( 
        ( JObj == eJsonType(psThis) ) && 
        ( NULL != psVal ) &&
        ( psThis->upsObj == psDictChange(psThis->upsObj, pnKey, psVal) )
    ) ? ( psThis ) : ( NULL ) ;
}
