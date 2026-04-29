#include "los_queue.h"
#include "los_task.h"
#include "cmsis_os2.h"
#include <stdio.h>

#include "shell.h"
#include "shcmd.h"

#define BUFFER_LEN 50

STATIC UINT32 myqueue;

VOID SendEntry(VOID)
{
    UINT32 ret = 0;
    CHAR msg[] = "testmsg";
    UINT32 len = sizeof(msg);

    ret = LOS_QueueWriteCopy(myqueue, msg, len, 0);
}

VOID getInfo(){
    QUEUE_INFO_S queueInfo;
    LOS_QueueInfoGet(myqueue,&queueInfo);
    printf("Queue ID:      %u\n", queueInfo.queueID);
    printf("Queue Length:  %u (max messages)\n", queueInfo.queueLen);
    printf("Queue Size:    %u bytes/node\n", queueInfo.queueSize);
    printf("Queue Head:    %u\n", queueInfo.queueHead);
    printf("Queue Tail:    %u\n", queueInfo.queueTail);
    printf("Readable Cnt:  %u (messages available)\n", queueInfo.readableCnt);
    printf("Writable Cnt:  %u (free slots)\n", queueInfo.writableCnt);    
}

VOID RecvEntry(VOID)
{
    UINT32 ret = 0;
    CHAR readBuf[BUFFER_LEN] = {0};
    UINT32 readLen = BUFFER_LEN;
    usleep(1000000);
    ret = LOS_QueueReadCopy(myqueue, readBuf, &readLen, 0);
    printf("recv message: %s.\n", readBuf);
    ret = LOS_QueueDelete(myqueue);
    printf("delete the queue success.\n");
    printf("getinfo:\n");
    getInfo();
}

UINT32 MyQueue(){
    printf("start queue example.\n");
    UINT32 ret = 0;
    UINT32 task;
    TSK_INIT_PARAM_S taskParam = { 0 };
    UINT32 task2;
    TSK_INIT_PARAM_S taskParam2 = { 0 };
    LOS_TaskLock();

    taskParam.pfnTaskEntry = (TSK_ENTRY_FUNC)SendEntry;
    taskParam.usTaskPrio = 5;
    taskParam.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    taskParam.pcName = "sendq";
    ret = LOS_TaskCreate(&task, &taskParam);

    taskParam2.pfnTaskEntry = (TSK_ENTRY_FUNC)RecvEntry;
    taskParam2.usTaskPrio = 6;
    taskParam2.uwStackSize = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    taskParam2.pcName = "receiveq";
    ret = LOS_TaskCreate(&task2, &taskParam2);

    ret = LOS_QueueCreate("queuereceive", 5, &myqueue, 0, 50);
    printf("create the queue success.\n");
    LOS_TaskUnlock();
    getInfo();
    return ret;
}

int registerMyQueue(){
    OsShellInit();
    osCmdReg(CMD_TYPE_EX,"queuetest",0,(CmdCallBackFunc)MyQueue);
}