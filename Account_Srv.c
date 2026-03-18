#include "Account_Srv.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

char* filename = "data/account.dat";
account_list_t account_list = NULL;

//获取用户类型字符串形式
char* getAccountTypeString(account_type_t type) {
    switch (type) {
    case USR_ANOMY: return "匿名用户";
    case USR_CLERK: return "销售员";
    case USR_MANG: return "经理";
    case USR_ADMIN: return "管理员";
    default: return "未知";
    }
}

// 从文件中加载所有用户到链表
int loadAllFromFile() {
    FILE* fp = fopen(filename, "rb");
    if (fp == NULL) {
        return 0;
    }

    if (account_list != NULL) {
        List_Destroy(account_list, account_list_node_t);
    }

    List_Init(account_list, account_list_node_t);
    if (account_list == NULL) {
        fclose(fp);
        return 0;
    }

    account_t node;
    int count = 0;
    while (fread(&node, sizeof(account_t), 1, fp) == 1) {
        account_list_t new_node = (account_list_t)malloc(sizeof(account_list_node_t));
        if (new_node == NULL) {
            break;
        }
        new_node->data = node;
        List_AddTail(account_list, new_node);
        count++;
    }

    fclose(fp);
    return count;
}

// 保存所有账户数据到文件
int saveAllToFile() {
    if (List_IsEmpty(account_list)) {
        return 0;
    }

    FILE* fp = fopen(filename, "wb");
    if (fp == NULL) {
        return 0;
    }

    int count = 0;
    account_list_t cur = NULL;
    List_ForEach(account_list, cur) {
        if (fwrite(&(cur->data), sizeof(account_t), 1, fp) == 1) {
            count++;
        }
        else {
            break;
        }
    }

    fclose(fp);
    return count;
}

// 初始化系统
void Account_Srv_InitSys() {
    FILE* fp = fopen(filename, "rb");
    bool adminExists = false;

    if (fp != NULL) {
        account_t temp;
        while (fread(&temp, sizeof(account_t), 1, fp) == 1) {
            if (strcmp(temp.username, "admin") == 0) {
                adminExists = true;
                break;
            }
        }
        fclose(fp);
    }

    if (!adminExists) {
        fp = fopen(filename, "ab");
        if (fp != NULL) {
            account_t admin = { 1, USR_ADMIN, "admin", "123456" };
            fwrite(&admin, sizeof(account_t), 1, fp);
            fclose(fp);
        }
    }

    loadAllFromFile();
}

// 验证用户登录
int Account_Srv_Vertify(char* usrName, char* pwd) {
    if (List_IsEmpty(account_list) || usrName == NULL || pwd == NULL) {
        return 0;
    }

    account_list_t cur = NULL;
    List_ForEach(account_list, cur) {
        if (strcmp(cur->data.username, usrName) == 0 &&
            strcmp(cur->data.password, pwd) == 0) {
            return 1;
        }
    }

    return 0;
}

// 获取下一个可用的ID
int getNextId() {
    if (List_IsEmpty(account_list)) {
        return 1;
    }

    int maxId = 0;
    account_list_t cur = NULL;
    List_ForEach(account_list, cur) {
        if (cur->data.id > maxId) {
            maxId = cur->data.id;
        }
    }

    return maxId + 1;
}

// 添加用户
int Account_Srv_Add(const account_t* data) {
    if (data == NULL) {
        return 0;
    }

    account_list_t new_node = (account_list_t)malloc(sizeof(account_list_node_t));
    if (new_node == NULL) {
        return 0;
    }

    new_node->data = *data;
    List_AddTail(account_list, new_node);

    FILE* fp = fopen(filename, "ab");
    if (fp == NULL) {
        List_DelNode(new_node);
        free(new_node);
        return 0;
    }

    int result = fwrite(data, sizeof(account_t), 1, fp);
    fclose(fp);

    if (result != 1) {
        List_DelNode(new_node);
        free(new_node);
        return 0;
    }

    return 1;
}

// 按姓名查找用户，支持模糊查询
account_list_t Account_Srv_FindByUsrName(char* usrName) {
    if (List_IsEmpty(account_list) || usrName == NULL) {
        return NULL;
    }

    // 创建结果链表
    account_list_t result_list = NULL;
    List_Init(result_list, account_list_node_t);
    if (result_list == NULL) {
        return NULL;
    }

    int count = 0;
    account_list_t cur = NULL;

    // 遍历链表
    List_ForEach(account_list, cur) {
        // 使用strstr进行模糊匹配
        if (strstr(cur->data.username, usrName) != NULL) {
            // 创建新节点副本
            account_list_t new_node = (account_list_t)malloc(sizeof(account_list_node_t));
            if (new_node != NULL) {
                new_node->data = cur->data;
                List_AddTail(result_list, new_node);
                count++;
            }
        }
    }

    if (count == 0) {
        List_Destroy(result_list, account_list_node_t);
        return NULL;
    }

    return result_list;
}

// 按id查找用户
account_t* Account_Srv_FindById(int id) {
    if (List_IsEmpty(account_list)) {
        return NULL;
    }

    account_list_t cur = NULL;
    List_ForEach(account_list, cur) {
        if (cur->data.id == id) {
            return &(cur->data);
        }
    }

    return NULL;
}

// 按id删除用户
int Account_Srv_Del(int id) {
    if (List_IsEmpty(account_list)) {
        return 0;
    }

    account_list_t cur = NULL;
    List_ForEach(account_list, cur) {
        if (cur->data.id == id) {
            if (strcmp(cur->data.username, "admin") == 0) {
                return 0;
            }

            List_FreeNode(cur);
            saveAllToFile();
            return 1;
        }
    }

    return 0;
}

// 修改用户
int Account_Srv_Modify(const account_t* data) {
    if (List_IsEmpty(account_list) || data == NULL) {
        return 0;
    }

    account_list_t cur = NULL;
    List_ForEach(account_list, cur) {
        if (cur->data.id == data->id) {
            if (strcmp(cur->data.username, "admin") == 0) {
                cur->data.type = data->type;
            }
            else {
                cur->data = *data;
            }

            saveAllToFile();
            return 1;
        }
    }

    return 0;
}