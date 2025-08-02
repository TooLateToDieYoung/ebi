#ifndef __EBI_H
#error "[ERROR] Make sure to explicitly include <ebi.h> first"
#else
#ifndef __EBI_JSON_H
#define __EBI_JSON_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum { JNul, JBoo, JInt, JFlt, JStr, JArr, JObj } eJson;

typedef struct sJson sJson;

int nJsonStringify(const sJson * psThis, char * pnDst);
sJson * psJsonParse(const char * pnSrc);
sJson * psJsonClone(const sJson * psSrc);
sJson * psJBooMake(bool bVal);
sJson * psJIntMake(long nVal);
sJson * psJFltMake(double nVal);
sJson * psJStrMake(const char * pnVal);
sJson * psJArrMake(void);
sJson * psJObjMake(void);
void vJsonFree(sJson * psThis);
eJson eJsonType(const sJson * psThis);

sJson * psJBooSet(sJson * psThis, bool bVal);
bool bJBooGet(const sJson * psThis);

sJson * psJIntSet(sJson * psThis, long nVal);
long nJIntGet(const sJson * psThis);

sJson * psJFltSet(sJson * psThis, double nVal);
double nJFltGet(const sJson * psThis);

sJson * psJStrSet(sJson * psThis, const char * pnVal);
const char * nJStrGet(const sJson * psThis);

size_t zJArrLength(const sJson * psThis);
sJson * psJArrAccess(sJson * psThis, size_t zIdx);
sJson * psJArrInsert(sJson * psThis, size_t zIdx, sJson * psVal);
sJson * psJArrRemove(sJson * psThis, size_t zIdx);
sJson * psJArrChange(sJson * psThis, size_t zIdx, sJson * psVal);
sJson * psJArrInvert(sJson * psThis);
sJson * psJArrSortBy(sJson * psThis, pfCompare pfValCmp);

size_t zJObjLength(const sJson * psThis);
size_t zJObjHeight(const sJson * psThis);
sJson * psJObjAccess(sJson * psThis, const char * pnKey);
sJson * psJObjInsert(sJson * psThis, const char * pnKey, sJson * psVal);
sJson * psJObjRemove(sJson * psThis, const char * pnKey);
sJson * psJObjChange(sJson * psThis, const char * pnKey, sJson * psVal);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __EBI_JSON_H */
#endif
