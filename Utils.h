// Utils.h
// cyq写
// 功能函数包
// 主要包含链表和文件的操作，大家尽量直接调用，因为这些都有完善的错误处理机制
// 另外诸如快排之类的记得调用库函数，不知道的问一下ai，尽量少手动实现多用库函数
// 效率高不容易错，实在没有还不太会写的跟我说一声我再补充到这个文件里
// 最后调用时记得传对参，是传地址还是变量看清楚

//新将分页和链表操作按书上要求移动至List.h文件

#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Common.h"
#include "List.h"

// 下面开始都是文件了

// 保存链表到文件
static int File_SaveList(const char* filename, void* head,
    void* (*getNextFunc)(void*),
    int (*writeDataFunc)(FILE*, void*))
{
    if (!filename || !writeDataFunc)
    {
        printf("参数不对\n");
        return -1;
    }
    FILE* fp = fopen(filename, "wb");
    if (!fp)
    {
        printf("打不开文件 %s\n", filename);
        return -1;
    }
    int count = 0;
    void* cur = head;
    while (cur)
    {
        if (writeDataFunc(fp, cur) != 1)
        {
            printf("写数据出错\n");
            fclose(fp);
            return -1;
        }
        count++;
        cur = getNextFunc(cur);
    }
    fclose(fp);
    printf("存了 %d 条到 %s\n", count, filename);
    return count;
}

// 从文件加载链表
static void* File_LoadList(const char* filename,
    void* (*readDataFunc)(FILE*),
    void* (*createNodeFunc)(void*))
{
    if (!filename || !readDataFunc || !createNodeFunc)
    {
        printf("参数不对\n");
        return NULL;
    }
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        // 文件不存在，正常情况
        return NULL;
    }
    // 创建头节点
    list_node_t* head = (list_node_t*)malloc(sizeof(list_node_t));
    if (!head)
    {
        printf("内存不够\n");
        fclose(fp);
        return NULL;
    }
    // 手动初始化为空双向循环链表
    head->prev = head;
    head->next = head;
    int count = 0;
    while (!feof(fp))
    {
        // 尝试读一个数据
        void* data = readDataFunc(fp);
        if (!data)
        {
            if (feof(fp)) break;
            printf("读文件失败\n");
            // 清理内存
            list_node_t* cur = head->next;
            while (cur != head)
            {
                list_node_t* next = cur->next;
                free(cur);
                cur = next;
            }
            free(head);
            fclose(fp);
            return NULL;
        }
        // 创建新节点
        void* newNode = createNodeFunc(data);
        if (!newNode)
        {
            free(data);
            printf("创建节点失败\n");
            // 清理内存
            list_node_t* cur = head->next;
            while (cur != head)
            {
                list_node_t* next = cur->next;
                free(cur);
                cur = next;
            }
            free(head);
            fclose(fp);
            return NULL;
        }
        // 手动添加到链表尾部
        list_node_t* node = (list_node_t*)newNode;
        list_node_t* tail = head->prev;
        tail->next = node;
        node->prev = tail;
        node->next = head;
        head->prev = node;
        count++;
        free(data);
    }
    fclose(fp);
    return head;
}

// 安全写入文件
static int File_WriteSafe(const char* filename, void* data, size_t dataSize, int numItems)
{
    if (!filename || !data || dataSize == 0)
    {
        return 0;
    }
    char tempFile[256];
    sprintf(tempFile, "%s.tmp", filename);
    FILE* fp = fopen(tempFile, "wb");
    if (!fp)
    {
        printf("创建临时文件失败\n");
        return 0;
    }
    size_t written = fwrite(data, dataSize, numItems, fp);
    fclose(fp);
    if (written != numItems)
    {
        printf("写入不完整\n");
        remove(tempFile);
        return 0;
    }
    if (rename(tempFile, filename) != 0)
    {
        printf("重命名失败\n");
        remove(tempFile);
        return 0;
    }
    return 1;
}

// 安全读取文件
static int File_ReadSafe(const char* filename, void* data, size_t dataSize, int maxNumItems)
{
    if (!filename || !data)
    {
        return -1;
    }
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        return 0; // 文件不存在正常
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if (size % dataSize != 0)
    {
        printf("文件大小不对\n");
        fclose(fp);
        return -1;
    }
    int num = size / dataSize;
    if (num > maxNumItems)
    {
        printf("数据太多，缓冲区不够\n");
        fclose(fp);
        return -1;
    }
    size_t read = fread(data, dataSize, num, fp);
    fclose(fp);
    if (read != num)
    {
        printf("读取不完整\n");
        return -1;
    }
    return num;
}

#endif // UTILS_H