/*
 * 鸿蒙南向设备基础课程期末考试 - 试题三（文件系统）
 * 简易文本编辑器 —— 基于 LittleFS 与 shell 命令交互
 *
 * 功能:
 *   1. 文件打开（读写+自动创建）、关闭、删除
 *   2. 文本写入（fsync 同步刷新）、文本读取（打印到终端）
 *   3. 文件加载时显示已有内容，支持追加编辑
 *   4. 逐字符串口输入：退格删除、回车保存退出、Ctrl+C 清空当前行
 *   5. 使用说明打印
 *   6. 统一注册入口，shell 命令 "text"
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "shell.h"
#include "shcmd.h"
#include "fcntl.h"
#include "unistd.h"
#include "sys/stat.h"
#include "uart.h"

/* ========== 全局状态变量 ========== */
#define TEXT_ROOT       "/littlefs/"
#define TEXT_BUF_SIZE   2048            /* 文本缓冲区不少于 2KB */

static int   g_textFd    = -1;          /* 文件描述符           */
static char  g_textBuf[TEXT_BUF_SIZE];  /* 文本内容缓冲区        */
static int   g_writePos  = 0;           /* 当前写入位置指针      */
static char  g_filePath[128];           /* 文件路径缓冲区        */

/* ========== UART 字符输入输出 ========== */

/* 从串口读取一个字符（阻塞等待） */
static int UartGetChar(void)
{
    int c;
    do {
        c = UartGetc();
    } while (c == 0);
    return c;
}

/* 向串口输出一个字符 */
static void UartPutChar(int c)
{
    (void)UartPutc(c, NULL);
}

/* ========== 使用说明打印 ========== */
static void TextUsage(void)
{
    printf("\n");
    printf("============================================\n");
    printf("  简易文本编辑器 (LittleFS Text Editor)\n");
    printf("============================================\n");
    printf("  使用方法:\n");
    printf("    text              显示本帮助信息\n");
    printf("    text <文件名>      打开/创建文件并进入编辑\n");
    printf("\n");
    printf("  文件存储路径: %s\n", TEXT_ROOT);
    printf("\n");
    printf("  编辑模式操作:\n");
    printf("    - 逐字符输入文本内容\n");
    printf("    - 退格键 (Backspace)  删除前一个字符\n");
    printf("    - Ctrl+N              输入换行\n");
    printf("    - 回车键 (Enter)      保存文件并退出编辑\n");
    printf("    - Ctrl+C              清空当前行\n");
    printf("\n");
    printf("  示例:\n");
    printf("    text test.c          编辑 /littlefs/test.c\n");
    printf("    text                 查看本帮助\n");
    printf("============================================\n");
    printf("\n");
}

/* ========== 文件操作功能 ========== */

/*
 * 打开文件（支持读写与自动创建）
 * 返回: 文件描述符，失败返回 -1
 */
static int TextOpen(const char *path)
{
    int fd;

    fd = open(path, O_RDWR | O_CREAT, 0777);
    if (fd < 0) {
        printf("[ERROR] 打开文件失败: %s (errno: %d)\n", path, fd);
        return -1;
    }
    printf("[INFO] 文件已打开: %s (fd=%d)\n", path, fd);
    return fd;
}

/*
 * 关闭文件
 * 返回: 0 成功，-1 失败
 */
static int TextClose(void)
{
    int ret;

    if (g_textFd < 0) {
        printf("[WARN] 没有打开的文件需要关闭\n");
        return -1;
    }

    ret = close(g_textFd);
    if (ret != 0) {
        printf("[ERROR] 关闭文件失败: %s\n", g_filePath);
        return -1;
    }
    printf("[INFO] 文件已关闭: %s\n", g_filePath);
    g_textFd = -1;
    return 0;
}

/*
 * 删除文件
 * 返回: 0 成功，-1 失败
 */
static int TextDelete(const char *path)
{
    int ret;

    ret = unlink(path);
    if (ret != 0) {
        printf("[ERROR] 删除文件失败: %s\n", path);
        return -1;
    }
    printf("[INFO] 文件已删除: %s\n", path);
    return 0;
}

/* ========== 文件读写功能 ========== */

/*
 * 将缓冲区内容写入已打开的文件并同步刷新到存储介质
 */
static int TextWrite(void)
{
    ssize_t len;
    int ret;

    if (g_textFd < 0) {
        printf("[ERROR] 文件未打开，无法写入\n");
        return -1;
    }

    if (g_writePos == 0) {
        printf("[WARN] 缓冲区为空，跳过写入\n");
        return 0;
    }

    /* 移动写入指针到文件末尾（追加模式） */
    (void)lseek(g_textFd, 0, SEEK_END);

    len = write(g_textFd, g_textBuf, g_writePos);
    if (len != g_writePos) {
        printf("[ERROR] 写入文件失败: 期望 %d 字节, 实际 %d 字节\n",
               g_writePos, (int)len);
        return -1;
    }

    /* 同步刷新到存储介质 */
    ret = fsync(g_textFd);
    if (ret != 0) {
        printf("[ERROR] fsync 同步失败\n");
        return -1;
    }

    printf("[INFO] 成功写入 %d 字节并同步到存储介质\n", (int)len);
    return 0;
}

/*
 * 将文件内容读入缓冲区并在终端打印显示
 * 返回: 读取的字节数，失败返回 -1
 */
static int TextRead(void)
{
    ssize_t len;
    off_t off;
    int ret;

    if (g_textFd < 0) {
        printf("[ERROR] 文件未打开，无法读取\n");
        return -1;
    }

    /* 移动读写指针到文件开头 */
    off = lseek(g_textFd, 0, SEEK_SET);
    if (off != 0) {
        printf("[ERROR] lseek 失败\n");
        return -1;
    }

    /* 读取文件内容到缓冲区 */
    memset(g_textBuf, 0, TEXT_BUF_SIZE);
    len = read(g_textFd, g_textBuf, TEXT_BUF_SIZE - 1);
    if (len < 0) {
        printf("[ERROR] 读取文件失败\n");
        return -1;
    }

    if (len > 0) {
        printf("\n[INFO] --- 文件已有内容 (%d 字节) ---\n", (int)len);
        printf("%s", g_textBuf);
        if (g_textBuf[len - 1] != '\n') {
            printf("\n");
        }
        printf("[INFO] --- 内容结束 ---\n\n");
        g_writePos = (int)len;   /* 写入指针定位到内容末尾 */
    } else {
        printf("[INFO] 文件为空，开始新编辑\n");
        g_writePos = 0;
    }

    return (int)len;
}

/* ========== 文件加载逻辑 ========== */

/*
 * 加载文件: 打开 → 读取已有内容并展示 → 定位写入指针到末尾
 *          → 关闭 → 重新以读写模式打开
 */
static int TextLoad(const char *path)
{
    int fd;

    /* 先尝试读取文件的已有内容 */
    fd = open(path, O_RDONLY);
    if (fd >= 0) {
        printf("[INFO] 文件已存在，加载已有内容...\n");
        g_textFd = fd;
        (void)TextRead();        /* 读取并显示内容，设置 g_writePos */
        close(fd);
        g_textFd = -1;
    } else {
        printf("[INFO] 文件不存在，将创建新文件\n");
        g_writePos = 0;
    }

    /* 重新以读写模式打开文件（支持自动创建） */
    g_textFd = TextOpen(path);
    if (g_textFd < 0) {
        return -1;
    }

    return 0;
}

/* ========== 文本输入逻辑 ========== */

/*
 * 循环等待用户输入的编辑功能，逐字符接收串口输入。
 *   退格键:  删除上一个字符
 *   回车键:  保存缓冲区内容并退出编辑
 *   Ctrl+C:  中断输入并清空当前行
 */
static void TextEdit(void)
{
    int ch;

    printf("\n========== 编辑模式 ==========\n");
    printf("输入文本 (Ctrl+N换行, 回车保存退出, Ctrl+C清空行, 退格删除):\n");
    printf("> ");

    /* 从已有内容的末尾继续编辑 */
    if (g_writePos > 0) {
        printf("%s", g_textBuf);
    }

    while (1) {
        ch = UartGetChar();   /* 阻塞等待串口输入一个字符 */

        /* Ctrl+C: 清空当前行 */
        if (ch == 0x03) {
            printf("\n[INFO] Ctrl+C 按下，清空当前缓冲区\n");
            memset(g_textBuf, 0, TEXT_BUF_SIZE);
            g_writePos = 0;
            printf("> ");
            continue;
        }

        /* Ctrl+N: 输入换行符 */
        if (ch == 0x0E) {
            if (g_writePos < TEXT_BUF_SIZE - 1) {
                g_textBuf[g_writePos] = '\n';
                g_writePos++;
                g_textBuf[g_writePos] = '\0';
                printf("\n> ");
            }
            continue;
        }

        /* 回车键: 保存并退出 */
        if (ch == '\r' || ch == '\n') {
            printf("\n[INFO] 回车键按下，保存文件并退出编辑\n");
            break;
        }

        /* 退格键: 删除上一个字符 */
        if (ch == 0x08 || ch == 0x7F) {
            if (g_writePos > 0) {
                g_writePos--;
                g_textBuf[g_writePos] = '\0';
                printf("\b \b");   /* 终端回退一格 */
            }
            continue;
        }

        /* 普通字符: 追加到缓冲区 */
        if (g_writePos < TEXT_BUF_SIZE - 1) {
            g_textBuf[g_writePos] = (char)ch;
            g_writePos++;
            g_textBuf[g_writePos] = '\0';
            UartPutChar(ch);       /* 回显字符 */
        } else {
            printf("\n[WARN] 缓冲区已满 (%d 字节)，请回车保存\n", TEXT_BUF_SIZE);
        }
    }
}

/* ========== 主命令处理函数 ========== */

/*
 * text 命令主函数:
 *   - 未输入文件名: 打印使用说明
 *   - 输入文件名:   拼接路径 → 加载文件 → 显示已有内容 →
 *                   进入编辑模式 → 保存 → 关闭文件
 */
void TextMain(INT32 argc, const char *argv[])
{
    /* 未输入文件名，打印使用说明 */
    if (argc == 0 || argv[0] == NULL || strlen(argv[0]) == 0) {
        TextUsage();
        return;
    }

    printf("\n2024382026 陈冠宇 text editor start...\n");

    /* 拼接完整路径: /littlefs/<用户输入的文件名> */
    if (snprintf_s(g_filePath, sizeof(g_filePath), sizeof(g_filePath) - 1,
                   "%s%s", TEXT_ROOT, argv[0]) < 0) {
        printf("[ERROR] 路径拼接失败\n");
        return;
    }
    printf("[INFO] 目标文件: %s\n", g_filePath);

    /* 加载文件（读取已有内容并展示，定位写入指针） */
    if (TextLoad(g_filePath) != 0) {
        printf("[ERROR] 文件加载失败\n");
        return;
    }

    /* 进入文本编辑模式（逐字符输入） */
    TextEdit();

    /* 写入缓冲区内容到文件 */
    if (TextWrite() != 0) {
        printf("[ERROR] 文件保存失败\n");
        TextClose();
        return;
    }

    /* 关闭文件 */
    TextClose();

    printf("[INFO] 文件保存成功: %s\n", g_filePath);
    printf("2024382026 陈冠宇 text editor finished.\n\n");
}

/* ========== 注册入口 ========== */

int registerTextDemo(void)
{
    OsShellInit();
    osCmdReg(CMD_TYPE_EX, "text", 0, (CMD_CBK_FUNC)TextMain);
}
