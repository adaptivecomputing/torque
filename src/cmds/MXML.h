#include "license_pbs.h" /* See here for the software license */

#include "src/include/mcom.h" /* mxml_t, enum MDataFormatEnum, mbool_t */

int MXMLExtractE(mxml_t *E, mxml_t *C, mxml_t **CP); 

int MXMLSetChild(mxml_t *E, char *CName, mxml_t **CE); 

int MXMLCreateE(mxml_t **E, char *Name); 

int MXMLDestroyE(mxml_t **EP); 

int MXMLSetAttr(mxml_t *E, char *A, void *V, enum MDataFormatEnum Format); 

int MXMLAppendAttr(mxml_t *E, char *AName, char *AVal, char Delim); 

int MXMLSetVal(mxml_t *E, void *V, enum MDataFormatEnum Format); 

int MXMLAddE(mxml_t *E, mxml_t *C); 

int MXMLToXString(mxml_t *E, char **Buf, int *BufSize, int MaxBufSize, char **Tail, mbool_t IsRootElement); 

int MXMLToString(mxml_t *E, char *Buf, int BufSize, char **Tail, mbool_t IsRootElement); 

int MXMLGetAttrF(mxml_t *E, char *AName, int *ATok, void *AVal, enum MDataFormatEnum DFormat, int VSize); 

int MXMLGetAttr(mxml_t *E, char *AName, int *ATok, char *AVal, int VSize); 

int MXMLGetChild(mxml_t *E, char *CName, int *CTok, mxml_t **C); 

int MXMLGetChildCI(mxml_t *E, char *CName, int *CTok, mxml_t **CP); 

int MXMLFromString(mxml_t **EP, char *XMLString, char **Tail, char *EMsg); 

