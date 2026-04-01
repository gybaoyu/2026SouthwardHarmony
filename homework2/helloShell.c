#include "shell.h"
#include "shcmd.h"
int cmdTest(void){
    printf("2024382026cgy hello cmd test!\n");
}

int registerUserTestCMD(){
    OsShellInit();
    osCmdReg(CMD_TYPE_EX,"test",0,(CmdCallBackFunc)cmdTest);
}