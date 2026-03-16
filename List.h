//cyq写
// List.h
// 这文件是链表和分页操作的，按教材写的
#ifndef LIST_H
#define LIST_H

#include <stdbool.h>

// 链表节点结构，按教材要求必须放在每个业务节点开头
typedef struct list_node {
    struct list_node* prev;
    struct list_node* next;
} list_node_t;

// 分页器
typedef struct {
    int totalRecords;   // 总共多少条
    int offset;         // 当前页从第几条开始
    int pageSize;       // 一页几条
    void* curPos;       // 当前页第一个节点在哪
} paging_t;

// 下面是链表操作的宏，教材表2.3里的
// 初始化链表
#define List_Init(list, node_t) \
    do { \
        (list) = (node_t*)malloc(sizeof(node_t)); \
        if(list) { \
            (list)->prev = (list); \
            (list)->next = (list); \
        } \
    } while(0)

// 释放数据节点
#define List_Free(list, node_t) \
    do { \
        if(list) { \
            node_t* cur = (list)->next; \
            while(cur != (list)) { \
                node_t* next_node = cur->next; \
                free(cur); \
                cur = next_node; \
            } \
            (list)->prev = (list); \
            (list)->next = (list); \
        } \
    } while(0)

// 销毁整个链表
#define List_Destroy(list, node_t) \
    do { \
        if(list) { \
            List_Free(list, node_t); \
            free(list); \
            (list) = NULL; \
        } \
    } while(0)

// 头插
#define List_AddHead(list, new_node) \
    do { \
        if((list) && (new_node)) { \
            ((list_node_t*)new_node)->next = (list)->next; \
            ((list_node_t*)new_node)->prev = (list); \
            (list)->next->prev = (list_node_t*)new_node; \
            (list)->next = (list_node_t*)new_node; \
        } \
    } while(0)

// 尾插
#define List_AddTail(list, new_node) \
    do { \
        if((list) && (new_node)) { \
            ((list_node_t*)new_node)->prev = (list)->prev; \
            ((list_node_t*)new_node)->next = (list); \
            (list)->prev->next = (list_node_t*)new_node; \
            (list)->prev = (list_node_t*)new_node; \
        } \
    } while(0)

// 往前插
#define List_InsertBefore(node, new_node) \
    do { \
        if((node) && (new_node)) { \
            ((list_node_t*)new_node)->prev = ((list_node_t*)node)->prev; \
            ((list_node_t*)new_node)->next = (list_node_t*)node; \
            ((list_node_t*)node)->prev->next = (list_node_t*)new_node; \
            ((list_node_t*)node)->prev = (list_node_t*)new_node; \
        } \
    } while(0)

// 往后插
#define List_InsertAfter(node, new_node) \
    do { \
        if((node) && (new_node)) { \
            ((list_node_t*)new_node)->next = ((list_node_t*)node)->next; \
            ((list_node_t*)new_node)->prev = (list_node_t*)node; \
            ((list_node_t*)node)->next->prev = (list_node_t*)new_node; \
            ((list_node_t*)node)->next = (list_node_t*)new_node; \
        } \
    } while(0)

// 判断链表是不是空的
#define List_IsEmpty(list) \
    ((list) == NULL ? true : ((list)->next == (list) && (list)->prev == (list)))

// 从链表中移除节点（不删内存）
#define List_DelNode(node) \
    do { \
        if(node) { \
            ((list_node_t*)node)->prev->next = ((list_node_t*)node)->next; \
            ((list_node_t*)node)->next->prev = ((list_node_t*)node)->prev; \
            ((list_node_t*)node)->next = (list_node_t*)node; \
            ((list_node_t*)node)->prev = (list_node_t*)node; \
        } \
    } while(0)

// 移除并删除节点
#define List_FreeNode(node) \
    do { \
        if(node) { \
            List_DelNode(node); \
            free(node); \
        } \
    } while(0)

// 遍历链表
#define List_ForEach(list, cur) \
    for((cur) = (list)->next; (cur) != (list); (cur) = (cur)->next)

// 下面是分页的宏，教材表2.4里的
// 定位到指定偏移
#define List_Paging(list, pg, node_t) \
    do { \
        int _cnt = 0; \
        node_t* _p = (list)->next; \
        while(_cnt < (pg).offset && _p != (list)) { \
            _cnt++; \
            _p = _p->next; \
        } \
        (pg).curPos = (void*)_p; \
    } while(0)

// 到第一页
#define Paging_Locate_FirstPage(list, pg) \
    do { \
        (pg).offset = 0; \
        (pg).curPos = (void*)((list)->next); \
    } while(0)

// 到最后一页
#define Paging_Locate_LastPage(list, pg, node_t) \
    do { \
        int _tmp = (pg).totalRecords % (pg).pageSize; \
        if(_tmp == 0 && (pg).totalRecords > 0) { \
            _tmp = (pg).pageSize; \
        } \
        (pg).offset = (pg).totalRecords - _tmp; \
        node_t* _pos = (list)->prev; \
        for(; _tmp > 1; _tmp--) { \
            _pos = _pos->prev; \
        } \
        (pg).curPos = (void*)_pos; \
    } while(0)

// 往前或往后翻页
#define Paging_Locate_OffsetPage(list, pg, offset_page, node_t) \
    do { \
        int _new_offset = (pg).offset + (offset_page) * (pg).pageSize; \
        if(_new_offset < 0) { \
            _new_offset = 0; \
        } else if(_new_offset >= (pg).totalRecords) { \
            _new_offset = (pg).totalRecords - 1; \
        } \
        (pg).offset = _new_offset; \
        List_Paging(list, pg, node_t); \
    } while(0)

// 遍历当前页
#define Paging_ViewPage_ForEach(list, pg, node_t, pos, i) \
    for((i) = 0, (pos) = (node_t*)(pg).curPos; \
        (pos) != (list) && (i) < (pg).pageSize; \
        (i)++, (pos) = (pos)->next)

// 算当前是第几页
#define Pageing_CurPage(pg) \
    ((pg).totalRecords == 0 ? 0 : 1 + (pg).offset / (pg).pageSize)

// 算总页数
#define Pageing_TotalPages(pg) \
    ((pg).totalRecords == 0 ? 0 : ((pg).totalRecords + (pg).pageSize - 1) / (pg).pageSize)

// 是不是第一页
#define Pageing_IsFirstPage(pg) \
    ((pg).offset == 0)

// 是不是最后一页
#define Pageing_IsLastPage(pg) \
    ((pg).offset + (pg).pageSize >= (pg).totalRecords)

#endif