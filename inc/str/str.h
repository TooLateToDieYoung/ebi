#ifndef __EBI_H
#error "[ERROR] Make sure to explicitly include <ebi.h> first"
#else
#ifndef __EBI_STR_H
#define __EBI_STR_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef char * xStr;

xStr xStrAlloc(size_t zLen);
xStr xStrMake(const char * pnSrc);
void vStrFree(xStr xThis);
size_t zStrBufferSize(xStr xThis);

/* allow to be used in C style string */
#define zStrLen strlen

xStr xStrCpy(xStr xThis, const char * pnSrc);  /* return maybe new alloc pointer != xThis */
xStr xStrCpyN(xStr xThis, const char * pnSrc); /* return alwasy == xThis */

xStr xStrCat(xStr xThis, const char * pnSrc);  /* return maybe new alloc pointer != xThis */
xStr xStrCatN(xStr xThis, const char * pnSrc); /* return alwasy == xThis */

/* allow to be used in C style string */
#define nStrCmp strcmp 

/* allow to be used in C style string */
#define xStrChr strchr

/* allow to be used in C style string */
#define xStrStr strstr

#if 0
void vTestFunction(void)
{
    const char *const pnStr = ", ";
    xStr xSrc = xStrMake("gin, whisky, rum, vodka, tequila, brandy, wine, sake");
    xStr xCat = xStrAlloc(0);
    xStr xToken;
    xStr xStore;

    xStore = xSrc;
    do {
        xToken = xStrTok(xStore, pnStr, &xStore);
        printf("+ [%p, %s]\r\n", xToken, xToken);

        xCat = xStrCat(xCat, " > ");
        xCat = xStrCat(xCat, xToken);
        printf("? [%p] %s\r\n", xCat, xCat);
    } while ( xStore );

    vStrFree(xSrc);
    vStrFree(xCat);
}
#endif
xStr xStrTok(xStr xStore, const char * pnStr, xStr * pxStore);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EBI_STR_H */
#endif
