/*
 * 分布式软总线实验手册案例 —— 数据传输完整示例
 *
 * 功能演示:
 *   1. 注册设备状态回调，监听设备上下线
 *   2. 创建会话服务端 (CreateSessionServer)
 *   3. 打开会话 (OpenSession)
 *   4. 发送字节数据 (SendBytes)
 *   5. 发送消息数据 (SendMessage)
 *   6. 关闭会话 (CloseSession)
 *   7. 移除会话服务端 (RemoveSessionServer)
 *   8. 注销设备状态回调 (UnregNodeDeviceStateCb)
 *
 * 测试命令: dsoftbus
 * 一条命令执行全部9步测试流程
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "securec.h"
#include "session.h"
#include "softbus_bus_center.h"
#include "softbus_errcode.h"
#include "softbus_log.h"

#include "shell.h"
#include "shcmd.h"

/* ========== 宏定义 ========== */
#define TYPE_SEND_BYTE      15
#define TYPE_SEND_MESSAGE   16
#define SLEEP_TIME          15
#define TRANS_UNIT_SIZE     1024
#define TRANS_SIZE_NUM      2
#define TRANS_SIZE_NUM_DOUBLE 4
#define LOOP_COUNT          10
#define NETWORK_ID_SIZE     100

/* ========== 全局变量 ========== */
static char const *g_pkgName = "com.huawei.communication.demo";
static char g_networkId[NETWORK_ID_SIZE];
static int g_sessionId = 0;
char const *g_sessionName = "com.huawei.ctrlbustest.JtCreateSessionServerLimit";
char const *g_groupid = "TEST_GROUP_ID";

/* ========== 会话回调接口实现 ========== */

static int OnSessionOpened(int sessionId, int result)
{
    printf("[SOFTBUS] OnSessionOpened: sessionId=%d, result=%d\n", sessionId, result);
    if (result == SOFTBUS_OK) {
        g_sessionId = sessionId;
        printf("[SOFTBUS] Session opened successfully! sessionId=%d\n", sessionId);
    } else {
        printf("[SOFTBUS] Session open failed! result=%d\n", result);
    }
    return result;
}

static void OnSessionClosed(int sessionId)
{
    printf("[SOFTBUS] OnSessionClosed: sessionId=%d\n", sessionId);
    g_sessionId = 0;
}

static void OnBytesReceived(int sessionId, const void *data, unsigned int dataLen)
{
    (void)data;
    printf("[SOFTBUS] OnBytesReceived: sessionId=%d, dataLen=%u\n", sessionId, dataLen);
}

static void OnMessageReceived(int sessionId, const void *data, unsigned int dataLen)
{
    (void)data;
    printf("[SOFTBUS] OnMessageReceived: sessionId=%d, dataLen=%u\n", sessionId, dataLen);
}

/* ========== 设备状态回调接口实现 ========== */

static void OnNodeOnline(NodeBasicInfo *info)
{
    if (info == NULL) {
        return;
    }
    if (strcpy_s(g_networkId, NETWORK_ID_SIZE, info->networkId) != EOK) {
        return;
    }
    printf("[SOFTBUS] Device online: name=%s, networkId=%s\n",
           info->deviceName, info->networkId);
}

static void OnNodeOffline(NodeBasicInfo *info)
{
    if (info == NULL) {
        return;
    }
    printf("[SOFTBUS] Device offline: name=%s, networkId=%s\n",
           info->deviceName, info->networkId);
}

/* ========== 会话监听器和设备状态回调结构体 ========== */

static ISessionListener g_sessionListener = {
    .OnSessionOpened = OnSessionOpened,
    .OnSessionClosed = OnSessionClosed,
    .OnBytesReceived = OnBytesReceived,
    .OnMessageReceived = OnMessageReceived,
};

static INodeStateCb g_nodeStateCb = {
    .events = EVENT_NODE_STATE_ONLINE | EVENT_NODE_STATE_OFFLINE,
    .onNodeOnline = OnNodeOnline,
    .onNodeOffline = OnNodeOffline,
};

static SessionAttribute g_sessionAttr = {
    .dataType = TYPE_BYTES,
};

/* ========== 核心子功能函数 ========== */

static void Start(void)
{
    printf("[SOFTBUS] ----- Step 1: RegNodeDeviceStateCb -----\n");
    int32_t ret = RegNodeDeviceStateCb(g_pkgName, &g_nodeStateCb);
    if (ret != SOFTBUS_OK) {
        printf("[SOFTBUS] ERROR: RegNodeDeviceStateCb failed! ret=%d\n", ret);
    } else {
        printf("[SOFTBUS] RegNodeDeviceStateCb success!\n");
    }
}

void Wait(void)
{
    printf("[SOFTBUS] Waiting %d seconds for device discovery...\n", SLEEP_TIME);
    sleep(SLEEP_TIME);
}

static int CreateServer(void)
{
    printf("[SOFTBUS] ----- Step 2: CreateSessionServer -----\n");
    printf("[SOFTBUS] pkg=%s, session=%s\n", g_pkgName, g_sessionName);
    int ret = CreateSessionServer(g_pkgName, g_sessionName, &g_sessionListener);
    if (ret != SOFTBUS_OK) {
        printf("[SOFTBUS] ERROR: CreateSessionServer failed! ret=%d\n", ret);
        return ret;
    }
    printf("[SOFTBUS] CreateSessionServer success!\n");
    return SOFTBUS_OK;
}

static int OpenSessionToPeer(void)
{
    printf("[SOFTBUS] ----- Step 3: OpenSession -----\n");
    printf("[SOFTBUS] peerDeviceId=%s, groupId=%s\n", g_networkId, g_groupid);
    int sessionId = OpenSession(g_sessionName, g_sessionName, g_networkId,
                                g_groupid, &g_sessionAttr);
    if (sessionId < 0) {
        printf("[SOFTBUS] ERROR: OpenSession failed! ret=%d\n", sessionId);
        return sessionId;
    }
    g_sessionId = sessionId;
    printf("[SOFTBUS] OpenSession success! sessionId=%d\n", sessionId);
    return SOFTBUS_OK;
}

static int SendByteData(void)
{
    char sendData[] = "Hello OpenHarmony Distributed SoftBus!";
    unsigned int dataLen = strlen(sendData) + 1;

    printf("[SOFTBUS] ----- Step 4: SendBytes -----\n");
    printf("[SOFTBUS] data=\"%s\", len=%u\n", sendData, dataLen);
    int ret = SendBytes(g_sessionId, sendData, dataLen);
    if (ret != SOFTBUS_OK) {
        printf("[SOFTBUS] ERROR: SendBytes failed! ret=%d\n", ret);
        return ret;
    }
    printf("[SOFTBUS] SendBytes success!\n");
    return SOFTBUS_OK;
}

static int SendMessageData(void)
{
    char message[] = "Distributed SoftBus Message Test - 分布式软总线消息测试";
    unsigned int msgLen = strlen(message) + 1;

    printf("[SOFTBUS] ----- Step 5: SendMessage -----\n");
    printf("[SOFTBUS] msg=\"%s\", len=%u\n", message, msgLen);
    int ret = SendMessage(g_sessionId, message, msgLen);
    if (ret != SOFTBUS_OK) {
        printf("[SOFTBUS] ERROR: SendMessage failed! ret=%d\n", ret);
        return ret;
    }
    printf("[SOFTBUS] SendMessage success!\n");
    return SOFTBUS_OK;
}

static void CloseCurrentSession(void)
{
    printf("[SOFTBUS] ----- Step 6: CloseSession -----\n");
    if (g_sessionId <= 0) {
        printf("[SOFTBUS] No active session to close.\n");
        return;
    }
    printf("[SOFTBUS] Closing session: sessionId=%d\n", g_sessionId);
    CloseSession(g_sessionId);
    printf("[SOFTBUS] CloseSession called.\n");
}

static void RemoveServer(void)
{
    printf("[SOFTBUS] ----- Step 7: RemoveSessionServer -----\n");
    printf("[SOFTBUS] pkg=%s, session=%s\n", g_pkgName, g_sessionName);
    int ret = RemoveSessionServer(g_pkgName, g_sessionName);
    if (ret != SOFTBUS_OK) {
        printf("[SOFTBUS] ERROR: RemoveSessionServer failed! ret=%d\n", ret);
        return;
    }
    printf("[SOFTBUS] RemoveSessionServer success!\n");
}

static void Stop(void)
{
    printf("[SOFTBUS] ----- Step 8: UnregNodeDeviceStateCb -----\n");
    int ret = UnregNodeDeviceStateCb(&g_nodeStateCb);
    if (ret != SOFTBUS_OK) {
        printf("[SOFTBUS] ERROR: UnregNodeDeviceStateCb failed! ret=%d\n", ret);
    } else {
        printf("[SOFTBUS] UnregNodeDeviceStateCb success!\n");
    }
}

/* ================================================================
 * 综合测试入口 —— 一条命令执行全部测试流程:
 *   Start → Wait → CreateServer → OpenSession → SendBytes →
 *   SendMessage → CloseSession → RemoveServer → Stop
 * ================================================================ */
int DsoftbusAllTest(void)
{
    printf("\n");
    printf("2024382026 陈冠宇 dsoftbus test start...\n");
    printf("╔══════════════════════════════════════════════════════╗\n");
    printf("║      分布式软总线实验手册案例 - 综合测试              ║\n");
    printf("╚══════════════════════════════════════════════════════╝\n\n");

    /* 1. 注册设备状态回调 */
    Start();

    /* 2. 等待设备发现 */
    Wait();

    /* 3. 创建会话服务端 */
    if (CreateServer() != SOFTBUS_OK) { Stop(); return -1; }

    /* 4. 打开会话 */
    if (OpenSessionToPeer() != SOFTBUS_OK) { RemoveServer(); Stop(); return -1; }

    /* 5. 等待会话就绪 */
    sleep(2);

    /* 6. 发送字节数据 */
    SendByteData();

    /* 7. 发送消息数据 */
    SendMessageData();

    /* 8. 等待传输完成 */
    sleep(2);

    /* 9. 关闭会话 */
    CloseCurrentSession();

    /* 10. 移除会话服务端 */
    RemoveServer();

    /* 11. 注销回调 */
    Stop();

    printf("\n2024382026 陈冠宇 dsoftbus test finished!\n");
    return 0;
}

/* ========== 注册 shell 命令 ========== */
int registerMyDsoftbus(void)
{
    OsShellInit();
    osCmdReg(CMD_TYPE_EX, "dsoftbus", 0, (CmdCallBackFunc)DsoftbusAllTest);
}
