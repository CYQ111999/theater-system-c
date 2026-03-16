//cyq写
// 主菜单

#include <stdio.h>
#include "Common.h"
#include "Studio_UI.h"
#include "Play_UI.h"
#include "Schedule_UI.h"
#include "Ticket.h"
#include "Sale_UI.h"
#include "Account_UI.h"
#include "MaiAccount_UI.h"
#include "Queries_Menu.h"
#include "Seat_UI.h"
#include "StaSales_UI.h"
#include "Salesanalysis_UI.h"

// 内部用的函数
static void show_menu(void);
static int process_choice(int choice);

// 主菜单入口
int Main_Menu(void)
{
    int choice = 0;
    int ok = 0;  // 输入是否有效的标志
    while (!ok)
    {
        show_menu();  // 先显示菜单
        if (scanf("%d", &choice) == 1)
        {
            getchar();  // 清掉回车
            // 检查是不是1-9
            if (choice >= 1 && choice <= 9)
            {
                ok = 1;
                // 9是注销，直接返回
                if (choice == 9)
                {
                    return 9;
                }
                // 处理其他选择
                process_choice(choice);
            }
            else
            {
                printf("请输入1-9的数字\n");
            }
        }
        else
        {
            printf("输入格式不对\n");
            while (getchar() != '\n');  // 清掉错误的输入
        }
    }
    return choice;
}

// 显示菜单
static void show_menu(void)
{
    printf("\n====== 剧院票务管理系统 ======\n");
    // 显示当前用户
    const char* role = "";
    if (gl_CurUser.type == USR_ADMIN)
        role = "管理员";
    else if (gl_CurUser.type == USR_MANG)
        role = "经理";
    else if (gl_CurUser.type == USR_CLERK)
        role = "售票员";
    printf("用户：[%s] %s\n", role, gl_CurUser.username);
    printf("-----------------------------\n");
    // 根据角色显示不同菜单
    switch (gl_CurUser.type)
    {
    case USR_ADMIN:  // 管理员
        printf("  1. 管理系统用户\n");
        printf("  2. 管理演出厅\n");
        printf("  3. 设置座位\n");
        break;
    case USR_MANG:   // 经理
        printf("  1. 管理剧目\n");
        printf("  2. 安排演出\n");
        printf("  3. 查询演出\n");
        printf("  4. 统计销售额\n");
        printf("  5. 统计票房\n");
        break;
    case USR_CLERK:  // 售票员
        printf("  1. 查询演出\n");
        printf("  2. 售票\n");
        printf("  3. 退票\n");
        printf("  4. 统计销售额\n");
        break;
    default:
        printf("  错误：未知用户类型\n");
    }
    // 公共选项
    printf("  8. 系统信息\n");
    printf("  9. 注销登录\n");
    printf("-----------------------------\n");
    printf("请选择 (1-9): ");
}

// 处理用户的选择
static int process_choice(int choice)
{
    // 系统信息（所有角色都一样）
    if (choice == 8)
    {
        printf("\n系统信息：TTMS v1.0 - 五人小组项目\n");
        printf("按回车继续...");
        getchar();
        return 1;
    }
    // 根据角色处理
    switch (gl_CurUser.type)
    {
    case USR_ADMIN:  // 管理员
        switch (choice)
        {
        case 1: Account_UI_MgtEntry(); break;  // 管理用户
        case 2: Studio_UI_MainMenu(); break;   // 管理演出厅
        case 3: Seat_UI_MainMenu(); break;     // 设置座位
        default: printf("无效选择\n");
        }
        break;

    case USR_MANG:   // 经理
        switch (choice)
        {
        case 1: Play_UI_MainMenu(); break;         // 管理剧目
        case 2: Schedule_UI_Add(); break;          // 安排演出
        case 3: Queries_Menu_Schedule(); break;    // 查询演出
        case 4: StaSales_UI_Analysis(); break;     // 统计销售额
        case 5: Salesanalysis_UI_Report(); break;  // 统计票房
        default: printf("无效选择\n");
        }
        break;

    case USR_CLERK:  // 售票员
        switch (choice)
        {
        case 1: Queries_Menu_Schedule(); break;  // 查询演出
        case 2: Ticket_UI_Sell(); break;         // 售票
        case 3: Ticket_UI_Refund(); break;       // 退票
        case 4: Sale_UI_Analysis(); break;       // 统计销售额
        default: printf("无效选择\n");
        }
        break;

    default:
        printf("用户类型错误\n");
    }
    return 1;
}