#include "los_memory.h"

#include "los_task.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "los_event.h"

#include "shell.h"
#include "shcmd.h"


#define TEST_POOL_SIZE (2*1024)
#define MEMBOX_WR_TEST_NUM  828

__attribute__((aligned(4))) UINT8 g_testDynPool[TEST_POOL_SIZE];

VOID ExampleDynMem(VOID)
{
    UINT32 *mem = NULL;
    UINT32 ret;

    /* 初始化内存池 */
    ret = LOS_MemInit(g_testDynPool, TEST_POOL_SIZE);
    if (LOS_OK  == ret) {
        printf("Mem init success!\n");
    } else {
        printf("Mem init failed!\n");
        return;
    }

    /* 申请内存块 */
    mem = (UINT32 *)LOS_MemAlloc(g_testDynPool, 4);
    if (mem == NULL) {
        printf("Mem alloc failed!\n");
        return;
    }
    printf("Mem alloc success!\n");

    /* 内存地址读写验证 */
    *mem = MEMBOX_WR_TEST_NUM;
    printf("*mem = %d\n", *mem);

    /* 释放内存 */
    ret = LOS_MemFree(g_testDynPool, mem);
    if (LOS_OK == ret) {
        printf("Mem free success!\n");
    } else {
        printf("Mem free failed!\n");
    }

    return;
}
