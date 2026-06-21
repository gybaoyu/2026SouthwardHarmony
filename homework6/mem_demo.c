#include "mem_demo.h"

#include "shell.h"
#include "shcmd.h"

void MemBasicAlloc(void)
{
   g_memPool1 = LOS_MemAlloc(LOSCFG_SYS_HEAP_ADDR,TEST_MEM_SIZE);
   g_testPool1 = LOS_MemAlloc(LOSCFG_SYS_HEAP_ADDR,TEST_POOL_SIZE);
   if((g_memPool1 == NULL)||(g_testPool1 == NULL)){
    PRINT_ERR("[ERROR] MemBasicAlloc:alloc failed , other mem tests would be failed %x %x\n",g_memPool1,g_testPool1);
   }else{
    printf("MemBasicAlloc:alloc OK, with original LOSCFG_SYS_HEAP_ADDR=0x%x ,g_memPool1 = 0x%x,g_testPool1=0x%x\n",
    LOSCFG_SYS_HEAP_ADDR,g_memPool1,g_testPool1);
   }
}

void MemBasicFree(void)
{
    UINT32 ret;

    if(g_memPool1!=NULL){
        ret=LOS_MemFree(LOSCFG_SYS_HEAP_ADDR, g_memPool1);
        if(ret==LOS_OK){
            printf("\n---MemBasicFree:g_memPool1 memory free ok!\n");
        }else{
            PRINT_ERR("\n---MemBasicFree:g_memPool1 memory free failed!\n");
        }
    }else{
       printf("\n---MemBasicFree:g_memPool1 is already NULL!\n");
    }

    if(g_testPool1!=NULL){
        ret=LOS_MemFree(LOSCFG_SYS_HEAP_ADDR, g_testPool1);
        if(ret==LOS_OK){
            printf("\n---MemBasicFree:g_testPool1 memory free ok!\n");
        }else{
            PRINT_ERR("\n---MemBasicFree:g_testPool1 memory free failed!\n");
        }
    }else{
        printf("\n---MemBasicFree:g_testPool1 is already NULL!\n");
    }
}

void MemTestInit(void)
{
    UINT32 ret;
    ret=LOS_MemInit(g_memPool1, TEST_MEM_SIZE);
    if(ret==LOS_OK)
        printf("\n---MemTestInit: memory init of g_memPool1 ok!\n");
    else
        PRINT_ERR("\n---MemTestInit: memory init of g_memPool1 failed!\n");
}

void MemTestDeinit(void)
{
    UINT32 ret;
    ret=memset_s(g_memPool1, TEST_MEM_SIZE, 0, TEST_MEM_SIZE);
    if(ret==LOS_OK)
        printf("\n---MemTestDeinit: zero memory of g_memPool1 ok!\n");
    else
        PRINT_ERR("\n---MemTestDeinit: zero memory of g_memPool1 failed!\n");

    ret=LOS_MemDeInit(g_memPool1);

    if(ret==LOS_OK)
        printf("\n---MemTestDeinit: memory pool with g_memPool1 de-init ok!\n");
    else
        PRINT_ERR("\n---MemTestDeinit: memory pool with g_memPool1 de-init failed!\n");
}

VOID Basic_Test(void)
{
    MemBasicAlloc();
    MemBasicFree();
}
VOID BasicAlloc_Free(void)
{
    UINT32 ret;
    UINT32 size;
    void *p = NULL;

    size = 0x100;
    p = LOS_MemAlloc((void *)LOSCFG_SYS_HEAP_ADDR, size);
    if(p!=NULL){
        printf("\n---BasicAlloc_Free:mem alloc ok, p=0x%x\n",p);
    }else{
        PRINT_ERR("\n---BasicAlloc_Free:mem alloc failed.\n");
    }

    (void)memset_s(p, size, 1, size);

    printf("\n---BasicAlloc_Free:memory contents from p to p+size\n");
    for(int i=0;i<size;i++){
        printf("0x%x ", *(char *) (p+i));
        if((i+1)%16==0){
            printf("\n");
        }
    }

    ret = LOS_MemFree((void *)LOSCFG_SYS_HEAP_ADDR, p);
    if(p!=NULL){
        printf("\n---BasicAlloc_Free:mem free ok, p=0x%x\n",p);
    }else{
        PRINT_ERR("\n---BasicAlloc_Free:mem free failed.\n");
    }
}

VOID Zero_Max_Alloc_Free(void)
{
    UINT32 size;
    void *p = NULL;
    size = 0;
    p = LOS_MemAlloc((void *)LOSCFG_SYS_HEAP_ADDR, size);
    if(p!=NULL){
        PRINT_ERR("\n---Zero_Max_Alloc_Free:eme alloc with size of 0 failed.\n");
    }else{
        printf("\n---Zero_Max_Alloc_Free:mem alloc with size of 0 ok, p=%x\n",p);
    }

    size = 0xffffffff;
    p = LOS_MemAlloc((void *)LOSCFG_SYS_HEAP_ADDR, size);
    if(p!=NULL){
        PRINT_ERR("\n---Zero_Max_Alloc_Free:eme alloc with size of max failed.\n");
    }else{
        printf("\n---Zero_Max_Alloc_Free:mem alloc with size of max ok, p=%x\n",p);
    }
}

VOID oneByteAlloc(void)
{
    UINT32 ret;
    UINT32 size;
    void *p = NULL;

    size = 0x1;
    p = LOS_MemAlloc((void *)LOSCFG_SYS_HEAP_ADDR, size);
    if(p!=NULL){
        printf("\n---OneByteAlloc:memory alloc ok, p=%x\n",p);
    }else{
        PRINT_ERR("\n---OneByteAlloc:memory failed.\n");
    }
    ret = LOS_MemFree((void *)LOSCFG_SYS_HEAP_ADDR, p);
}

VOID InitMemWithIlligalParam(void)
{
    UINT32 ret;

    ret = LOS_MemInit(NULL, TEST_MEM_SIZE);
    if(ret==LOS_NOK){
        printf("\n---InitMemWithIlligalParam: memory init with *pool=NULL,\
            size=TEST_MEM_SIZE returns LOS_NOK!\n");
    }else{
        PRINT_ERR("\n---InitMemWithIlligalParammemory init with *pool=NULL,\
            size=TEST_MEM_SIZE failed!\n");
    }

    ret = LOS_MemInit(g_memPool1, MIN_MEM_POOL_SIZE - 1);
    if(ret==LOS_NOK){
        printf("\n---InitMemWithIlligalParam: memory init with *pool=g_memPool1,\
            size=MIN_MEM_POOL_SIZE - 1 returns LOS_NOK!\n");
    }else{
    PRINT_ERR("\n---InitMemWithIlligalParam: memory init with *pool=g_memPool1,\
        size=MIN_MEM_POOL_SIZE - 1 failed!\n");
    }
}


VOID poolInitMem(void)
{
    void *pool = NULL;
    UINT32 ret;
    printf("\n---[DEBUG]:g_memPool1=%x\n",g_memPool1);
    ret = LOS_MemInit(g_memPool1, TEST_MEM_SIZE);
    if(ret==LOS_OK)
        printf("\n---poolInitMem: memory init with *pool=g_memPool1,size=TEST_MEM_SIZE ok!\n");
    else
        PRINT_ERR("\n---poolInitMem: memory init with *pool=g_memPool1,size=TEST_MEM_SIZE failed!\n");

    pool = (void *)((UINT32)(UINTPTR)g_memPool1 + LOS_MemPoolSizeGet(g_memPool1));

    ret = LOS_MemInit(pool, MIN_DLNK_POOL_SIZE + 1000);
    if(ret==LOS_OK)
        printf("\n---poolInitMem: memory init with *pool=pool,size=MIN_DLNK_POOL_SIZE + 1000 ok!\n");
    else
        PRINT_ERR("\n---poolInitMem: memory init with *pool=pool,size=MIN_DLNK_POOL_SIZE + 1000 failed!\n");

    ret = LOS_MemDeInit(pool);
    if(ret==LOS_OK)
        printf("\n---poolInitMem: memory de-init with *pool=pool ok!\n");
    else
        PRINT_ERR("\n---poolInitMem: memory de-init with *pool=pool failed!\n");
    ret = LOS_MemDeInit(g_memPool1);
    if(ret==LOS_OK)
        printf("\n---poolInitMem: memory de-init with *pool=g_memPool1 ok!\n");
    else
        PRINT_ERR("\n---poolInitMem: memory de-init with *pool=g_memPool1 failed!\n");
}

VOID ContinueAlloc(void)
{
    UINT32 size, memFreeSize;
    unsigned long counter=0;
    void *p0 = NULL;

    MemTestInit();

    size = 0x400;

    while (1) {
        p0 = LOS_MemAlloc(g_memPool1, size);
        if (p0 == NULL) {
            break;
        }else{
            counter++;
        }
    }
    memFreeSize=LOS_MemPoolSizeGet(g_memPool1) - LOS_MemTotalUsedGet(g_memPool1);
    if (memFreeSize >= (size + LOS_MEM_NODE_HEAD_SIZE + LOS_MEM_POOL_SIZE)) {
        printf("\n---ContinueAlloc: %ld time(s), \
            the free memory size is %d\n", counter, memFreeSize);
    }else
        printf("\n---ContinueAlloc: after %ld time(s) alloc, \
            the mememory is out!\n", counter);
    size = size;
    p0 = LOS_MemAlloc(g_memPool1, size);
    if(p0!=NULL)
        printf("\n---ContinueAlloc: memory alloc ok, p0=%x\n", p0);
    else
        PRINT_ERR("\n---ContinueAlloc: memory alloc failed.\n");
    EXIT:
        MemTestDeinit();
}

VOID ReallocMem(VOID)
{
    UINT32 size = 0x500;
    UINT32 freesize=0;
    void *p0 = NULL;
    void *f0 = NULL;
    void *p[(TEST_MEM_SIZE) / 0x500] = {NULL};
    int i = 0;

    MemTestInit();

    for (p0 = LOS_MemAlloc(g_memPool1, size); p0 != NULL; i++) {
    printf("\n---ReallocMem: New mem p[%d] allocated with address = 0x%x\n", i, p0);
    freesize=LOS_MemPoolSizeGet(g_memPool1) - LOS_MemTotalUsedGet(g_memPool1);
    printf("\n---ReallocMem: freesize = %d .\n", freesize);
    p0 = LOS_MemAlloc(g_memPool1, size);
    }

    p0 = LOS_MemRealloc(g_memPool1, p0, size / 2); // 2, The reallocated memory size is half of its previous size.
    if(p0!=NULL){
        printf("\n---ReallocMem: Re-alloced mem with half of its previous size. New address =0x%x \n", p0);
    }else{
        printf("\n---ReallocMem: Re-alloced mem with half of its previous size FAILED \n");
    }
    f0 = LOS_MemAlloc(g_memPool1, size / 4); // 4, The reallocated memory size is a quarter of its previous size.
    if(f0!=NULL){
        printf("\n---ReallocMem: Re-alloced mem with quarter of its previous size. New address =0x%x \n", f0);
    }else{
        printf("\n---ReallocMem: Re-alloced mem with quarter of its previous size FAILED \n");
    }

    if (((UINT32)(UINTPTR)p[0]) < ((UINT32)(UINTPTR)f0) && ((UINT32)(UINTPTR)f0 < ((UINT32)(UINTPTR)p[1]))) {
        printf("\n---ReallocMem: pointer position ok,p[0]=0x%x, f0=0x%x, p[1]=0x%x \n", p[0],f0,p[1]);
    }else{
        printf("\n---ReallocMem: pointer position error, p[0]=0x%x, f0=0x%x, p[1]=0x%x \n", p[0],f0,p[1]);
    }
    EXIT:
        MemTestDeinit();
}

VOID Mem_Demo(void){
    printf("-------2024382026 cgy start Basic_Test...\n");
    Basic_Test();
    printf("+++++++2024382026 cgy end Basic_Test...\n");

    printf("-------2024382026 cgy start BasicAlloc_Free...\n");
    BasicAlloc_Free();
    printf("+++++++2024382026 cgy end BasicAlloc_Free...\n");

    printf("-------2024382026 cgy start Zero_Max_Alloc_Free...\n");
    Zero_Max_Alloc_Free();
    printf("+++++++2024382026 cgy end Zero_Max_Alloc_Free...\n");

    printf("-------2024382026 cgy start oneByteAlloc...\n");
    oneByteAlloc();
    printf("+++++++2024382026 cgy end oneByteAlloc...\n");

    printf("-------2024382026 cgy start InitMemWithIlligalParam...\n");
    InitMemWithIlligalParam();
    printf("+++++++2024382026 cgy end InitMemWithIlligalParam...\n");

    printf("-------2024382026 cgy start poolInitMem...\n");
    poolInitMem();
    printf("+++++++2024382026 cgy end poolInitMem...\n");

    printf("-------2024382026 cgy start ContinueAlloc...\n");
    ContinueAlloc();
    printf("+++++++2024382026 cgy end ContinueAlloc...\n");

    printf("-------2024382026 cgy start ReallocMem...\n");
    ReallocMem();
    printf("+++++++2024382026 cgy end ReallocMem...\n");
}

extern void Mem_Demo();
UINT32 Shell_Cmd_Event(UINT32 argc, const CHAR **argv)
{
}

UINT32 Shell_Cmd_Queue(UINT32 argc, const CHAR **argv)
{
}

UINT32 Shell_Cmd_Mutex(UINT32 argc, const CHAR **argv)
{
}

UINT32 Shell_Cmd_Semaphore(UINT32 argc, const CHAR **argv)
{
}

UINT32 Shell_Cmd_Memory(UINT32 argc, const CHAR **argv)
{
    (void)argc;
    (void)argv;
    printf("This is memory demo launch command.\n");
    Mem_Demo();

    return 0;
}

void regMyShellCmd(void)
{
    osCmdReg(CMD_TYPE_STD, "event_demo", 0, Shell_Cmd_Event);
    osCmdReg(CMD_TYPE_STD, "queue_demo", 0, Shell_Cmd_Queue);
    osCmdReg(CMD_TYPE_STD, "mutex_demo", 0, Shell_Cmd_Mutex);
    osCmdReg(CMD_TYPE_STD, "sem_demo", 0, Shell_Cmd_Semaphore);
    osCmdReg(CMD_TYPE_STD, "mem_demo", 0, Shell_Cmd_Memory);
}