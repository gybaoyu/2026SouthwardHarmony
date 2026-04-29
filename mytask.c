#include "los_task.h"
#include "cmsis_os2.h"
#include "shell.h"
#include "shcmd.h"

UINT32 g_taskLoID;
UINT32 g_taskHiID;
#define TSK_PRIOR_HI 4
#define TSK_PRIOR_LO 5
UINT32 ExampleTaskHi(VOID)
{
    UINT32 ret;
    printf("Enter TaskHi Handler.\n");
    /* 延时2个Tick，延时后该任务会挂起，执行剩余任务中最高优先级的任务(g_taskLoID任务) */
    ret = LOS_TaskDelay(100);
    if (ret != LOS_OK) {
        printf("Delay Task Failed.\n");
        return LOS_NOK;
    }
    /* 2个Tick时间到了后，该任务恢复，继续执行 */
    printf("TaskHi LOS_TaskDelay Done.\n");
    /* 挂起自身任务 */
    ret = LOS_TaskSuspend(g_taskHiID);
    if (ret != LOS_OK) {
        printf("Suspend TaskHi Failed.\n");
        return LOS_NOK;
    }
    printf("TaskHi LOS_TaskResume Success.\n");
    return LOS_OK;
}

/* 低优先级任务入口函数 */
UINT32 ExampleTaskLo(VOID)
{
    UINT32 ret;
    printf("Enter TaskLo Handler.\n");
    /* 延时2个Tick，延时后该任务会挂起，执行剩余任务中就高优先级的任务(背景任务) */
    ret = LOS_TaskDelay(100);
    if (ret != LOS_OK) {
        printf("Delay TaskLo Failed.\n");
        return LOS_NOK;
    }
    printf("TaskHi LOS_TaskSuspend Success.\n");
    /* 恢复被挂起的任务g_taskHiID */
    ret = LOS_TaskResume(g_taskHiID);
    if (ret != LOS_OK) {
        printf("Resume TaskHi Failed.\n");
        return LOS_NOK;
    }
    printf("TaskHi LOS_TaskDelete Success.\n");
    return LOS_OK;
}
/* 任务测试入口函数，在里面创建优先级不一样的两个任务 */
UINT32 ExampleTaskCaseEntry(VOID)
{
    UINT32 ret;
    TSK_INIT_PARAM_S initParam = {0};

    /* 锁任务调度 */
    LOS_TaskLock();
    printf("LOS_TaskLock() Success!\n");
    /* 高优先级任务的初始化参数，其资源回收需要其他任务调用 LOS_TaskJoin */
    initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)ExampleTaskHi;
    initParam.usTaskPrio = 25;
    initParam.pcName = "h";
    initParam.uwStackSize = 4096;
    // initParam.uwResved   = LOS_TASK_ATTR_JOINABLE;

    /* 创建高优先级任务，由于锁任务调度，任务创建成功后不会马上执行 */
    ret = LOS_TaskCreate(&g_taskHiID, &initParam);
    if (ret != LOS_OK) {
        LOS_TaskUnlock();
        printf("ExampleTaskHi create Failed! ret=%d\n", ret);
        return LOS_NOK;
    }
    printf("ExampleTaskHi create Success!\n");

    /* 低优先级任务的初始化参数，任务结束后会自行结束销毁 */
    initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)ExampleTaskLo;
    initParam.usTaskPrio = 27;
    initParam.pcName = "l";
    initParam.uwStackSize = 4096;
    // initParam.uwResved   = LOS_TASK_STATUS_DETACHED;

    /* 创建低优先级任务，由于锁任务调度，任务创建成功后不会马上执行 */
    ret = LOS_TaskCreate(&g_taskLoID, &initParam);
    if (ret!= LOS_OK) {
        LOS_TaskUnlock();
        printf("ExampleTaskLo create Failed!\n");
        return LOS_NOK;
    }
    printf("ExampleTaskLo create Success!\n");

    /* 解锁任务调度，此时会发生任务调度，执行就绪列表中最高优先级任务 */
    LOS_TaskUnlock();
    // ret = LOS_TaskJoin(g_taskHiID, NULL);
    // if (ret != LOS_OK) {
    //     printf("Join ExampleTaskHi Failed!\n");
    //     printf("0x%08x\n",ret);
    // } else {
    //     printf("Join ExampleTaskHi Success!\n");
    // }
    // while(1){};
    return LOS_OK;
}

int registerMyTask(){
    OsShellInit();
    osCmdReg(CMD_TYPE_EX,"mytask",0,(CmdCallBackFunc)ExampleTaskCaseEntry);
}