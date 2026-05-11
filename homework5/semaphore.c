#include "los_sem.h"
#include "los_task.h"
#include "cmsis_os2.h"
#include <stdio.h>

#include "shell.h"
#include "shcmd.h"

static UINT32 semid;

void SemaphoreTask1(void){
    UINT32 ret;
    printf("SemaphoreTask1 try to get semaphore\n");
    ret = LOS_SemPend(semid, 10);
    if (ret == LOS_OK) {
         LOS_SemPost(semid);
         return;
    }
    if (ret == LOS_ERRNO_SEM_TIMEOUT) {
        printf("SemaphoreTask1 timeout\n");
        ret = LOS_SemPend(semid, LOS_WAIT_FOREVER);
        printf("SemaphoreTask1 wait_forever to pend\n");
        if (ret == LOS_OK) {
            printf("SemaphoreTask1 get sem success\n");
            LOS_SemPost(semid);
            return;
        }
    }
}

void SemaphoreTask2(void){
    UINT32 ret;
    printf("SemaphoreTask2 try to get semaphore\n");
    ret = LOS_SemPend(semid, LOS_WAIT_FOREVER);
    if (ret == LOS_OK) {
        printf("SemaphoreTask2 get semaphore success\n");
    }
    LOS_TaskDelay(20);
    printf("SemaphoreTask2 post semaphore\n");
    LOS_SemPost(semid);
    return;
}

void SemaphoreTask3(void){
    UINT32 ret;
    printf("SemaphoreTask3 try to get semaphore\n");
    ret = LOS_SemPend(semid, 10);
    if (ret == LOS_ERRNO_SEM_TIMEOUT) {
        printf("SemaphoreTask3 timeout\n");
        ret = LOS_SemPend(semid, LOS_WAIT_FOREVER);
        printf("SemaphoreTask3 wait_forever to pend\n");
        if (ret == LOS_OK) {
            printf("SemaphoreTask3 get sem success\n");
            LOS_TaskDelay(2000);
            LOS_SemPost(semid);
            return;
        }
    }
}

INT32 Semaphore(){
    printf("start Semaphore\n");
    UINT32 ret = 0;
    UINT32 task;
    TSK_INIT_PARAM_S taskParam = { 0 };
    UINT32 task2;
    TSK_INIT_PARAM_S taskParam2 = { 0 };
    UINT32 task3;
    TSK_INIT_PARAM_S taskParam3 = { 0 };
    LOS_TaskLock();

    LOS_SemCreate(1, &semid);
    taskParam.pfnTaskEntry = (TSK_ENTRY_FUNC)SemaphoreTask1;
    taskParam.usTaskPrio = 6;
    taskParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    taskParam.pcName = "SemaphoreTask1";
    ret = LOS_TaskCreate(&task, &taskParam);

    taskParam2.pfnTaskEntry = (TSK_ENTRY_FUNC)SemaphoreTask2;
    taskParam2.usTaskPrio = 5;
    taskParam2.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    taskParam2.pcName = "SemaphoreTask2";
    ret = LOS_TaskCreate(&task2, &taskParam2);

    taskParam3.pfnTaskEntry = (TSK_ENTRY_FUNC)SemaphoreTask3;
    taskParam3.usTaskPrio = 5;
    taskParam3.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    taskParam3.pcName = "SemaphoreTask3";
    ret = LOS_TaskCreate(&task3, &taskParam3);

    // LOS_SemPost(semid);
    LOS_TaskUnlock();
    printf("waiting 400 ticks and delete the sem\n");
    LOS_TaskDelay(400);
    ret = LOS_SemDelete(semid);
    if(ret!=LOS_OK){
        printf("delete failed\n");
        LOS_TaskDelay(2000);
        ret = LOS_SemDelete(semid);
        if(ret!=LOS_OK){
            printf("delete failed2\n");
            return ret;
        }
    }
    printf("delete success\n");    
    return ret;
}

int registerMySem(){
    OsShellInit();
    osCmdReg(CMD_TYPE_EX,"semtest",0,(CmdCallBackFunc)Semaphore);
}