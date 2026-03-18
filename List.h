//cyq写
// List.h
// 这文件是链表和分页操作的，按教材写的
#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdlib.h>

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

// 辅助宏：从数据节点获取list_node_t指针
#define LIST_NODE(ptr) ((list_node_t*)(ptr))

// 辅助宏：从list_node_t指针获取数据节点指针
#define LIST_DATA(ptr) ((void*)(ptr))

// 下面是链表操作的宏，教材表2.3里的
// 初始化链表
#define List_Init(list, node_t) \
    do { \
        (list) = (node_t*)malloc(sizeof(node_t)); \
        if(list) { \
            LIST_NODE(list)->prev = LIST_NODE(list); \
            LIST_NODE(list)->next = LIST_NODE(list); \
        } \
    } while(0)

// 释放数据节点
#define List_Free(list, node_t) \
    do { \
        if(list) { \
            list_node_t* cur_node = LIST_NODE(list)->next; \
            while(cur_node != LIST_NODE(list)) { \
                list_node_t* next_node = cur_node->next; \
                void* data_node = LIST_DATA(cur_node); \
                free(data_node); \
                cur_node = next_node; \
            } \
            LIST_NODE(list)->prev = LIST_NODE(list); \
            LIST_NODE(list)->next = LIST_NODE(list); \
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
            LIST_NODE(new_node)->next = LIST_NODE(list)->next; \
            LIST_NODE(new_node)->prev = LIST_NODE(list); \
            LIST_NODE(list)->next->prev = LIST_NODE(new_node); \
            LIST_NODE(list)->next = LIST_NODE(new_node); \
        } \
    } while(0)

// 尾插
#define List_AddTail(list, new_node) \
    do { \
        if((list) && (new_node)) { \
            LIST_NODE(new_node)->prev = LIST_NODE(list)->prev; \
            LIST_NODE(new_node)->next = LIST_NODE(list); \
            LIST_NODE(list)->prev->next = LIST_NODE(new_node); \
            LIST_NODE(list)->prev = LIST_NODE(new_node); \
        } \
    } while(0)

// 往前插
#define List_InsertBefore(node, new_node) \
    do { \
        if((node) && (new_node)) { \
            LIST_NODE(new_node)->prev = LIST_NODE(node)->prev; \
            LIST_NODE(new_node)->next = LIST_NODE(node); \
            LIST_NODE(node)->prev->next = LIST_NODE(new_node); \
            LIST_NODE(node)->prev = LIST_NODE(new_node); \
        } \
    } while(0)

// 往后插
#define List_InsertAfter(node, new_node) \
    do { \
        if((node) && (new_node)) { \
            LIST_NODE(new_node)->next = LIST_NODE(node)->next; \
            LIST_NODE(new_node)->prev = LIST_NODE(node); \
            LIST_NODE(node)->next->prev = LIST_NODE(new_node); \
            LIST_NODE(node)->next = LIST_NODE(new_node); \
        } \
    } while(0)

// 判断链表是不是空的
#define List_IsEmpty(list) \
    ((list) == NULL ? true : (LIST_NODE(list)->next == LIST_NODE(list) && \
                              LIST_NODE(list)->prev == LIST_NODE(list)))

// 从链表中移除节点（不删内存）
#define List_DelNode(node) \
    do { \
        if(node) { \
            LIST_NODE(node)->prev->next = LIST_NODE(node)->next; \
            LIST_NODE(node)->next->prev = LIST_NODE(node)->prev; \
            LIST_NODE(node)->next = LIST_NODE(node); \
            LIST_NODE(node)->prev = LIST_NODE(node); \
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
    for((cur) = (void*)LIST_NODE(list)->next; \
        (cur) != (void*)LIST_NODE(list); \
        (cur) = (void*)LIST_NODE(cur)->next)

// 下面是分页的宏，教材表2.4里的
// 定位到指定偏移
#define List_Paging(list, pg, node_t) \
    do { \
        int _cnt = 0; \
        list_node_t* _p = LIST_NODE(list)->next; \
        while(_cnt < (pg).offset && _p != LIST_NODE(list)) { \
            _cnt++; \
            _p = _p->next; \
        } \
        (pg).curPos = (void*)_p; \
    } while(0)

// 到第一页
#define Paging_Locate_FirstPage(list, pg) \
    do { \
        (pg).offset = 0; \
        (pg).curPos = (void*)(LIST_NODE(list)->next); \
    } while(0)

// 到最后一页
#define Paging_Locate_LastPage(list, pg, node_t) \
    do { \
        int _tmp = (pg).totalRecords % (pg).pageSize; \
        if(_tmp == 0 && (pg).totalRecords > 0) { \
            _tmp = (pg).pageSize; \
        } \
        (pg).offset = (pg).totalRecords - _tmp; \
        list_node_t* _pos = LIST_NODE(list)->prev; \
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
        (i)++, (pos) = (node_t*)LIST_NODE(pos)->next)

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