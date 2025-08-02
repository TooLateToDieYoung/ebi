#ifndef __EBI_H
#error "[ERROR] Make sure to explicitly include <ebi.h> first"
#else
#ifndef __EBI_DICT_H
#define __EBI_DICT_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct sDict sDict;

sDict * psDictMake(pfRelease pfDataFree);
void vDictFree(sDict * psThis);

size_t zDictLength(const sDict * psThis);
size_t zDictHeight(const sDict * psThis);

void * pvDictAccess(sDict * psThis, const char * pnKey);
sDict * psDictInsert(sDict * psThis, const char * pnKey, void * pvData);
sDict * psDictRemove(sDict * psThis, const char * pnKey);
sDict * psDictChange(sDict * psThis, const char * pnKey, void * pvData);

#if 0
const char * pnKey = NULL;
for (
    pnKey = pnDictRootKey(psDict);
    pnKey;
    pnKey = pnDictNextKey(psDict, pnKey)
) {
    (void)pvDictAccess(psDict, pnKey);
}
#endif
const char * pnDictRootKey(sDict * psThis);
const char * pnDictNextKey(sDict * psThis, const char * pnKey);

#ifdef __cplusplus2qw
}
#endif /* __cplusplus */

#endif /* __EBI_DICT_H */
#endif
