#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"
#include "shcmd.h"
#include "fcntl.h"
#include "unistd.h"

#include "sys/stat.h"
#include "cmsis_os2.h"

#define TEXT_CMD "text"
#define TEXT_ROOT "/littlefs/"

int test_fd = 0;
char textName[100] = {0};

int test_rule(){
    printf("\n*********textdemo*********\n");
    printf("input: text filename\n");
}

int test_write(char *data){
    int len = write(test_fd,data,strlen(data));
    if(len != strlen(data)){
        printf("text failed\n");
        return len;
    }
    len = fsync(test_fd);
    if(len != LOS_OK){
        printf("fsync failed\n");
    }
    printf("text success\n");
    return len;
}

int textopen(){
    // test_fd = open(textName,O_RDWR|O_CREAT|O_TRUNC,0777);
    test_fd = open(textName,O_RDWR|O_CREAT,0777);
    if(test_fd <= 0){
        printf("open %s failed",textName);
    }
    printf("open %s success\n",textName);
    return test_fd;
}

void textmain(INT32 argc,const char *argv[]){
    if(argv[0] == NULL){
        test_rule();
        printf("the file name can not be null!\n");
        return;
    }else{
        sprintf_s(textName,100,"%s%s",TEXT_ROOT,argv[0]);
        printf("textName=%s\n",textName);
        if(textopen() > 0){
            printf("textopen successfully\n");
        }
    }
}

// VOID ExampleLittlefs(VOID)
// {
//     int ret;
//     int fd;
//     ssize_t len;
//     off_t off;
//     char dirName[BUF_SIZE] = TEST_ROOT"/test";
//     char fileName[BUF_SIZE] = TEST_ROOT"/test/file.txt";
//     char writeBuf[BUF_SIZE] = "Hello OpenHarmony!";
//     char readBuf[BUF_SIZE] = {0};

//     /* 创建测试目录 */
//     ret = mkdir(dirName, 0777);
//     if (ret != LOS_OK) {
//         printf("mkdir failed.\n");
//         return;
//     }

//     /* 创建可读写测试文件 */
//     fd = open(fileName, O_RDWR | O_CREAT, 0777);
//     if (fd < 0) {
//         printf("open file failed.\n");
//         return;
//     }

//     /* 将writeBuf中的内容写入文件 */
//     len = write(fd, writeBuf, strlen(writeBuf));
//     if (len != strlen(writeBuf)) {
//         printf("write file failed.\n");
//         return;
//     }

//     /* 将文件内容刷入存储设备中 */
//     ret = fsync(fd);
//     if (ret != LOS_OK) {
//         printf("fsync failed.\n");
//         return;
//     }

//     /* 将读写指针偏移至文件头 */
//     off = lseek(fd, 0, SEEK_SET);
//     if (off != 0) {
//         printf("lseek failed.\n");
//         return;
//     }

//     /* 将文件内容读出至readBuf中，读取长度为readBuf大小 */
//     len = read(fd, readBuf, sizeof(readBuf));
//     if (len != strlen(writeBuf)) {
//         printf("read file failed.\n");
//         return;
//     }
//     printf("%s\n", readBuf);

//     /* 关闭测试文件 */
//     ret = close(fd);
//     if (ret != LOS_OK) {
//         printf("close failed.\n");
//         return;
//     }

//     /* 删除测试文件 */
//     ret = unlink(fileName);
//     if (ret != LOS_OK) {
//         printf("unlink failed.\n");
//         return;
//     }

//     /* 删除测试目录 */
//     ret = rmdir(dirName);
//     if (ret != LOS_OK) {
//         printf("rmdir failed.\n");
//         return;
//     }

//     return LOS_OK;
// }

// UINT32 ExampleTaskCaseEntry(VOID){
//     UINT32 ret;
//     TSK_INIT_PARAM_S initParam = {0};
//     LOS_TaskLock();
//     initParam.pfnTaskEntry = (TSK_ENTRY_FUNC)ExampleTaskHi;
//     initParam.usTaskPrio = 25;
//     initParam.pcName = "textdemo";
//     initParam.uwStackSize = 4096;
//     // initParam.uwResved   = LOS_TASK_ATTR_JOINABLE;

//     /* 创建高优先级任务，由于锁任务调度，任务创建成功后不会马上执行 */
//     ret = LOS_TaskCreate(&g_taskHiID, &initParam);
//     if (ret != LOS_OK) {
//         LOS_TaskUnlock();
//         printf("ExampleTaskHi create Failed! ret=%d\n", ret);
//         return LOS_NOK;
//     }
//     LOS_TaskUnlock();
//     return LOS_OK;
// }


int registerTextDemo(){
    OsShellInit();
    osCmdReg(CMD_TYPE_EX,"text",0,(CMD_CBK_FUNC)textmain);
}