
#ifndef IT_LOS_MEM_H
#define IT_LOS_MEM_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

#include "osTest.h"
#include "los_memory.h"
#include "los_config.h"
#include "iCunit.h"
#include "math.h"


#define TEST_MEM_SIZE 0x2000

#define IS_ALIGNED_SIZE(value, alignSize) (0 == ((UINT32)(value) & (UINT32)((alignSize) - 1)))

#define TEST_POOL_SIZE (1 * 8 * 1024)


#define OS_MEM_WATERLINE 1

#define LOS_DLNK_NODE_HEAD_SIZE 0

#define MIN_DLNK_POOL_SIZE      0
/* Supposing a Second Level Index: SLI = 3. */
#define OS_MEM_SLI 3
/* Giving 1 free list for each small bucket: 4, 8, 12, up to 124. */
#define OS_MEM_SMALL_BUCKET_COUNT 31
#define OS_MEM_SMALL_BUCKET_MAX_SIZE 128
/* Giving OS_MEM_FREE_LIST_NUM free lists for each large bucket. */
#define OS_MEM_LARGE_BUCKET_COUNT 24
#define OS_MEM_FREE_LIST_NUM (1 << OS_MEM_SLI)
/* OS_MEM_SMALL_BUCKET_MAX_SIZE to the power of 2 is 7. */
#define OS_MEM_LARGE_START_BUCKET 7

/* The count of free list. */
#define OS_MEM_FREE_LIST_COUNT (OS_MEM_SMALL_BUCKET_COUNT + (OS_MEM_LARGE_BUCKET_COUNT << OS_MEM_SLI))
/* The bitmap is used to indicate whether the free list is empty, 1: not empty, 0: empty. */
#define OS_MEM_BITMAP_WORDS ((OS_MEM_FREE_LIST_COUNT >> 5) + 1)

struct TestMemNodeHead {
#if (LOSCFG_BASE_MEM_NODE_INTEGRITY_CHECK == 1)
    UINT32 magic;
#endif
#if (LOSCFG_MEM_LEAKCHECK == 1)
    UINTPTR linkReg[LOS_RECORD_LR_CNT];
#endif
    union {
        struct TestMemNodeHead *prev; /* The prev is used for current node points to the previous node */
        struct TestMemNodeHead *next; /* The next is used for sentinel node points to the expand node */
    } ptr;
#if (LOSCFG_MEM_FREE_BY_TASKID == 1)
    UINT32 taskID : 6;
    UINT32 sizeAndFlag : 26;
#else
    UINT32 sizeAndFlag;
#endif
};

struct TestMemUsedNodeHead {
    struct TestMemNodeHead header;
};

struct TestMemFreeNodeHead {
    struct TestMemNodeHead header;
    struct TestMemFreeNodeHead *prev;
    struct TestMemFreeNodeHead *next;
};

struct TestMemPoolInfo {
    VOID *pool;
    UINT32 totalSize;
    UINT32 attr;
#if (LOSCFG_MEM_WATERLINE == 1)
    UINT32 waterLine;   /* Maximum usage size in a memory pool */
    UINT32 curUsedSize; /* Current usage size in a memory pool */
#endif
};

struct TestMemPoolHead {
    struct TestMemPoolInfo info;
    UINT32 freeListBitmap[OS_MEM_BITMAP_WORDS];
    struct TestMemFreeNodeHead *freeList[OS_MEM_FREE_LIST_COUNT];
#if (LOSCFG_MEM_MUL_POOL == 1)
    VOID *nextPool;
#endif
};

#define LOS_MEM_NODE_HEAD_SIZE sizeof(struct TestMemUsedNodeHead)
#define MIN_MEM_POOL_SIZE (LOS_MEM_NODE_HEAD_SIZE + sizeof(struct TestMemPoolHead))
#define LOS_MEM_POOL_SIZE sizeof(struct TestMemPoolHead)

void *g_memPool1;
void *g_testPool1;

void MemBasicAlloc(void);
void MemBasicFree(void);
void MemTestInit(void);
void MemTestDeinit(void);

VOID Basic_Test(void);
VOID BasicAlloc_Free(void);
VOID Zero_Max_Alloc_Free(void);
VOID oneByteAlloc(void);
VOID InitMemWithIlligalParam(void);
VOID poolInitMem(void);
VOID ContinueAlloc(void);
VOID ReallocMem(VOID);
VOID Mem_Demo(void);
#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* IT_LOS_MEM_H */