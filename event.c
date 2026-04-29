#include "los_task.h"
#include "cmsis_os2.h"
#include <stdio.h>
#include "los_event.h"

#include "shell.h"
#include "shcmd.h"

/* 事件控制结构体 */
EVENT_CB_S g_exampleEvent;

/* 等待的事件类型 */
#define EVENT_WAIT 0x00000001

/* 等待超时时间 */
#define EVENT_TIMEOUT 100

/* 用例任务入口函数 */
VOID EventReadTask(VOID)
{
    UINT32 ret;
    UINT32 event;

    /* 超时等待方式读事件,超时时间为100 ticks, 若100 ticks后未读取到指定事件，读事件超时，任务直接唤醒 */
    printf("Example_Event wait event 0x%x \n", EVENT_WAIT);

    event = LOS_EventRead(&g_exampleEvent, EVENT_WAIT, LOS_WAITMODE_AND, EVENT_TIMEOUT);
    if (event == EVENT_WAIT) {
        printf("Example_Event, read event :0x%x\n", event);
        /* 清标志位 */  
        printf("begin LOS_EventClear..\n");  
        printf("EventMask:%d\n", g_exampleEvent.uwEventID);
        LOS_EventClear(&g_exampleEvent, ~g_exampleEvent.uwEventID);
        printf("EventMask:%d\n", g_exampleEvent.uwEventID);
        printf("LOS_EventClear successfully\n");
        /* 删除事件 */
        ret = LOS_EventDestroy(&g_exampleEvent);
        if (ret != LOS_OK) {
            printf("destory event failed .\n");
            return LOS_NOK;
        }
        printf("destory event success .\n");
    } else {
        printf("Example_Event, read event timeout\n");
    }
}

UINT32 ExampleEvent(VOID)
{
    UINT32 ret;
    UINT32 taskId;
    TSK_INIT_PARAM_S taskParam;

    /* 事件初始化 */
    ret = LOS_EventInit(&g_exampleEvent);
    if (ret != LOS_OK) {
        printf("init event failed .\n");
        return LOS_NOK;
    }

    /* 创建任务 */
    memset_s(&taskParam,sizeof(TSK_INIT_PARAM_S),0,sizeof(TSK_INIT_PARAM_S));
    taskParam.pfnTaskEntry = (TSK_ENTRY_FUNC)EventReadTask;
    taskParam.pcName       = "EventReadTask";
    taskParam.uwStackSize  = LOSCFG_BASE_CORE_TSK_DEFAULT_STACK_SIZE;
    taskParam.usTaskPrio   = 3;
    ret = LOS_TaskCreate(&taskId, &taskParam);
    if (ret != LOS_OK) {
        printf("task create failed.\n");
        return LOS_NOK;
    }

    /* 写事件 */
    printf("Example_TaskEntry write event.\n");

    ret = LOS_EventWrite(&g_exampleEvent, EVENT_WAIT);
    if (ret != LOS_OK) {
        printf("event write failed.\n");
        return LOS_NOK;
    }
    return LOS_OK;
}

int registerMyEvent(){
    OsShellInit();
    osCmdReg(CMD_TYPE_EX,"event",0,(CmdCallBackFunc)ExampleEvent);
}