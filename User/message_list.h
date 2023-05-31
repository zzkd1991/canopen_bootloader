#ifndef _MESSAGE_LIST_H
#define _MESSAGE_LIST_H

#include "los_typedef.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

typedef struct LOS_DL_LIST
{
	struct LOS_DL_LIST *pstPrev;
	struct LOS_DL_LIST *pstNext;
}LOS_DL_LIST;

static void LOS_ListInit(LOS_DL_LIST *pstList)
{
	pstList->pstNext = pstList;
	pstList->pstPrev = pstList;	
}

#define LOS_DL_LIST_FIRST(pstObject)	((pstObject)->pstNext)

static void LOS_ListAdd(LOS_DL_LIST *pstList, LOS_DL_LIST *pstNode)
{
	pstNode->pstNext = pstList->pstNext;
	pstNode->pstPrev = pstList;
	pstList->pstNext->pstPrev = pstNode;
	pstList->pstNext = pstNode;
}

static void LOS_ListTailInsert(LOS_DL_LIST *pstList, LOS_DL_LIST *pstNode)
{
	LOS_ListAdd(pstList->pstPrev, pstNode);
}

static void LOS_ListDelete(LOS_DL_LIST *pstNode)
{
	pstNode->pstNext->pstPrev = pstNode->pstPrev;
	pstNode->pstPrev->pstNext = pstNode->pstNext;
	pstNode->pstNext = (LOS_DL_LIST *)NULL;
	pstNode->pstPrev = (LOS_DL_LIST *)NULL;
}

static bool LOS_ListEmpty(LOS_DL_LIST *pstNode)
{
	return (bool)(pstNode->pstNext == pstNode);
}

#define OFFSET_OF_FIELD(type, field)	((uint32_t)(((type *)0)->field))

#define LOS_OFF_SET_OF(type, member)	((long)&((type *)0)->member)

#define LOS_DL_LIST_ENTRY(item, type, member) \
	((type *)((char *)item - LOS_OFF_SET_OF(type, member))) \

#define LOS_DL_LIST_FOR_EACH_ENTRY(item, list, type, member) \
	for(item = LOS_DL_LIST_ENTRY((list)->pstNext, type, member); \
		&item->member != (list); \
		item = LOS_DL_LIST_ENTRY(item->member.pstNext, type, member))


#define LOS_DL_LIST_FOR_EACH_ENTRY_SAFE(item, next, list, type, member)		\
	for(item = LOS_DL_LIST_ENTRY((list)->pstNext, type, member), \
		next = LOS_DL_LIST_ENTRY(item->member.pstNext, type, member); \
		&item->member != (list); \
		item = next, next = LOS_DL_LIST_ENTRY(item->member.pstNext, type, member))

static void osListDel(LOS_DL_LIST *pstPrevNode, LOS_DL_LIST *pstNextNode)
{
	pstNextNode->pstPrev = pstPrevNode;
	pstPrevNode->pstNext = pstNextNode;
}

static void LOS_ListDelInit(LOS_DL_LIST *pstList)
{
	osListDel(pstList->pstPrev, pstList->pstNext);
	LOS_ListInit(pstList);
}

#define LOS_DL_LIST_FOR_EACH(item, list)	\
	for((item) = (list)->pstNext; \
		(item) != (list);	\
		(item) = (item)->pstNext)

#define LOS_DL_LIST_FOR_EACH_SAFE(item, next, list) \
	for(item = (list)->pstNext, next = item->pstNext; item != (list); \
		item = next, next = item->pstNext)

#define LOS_DL_LIST_HEAD(list)	\
	LOS_DL_LIST list = {&(list), &(list)}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif


#endif
