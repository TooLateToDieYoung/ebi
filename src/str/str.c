#include <ebi.h>
#include <str/str.h>

#include <stdio.h>

static const size_t _zReserved = sizeof(unsigned char) + sizeof(char); /* uShift + '\0' */

typedef struct {
    unsigned char nOffset;
    char nBuffer[];
} sStr;

xStr xStrAlloc(size_t zLen)
{
    sStr * psThis;
    unsigned char nOffset;

    zLen += _zReserved;
    for ( nOffset = 1; zLen > ( 1U << nOffset ); ++nOffset ) { }

    psThis = (sStr *)malloc(( 1U << nOffset ));
    if ( !psThis ) { return NULL; }

    psThis->nOffset = nOffset;
    psThis->nBuffer[0] = '\0';
    return psThis->nBuffer;
}
xStr xStrMake(const char * pnSrc)
{
    return ( pnSrc ) ? xStrCpyN(xStrAlloc(strlen(pnSrc)), pnSrc) : ( NULL ) ;
}
void vStrFree(xStr xThis)
{
    free(&xThis[-1]);
}
size_t zStrBufferSize(xStr xThis)
{
    return ( (size_t)( 1U << xThis[-1] ) - _zReserved ); 
}

xStr xStrCpy(xStr xThis, const char * pnSrc)
{
    xStr xThat;
    size_t zBufferSize;
    size_t zLength;

    if ( !xThis || !pnSrc ) { return xThis; }

    zBufferSize = zStrBufferSize(xThis);
    zLength = strlen(pnSrc);

    if ( zLength > zBufferSize )
    {
        xThat = xStrAlloc(zLength);
        if ( !xThat ) { return xThis; }
        vStrFree(xThis);
        xThis = NULL;
    }
    else
    {
        xThat = xThis;
    }

    xThat[ snprintf(xThat, 1 + zLength, "%s", pnSrc) ] = '\0';
    return xThat;
}
xStr xStrCpyN(xStr xThis, const char * pnSrc)
{
    size_t zBufferSize;
    size_t zLength;

    if ( !xThis || !pnSrc ) { return xThis; }

    zBufferSize = zStrBufferSize(xThis);
    zLength = strlen(pnSrc);
    if ( zLength > zBufferSize )
    {
        zLength = zBufferSize;
    }

    xThis[ snprintf(xThis, 1 + zLength, "%s", pnSrc) ] = '\0';
    return xThis;
}

xStr xStrCat(xStr xThis, const char * pnSrc)
{
    xStr xThat;
    size_t zBufferSize;
    size_t zDstLen;
    size_t zSrcLen;

    if ( !xThis || !pnSrc ) { return xThis; }
    
    zBufferSize = zStrBufferSize(xThis);
    zDstLen = zStrLen(xThis);
    zSrcLen = strlen(pnSrc);
    if ( zDstLen + zSrcLen > zBufferSize )
    {
        xThat = xStrAlloc(zDstLen + zSrcLen);
        if ( !xThat ) { return xThis; }
        strcpy(xThat, xThis);
        vStrFree(xThis);
        xThis = NULL;
    }
    else
    {
        xThat = xThis;
    }

    xThat[ zDstLen + snprintf(&xThat[zDstLen], 1 + zSrcLen, "%s", pnSrc) ] = '\0';
    return xThat;
}
xStr xStrCatN(xStr xThis, const char * pnSrc)
{
    size_t zBufferSize;
    size_t zDstLen;
    size_t zSrcLen;
    size_t zAddLen;

    if ( !xThis || !pnSrc ) { return xThis; }

    zBufferSize = zStrBufferSize(xThis);
    zDstLen = zStrLen(xThis);
    zSrcLen = strlen(pnSrc);
    zAddLen = ( zDstLen + zSrcLen > zBufferSize ) ? ( zBufferSize - zDstLen ) : ( zSrcLen ) ;

    xThis[ zDstLen + snprintf(&xThis[zDstLen], 1 + zAddLen, "%s", pnSrc) ] = '\0';
    return xThis;
}

xStr xStrTok(xStr xStore, const char * pnStr, xStr * pxStore)
{
    xStr xRet = xStore;

    *pxStore = xStrStr(xStore, pnStr);
    if ( *pxStore )
    {
        ( *pxStore )[0] = '\0';
        ( *pxStore ) += strlen(pnStr);
    }

    return xRet;
}
