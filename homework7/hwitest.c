#include "los_interrupt.h"
#include "los_compiler.h"
#include "los_task.h"
#include "cmsis_os2.h"
#include <stdio.h>

#include "shell.h"
#include "shcmd.h"

#define HWI_NUM_TEST 7

STATIC VOID UsrIrqEntry(VOID)
{
    printf("in the func UsrIrqEntry\n");
}

STATIC VOID InterruptTest(VOID)
{
    printf("Task trigger interrupt start.\n");
    LOS_HwiTrigger(HWI_NUM_TEST);
    printf("Task trigger interrupt done.\n");
}

UINT32 ExampleInterrupt(VOID)
{
    printf("2024382026 陈冠宇 hwitest start...\n");
    UINT32 ret;
    HWI_PRIOR_T hwiPrio = 3;
    HWI_MODE_T mode = 0;
    HWI_ARG_T arg = 0;

    /* 1. 创建中断 */
    ret = LOS_HwiCreate(HWI_NUM_TEST, hwiPrio, mode, (HWI_PROC_FUNC)UsrIrqEntry, arg);
    if (ret == LOS_OK) {
        printf("Hwi create success!\n");
    } else {
        printf("Hwi create failed!\n");
        return LOS_NOK;
    }

    /* 2. 创建任务来触发中断 */
    TSK_INIT_PARAM_S taskParam = { 0 };
    UINT32 testTaskID;

    taskParam.pfnTaskEntry = (TSK_ENTRY_FUNC)InterruptTest;
    taskParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    taskParam.pcName = "InterruptTest";
    taskParam.usTaskPrio = 2;
    taskParam.uwResved = LOS_TASK_ATTR_JOINABLE;
    ret = LOS_TaskCreate(&testTaskID, &taskParam);
    if (LOS_OK != ret) {
        printf("InterruptTest task error\n");
    }

    /* 等待任务执行完毕 */
    LOS_TaskDelay(50);

    /* 3. 删除中断 */
    ret = LOS_HwiDelete(HWI_NUM_TEST, NULL);
    if (ret == LOS_OK) {
        printf("Hwi delete success!\n");
    } else {
        printf("Hwi delete failed!\n");
        return LOS_NOK;
    }

    printf("hwitest finished.\n");
    return LOS_OK;
}

int registerMyHwi(void)
{
    OsShellInit();
    osCmdReg(CMD_TYPE_EX, "hwitest", 0, (CmdCallBackFunc)ExampleInterrupt);
}
