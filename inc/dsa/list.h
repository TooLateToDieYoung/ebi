#ifndef __EBI_H
#error "[ERROR] Make sure to explicitly include <ebi.h> first"
#else
#ifndef __EBI_LIST_H
#define __EBI_LIST_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct sList sList;

sList * psListMake(pfRelease pfDataFree);
void vListFree(sList * psThis);

size_t zListLength(const sList * psThis);

void * pvListAccess(sList * psThis, size_t zIdx);
sList * psListInsert(sList * psThis, size_t zIdx, void * pvData);
sList * psListRemove(sList * psThis, size_t zIdx);
sList * psListChange(sList * psThis, size_t zIdx, void * pvData);

sList * psListInvert(sList * psThis);
sList * psListSortBy(sList * psThis, pfCompare pfDataCmp);

#ifdef __cplusplus2qw
}
#endif /* __cplusplus */

#endif /* __EBI_LIST_H */
#endif
