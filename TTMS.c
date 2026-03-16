//cyq写
// TTMS.c
// 程序主入口

#include "Account_UI.h"
#include "Main_Menu.h"

int main(void)
{
    int running = 1;  // 控制主循环
    int choice = 0;
    // 登录部分
    while (1)
    {
        int login_ok = SysLogin();  // 教材要求的函数名
        if (login_ok == 1)
        {
            printf("登录成功，欢迎使用！\n");
            break;
        }
        else
        {
            printf("登录失败，请重试\n");
        }
    }
    // 主循环
    while (running)
    {
        choice = Main_Menu();
        if (choice == 9)  // 注销
        {
            running = 0;
            printf("正在注销...\n");
        }
    }
    printf("程序结束，再见！\n");
    return 0;
}