#include "los_mux.h"
#include "los_task.h"
#include "cmsis_os2.h"
#include <stdio.h>

#include "shell.h"
#include "shcmd.h"
/* 互斥锁句柄 */
UINT32 g_testMux;

VOID ExampleMutexTask1(VOID)
{
    UINT32 ret;

    printf("task1 try to get mux\n");
    ret = LOS_MuxPend(g_testMux, 10);
    if (ret == LOS_OK) {
        printf("task1 get mux.\n");
        LOS_MuxPost(g_testMux);
        LOS_MuxDelete(g_testMux);
        return;
    }
    
    if (ret == LOS_ERRNO_MUX_TIMEOUT ) {
        printf("task1 timeout \n");VOID ExampleMutexTask1(VOID)
{
    UINT32 ret;

    printf("task1 try to get mux\n");
    ret = LOS_MuxPend(g_testMux, 10);
    if (ret == LOS_OK) {
        printf("task1 get mux.\n");
        LOS_MuxPost(g_testMux);
        LOS_MuxDelete(g_testMux);
        return;
    }
    
    if (ret == LOS_ERRNO_MUX_TIMEOUT ) {
        printf("task1 timeout \n");
        ret = LOS_MuxPend(g_testMux, LOS_WAIT_FOREVER);
        if (ret == LOS_OK) {
            printf("task1 get mux success.\n");
            LOS_MuxPost(g_testMux);
            LOS_MuxDelete(g_testMux);
            printf("task1 post and delete mux.\n");
            return;
        }
    }
    return;
}
        ret = LOS_MuxPend(g_testMux, LOS_WAIT_FOREVER);
        if (ret == LOS_OK) {
            printf("task1 get mux success.\n");
            LOS_MuxPost(g_testMux);
            LOS_MuxDelete(g_testMux);
            printf("task1 post and delete mux.\n");
            return;
        }
    }
    return;
}

VOID ExampleMutexTask2(VOID)
{
    printf("task2 try to get mux, wait forever.\n");
    (VOID)LOS_MuxPend(g_testMux, LOS_WAIT_FOREVER);
    printf("task2 get mux and suspend 100 ticks.\n");
    LOS_TaskDelay(100);
    printf("task2 resumed and post the mux\n");
    LOS_MuxPost(g_testMux);
    return;
}

UINT32 ExampleMutex(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S task1 = { 0 };
    TSK_INIT_PARAM_S task2 = { 0 };
    UINT32 taskId01;
    UINT32 taskId02;

    LOS_MuxCreate(&g_testMux);
    LOS_TaskLock();
    task1.pfnTaskEntry = (TSK_ENTRY_FUNC)ExampleMutexTask1;
    task1.pcName       = "MuxTask1";
    task1.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    task1.usTaskPrio   = 5;
    ret = LOS_TaskCreate(&taskId01, &task1);
    if (ret != LOS_OK) {
        printf("task1 create failed.\n");
        return LOS_NOK;
    }
    task2.pfnTaskEntry = (TSK_ENTRY_FUNC)ExampleMutexTask2;
    task2.pcName       = "MuxTask2";
    task2.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    task2.usTaskPrio   = 4;
    ret = LOS_TaskCreate(&taskId02, &task2);
    if (ret != LOS_OK) {
        printf("task2 create failed.\n");
        return LOS_NOK;
    }

    LOS_TaskUnlock();

    return LOS_OK;
}

int registerMyMux(){
    OsShellInit();
    osCmdReg(CMD_TYPE_EX,"muxtest",0,(CmdCallBackFunc)ExampleMutex);
}