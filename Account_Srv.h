#pragma once
#ifndef ACCOUNT_SRV_H
#define ACCOUNT_SRV_H

#include "Common.h"

extern account_list_t account_list;
// 从文件中加载所有用户到链表
int loadAllFromFile();

// 保存所有账户数据到文件
int saveAllToFile();

//初始化系统(创建匿名Admin系统用户)
void Account_Srv_InitSys();

//验证用户(用户名+密码)
int Account_Srv_Vertify(char* usrName, char* pwd);

//按姓名查找用户，可以重名所以是列表
account_list_t Account_Srv_FindByUsrName(char* usrName);

//按id查找用户
account_t* Account_Srv_FindById(int id);

// 保存新账户数据到文件
int Account_Srv_Add(const account_t* data);


//为新用户获取最新的id
int getNextId();

//获取用户类型字符串形式
char* getAccountTypeString(account_type_t type);

//按id删除用户
int Account_Srv_Del(int id);

// 修改用户
int Account_Srv_Modify(const account_t* data);

#endif

