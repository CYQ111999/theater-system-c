#include "Account_Srv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>


// 清屏函数
#ifdef _WIN32
#define clear_screen() system("cls")
#else
#define clear_screen() system("clear")
#endif

// 等待用户按键
void press_any_key() {
	printf("\n按任意键继续...");
	getchar();
}

// 显示当前页的用户信息
void display_current_page(paging_t* pg) {
	if (!account_list || pg->totalRecords == 0) {
		printf("\n当前没有用户数据！\n");
		return;
	}

	printf("\n═══════════════════════════════════════════════════════\n");
	printf("                      用户列表\n");
	printf("═══════════════════════════════════════════════════════\n");

	int i = 0;
	account_list_t pos = NULL;

	printf("序号  ID   用户名       密码       用户类型\n");
	printf("----  ----  ----------  ----------  --------------\n");

	Paging_ViewPage_ForEach(account_list, *pg, account_list_node_t, pos, i) {
		if (pos) {
			char* type_str = "未知";
			switch (pos->data.type) {
			case USR_ANOMY: type_str = "匿名用户"; break;
			case USR_CLERK: type_str = "销售员"; break;
			case USR_MANG:  type_str = "经理"; break;
			case USR_ADMIN: type_str = "管理员"; break;
			}

			printf("%-4d  %-4d  %-10s  %-10s  %s\n",
				i + 1,
				pos->data.id,
				pos->data.username,
				pos->data.password,
				type_str);
		}
	}

	printf("═══════════════════════════════════════════════════════\n");
}

// 显示分页信息
void display_page_info(paging_t* pg) {
	int current_page = Pageing_CurPage(*pg);
	int total_pages = Pageing_TotalPages(*pg);

	printf("当前第 %d/%d 页 | 总记录数: %d | 每页显示: %d 条\n",
		current_page, total_pages, pg->totalRecords, pg->pageSize);
}

// 重新计算总记录数的辅助函数
void recalc_total_records(paging_t* pg) {
	if (!account_list) {
		pg->totalRecords = 0;
		return;
	}

	int count = 0;
	account_list_t cur = NULL;
	List_ForEach(account_list, cur) {
		count++;
	}
	pg->totalRecords = count;
}

// 新增用户功能
void Account_UI_Add(paging_t* pg) {
	clear_screen();
	printf("═══════════════════════════════════════════════════════\n");
	printf("                      新增用户\n");
	printf("═══════════════════════════════════════════════════════\n");

	// 获取下一个可用的ID
	int next_id = getNextId();
	printf("新用户ID将自动分配为: %d\n", next_id);
	printf("═══════════════════════════════════════════════════════\n");

	account_t new_user;
	new_user.id = next_id;

	// 输入用户名
	int valid_username = 0;
	while (!valid_username) {
		printf("请输入用户名 (3-20个字符, 不能包含空格): ");
		fgets(new_user.username, 30, stdin);
		new_user.username[strcspn(new_user.username, "\n")] = '\0';  // 移除换行符

		// 验证用户名
		if (strlen(new_user.username) < 3) {
			printf("错误：用户名太短，至少需要3个字符！\n");
		}
		else if (strlen(new_user.username) > 20) {
			printf("错误：用户名太长，最多20个字符！\n");
		}
		else if (strchr(new_user.username, ' ') != NULL) {
			printf("错误：用户名不能包含空格！\n");
		}
		else {
			valid_username = 1;
		}
	}

	// 输入密码
	int valid_password = 0;
	while (!valid_password) {
		printf("请输入密码 (6-20个字符): ");
		fgets(new_user.password, 30, stdin);
		new_user.password[strcspn(new_user.password, "\n")] = '\0';  // 移除换行符

		if (strlen(new_user.password) < 6) {
			printf("错误：密码太短，至少需要6个字符！\n");
		}
		else if (strlen(new_user.password) > 20) {
			printf("错误：密码太长，最多20个字符！\n");
		}
		else {
			valid_password = 1;
		}
	}

	// 输入用户类型
	int valid_type = 0;
	while (!valid_type) {
		printf("\n请选择用户类型：\n");
		printf("  1. 销售员 (USR_CLERK)\n");
		printf("  2. 经理 (USR_MANG)\n");
		printf("  3. 管理员 (USR_ADMIN)\n");
		printf("  4. 匿名用户 (USR_ANOMY)\n");
		printf("请输入类型编号 (1-4): ");

		int type_choice;
		if (scanf("%d", &type_choice) != 1) {
			printf("错误：请输入有效的数字！\n");
			while (getchar() != '\n');  // 清空输入缓冲区
			continue;
		}
		getchar();  // 清空换行符

		switch (type_choice) {
		case 1:
			new_user.type = USR_CLERK;
			valid_type = 1;
			break;
		case 2:
			new_user.type = USR_MANG;
			valid_type = 1;
			break;
		case 3:
			new_user.type = USR_ADMIN;
			valid_type = 1;
			break;
		case 4:
			new_user.type = USR_ANOMY;
			valid_type = 1;
			break;
		default:
			printf("错误：无效的类型编号，请重新选择！\n");
			break;
		}
	}

	// 确认信息
	printf("\n═══════════════════════════════════════════════════════\n");
	printf("请确认用户信息：\n");
	printf("  ID: %d\n", new_user.id);
	printf("  用户名: %s\n", new_user.username);
	printf("  密码: %s\n", new_user.password);
	printf("  用户类型: %d", new_user.type);

	switch (new_user.type) {
	case USR_ANOMY: printf(" (匿名用户)\n"); break;
	case USR_CLERK: printf(" (销售员)\n"); break;
	case USR_MANG:  printf(" (经理)\n"); break;
	case USR_ADMIN: printf(" (管理员)\n"); break;
	}

	printf("═══════════════════════════════════════════════════════\n");
	printf("确认添加此用户吗？(y/n): ");

	char confirm;
	scanf("%c", &confirm);
	getchar();  // 清空换行符

	if (confirm == 'y' || confirm == 'Y') {
		// 调用数据层函数添加用户
		int result = Account_Srv_Add(&new_user);

		if (result) {
			printf("\n用户添加成功！\n");

			// 直接更新分页器的总记录数（因为链表已经更新了）
			pg->totalRecords++;

			// 计算新用户所在的页码
			int total_pages = Pageing_TotalPages(*pg);

			// 定位到最后一页
			Paging_Locate_LastPage(account_list, *pg, account_list_node_t);

			printf("已自动跳转到最后一页查看新增结果。\n");
			printf("新用户位于第 %d/%d 页\n", total_pages, total_pages);
		}
		else {
			printf("\n用户添加失败！可能是用户名已存在或其他错误。\n");
		}
	}
	else {
		printf("\n已取消添加用户。\n");
	}

	press_any_key();
}

// 修改用户功能 - 使用改造后的模糊查询函数
void Account_UI_Modify(paging_t* pg) {
	clear_screen();
	printf("═══════════════════════════════════════════════════════\n");
	printf("                      修改用户\n");
	printf("═══════════════════════════════════════════════════════\n");

	if (List_IsEmpty(account_list)) {
		printf("当前没有用户数据！\n");
		press_any_key();
		return;
	}

	// 1. 按用户名模糊查找
	char search_keyword[30];
	printf("请输入要查找的用户名（支持模糊搜索）: ");
	fgets(search_keyword, 30, stdin);
	search_keyword[strcspn(search_keyword, "\n")] = '\0';

	if (strlen(search_keyword) == 0) {
		printf("搜索关键字不能为空！\n");
		press_any_key();
		return;
	}

	// 使用改造后的模糊查询函数
	account_list_t search_results = Account_Srv_FindByUsrName(search_keyword);

	if (search_results == NULL) {
		printf("没有找到包含 '%s' 的用户\n", search_keyword);
		press_any_key();
		return;
	}

	// 显示搜索结果
	printf("\n═══════════════════════════════════════════════════════\n");
	printf("                   搜索结果\n");
	printf("═══════════════════════════════════════════════════════\n");

	printf("序号  ID   用户名       用户类型\n");
	printf("----  ----  ----------  --------------\n");

	int result_count = 0;
	account_list_t pos = NULL;
	List_ForEach(search_results, pos) {
		if (pos) {
			result_count++;
			char* type_str = "未知";
			switch (pos->data.type) {
			case USR_ANOMY: type_str = "匿名用户"; break;
			case USR_CLERK: type_str = "销售员"; break;
			case USR_MANG:  type_str = "经理"; break;
			case USR_ADMIN: type_str = "管理员"; break;
			}

			printf("%-4d  %-4d  %-10s  %s\n",
				result_count,
				pos->data.id,
				pos->data.username,
				type_str);
		}
	}

	printf("═══════════════════════════════════════════════════════\n");
	printf("共找到 %d 个匹配的用户\n", result_count);

	// 2. 让用户指定要修改的用户ID
	int target_id;
	printf("\n请输入要修改的用户ID: ");
	scanf("%d", &target_id);
	getchar();  // 清空换行符

	// 检查输入的用户ID是否在搜索结果中
	int id_found_in_results = 0;
	account_list_t cur_result = NULL;
	List_ForEach(search_results, cur_result) {
		if (cur_result->data.id == target_id) {
			id_found_in_results = 1;
			break;
		}
	}

	if (!id_found_in_results) {
		printf("错误：ID为 %d 的用户不在搜索结果中！\n", target_id);
		List_Destroy(search_results, account_list_node_t);
		press_any_key();
		return;
	}

	// 查找目标用户
	account_t* target_user = Account_Srv_FindById(target_id);

	if (target_user == NULL) {
		printf("未找到ID为 %d 的用户\n", target_id);
		List_Destroy(search_results, account_list_node_t);
		press_any_key();
		return;
	}

	// 3. 显示当前信息并输入修改内容
	printf("\n═══════════════════════════════════════════════════════\n");
	printf("              用户 %d 的当前信息\n", target_user->id);
	printf("═══════════════════════════════════════════════════════\n");

	printf("当前信息：\n");
	printf("  用户名: %s\n", target_user->username);
	printf("  密码: %s\n", target_user->password);
	printf("  用户类型: %d", target_user->type);

	switch (target_user->type) {
	case USR_ANOMY: printf(" (匿名用户)\n"); break;
	case USR_CLERK: printf(" (销售员)\n"); break;
	case USR_MANG:  printf(" (经理)\n"); break;
	case USR_ADMIN: printf(" (管理员)\n"); break;
	}

	printf("\n═══════════════════════════════════════════════════════\n");
	printf("请输入修改后的信息（留空则保持原值）：\n");

	// 准备修改后的用户数据
	account_t modified_user = *target_user;

	// 输入新用户名
	char new_username[30];
	printf("新用户名（留空保持原值 '%s'）: ", target_user->username);
	fgets(new_username, 30, stdin);
	new_username[strcspn(new_username, "\n")] = '\0';

	if (strlen(new_username) > 0) {
		// 检查用户名是否与现有用户重复（除了自己）
		int username_exists = 0;
		account_list_t cur = NULL;
		List_ForEach(account_list, cur) {
			if (cur->data.id != target_user->id && strcmp(cur->data.username, new_username) == 0) {
				username_exists = 1;
				break;
			}
		}

		if (username_exists) {
			printf("错误：用户名 '%s' 已存在！\n", new_username);
			List_Destroy(search_results, account_list_node_t);
			press_any_key();
			return;
		}

		strcpy(modified_user.username, new_username);
	}

	// 输入新密码
	if (strcmp(target_user->username, "admin") == 0) {
		printf("注意：管理员账户密码不能修改，将保持原值。\n");
	}
	else {
		char new_password[30];
		printf("新密码（留空保持原值 '%s'）: ", target_user->password);
		fgets(new_password, 30, stdin);
		new_password[strcspn(new_password, "\n")] = '\0';

		if (strlen(new_password) > 0) {
			strcpy(modified_user.password, new_password);
		}
	}

	// 输入新用户类型
	int new_type_choice = -1;
	printf("新用户类型（0-保持原值，1-销售员，2-经理，3-管理员，4-匿名）: ");
	char type_input[10];
	fgets(type_input, 10, stdin);
	type_input[strcspn(type_input, "\n")] = '\0';

	if (strlen(type_input) > 0) {
		sscanf(type_input, "%d", &new_type_choice);

		if (new_type_choice != 0) {  // 0表示保持原值
			switch (new_type_choice) {
			case 4: modified_user.type = USR_ANOMY; break;
			case 1: modified_user.type = USR_CLERK; break;
			case 2: modified_user.type = USR_MANG; break;
			case 3: modified_user.type = USR_ADMIN; break;
			default:
				printf("错误：无效的用户类型！\n");
				List_Destroy(search_results, account_list_node_t);
				press_any_key();
				return;
			}
		}
	}

	// 4. 确认修改
	printf("\n═══════════════════════════════════════════════════════\n");
	printf("             请确认修改后的信息\n");
	printf("═══════════════════════════════════════════════════════\n");

	printf("修改后信息：\n");
	printf("  ID: %d\n", modified_user.id);
	printf("  用户名: %s\n", modified_user.username);
	printf("  密码: %s\n", modified_user.password);
	printf("  用户类型: %d", modified_user.type);

	switch (modified_user.type) {
	case USR_ANOMY: printf(" (匿名用户)\n"); break;
	case USR_CLERK: printf(" (销售员)\n"); break;
	case USR_MANG:  printf(" (经理)\n"); break;
	case USR_ADMIN: printf(" (管理员)\n"); break;
	}

	printf("═══════════════════════════════════════════════════════\n");
	printf("确认修改此用户吗？(y/n): ");

	char confirm;
	scanf("%c", &confirm);
	getchar();  // 清空换行符

	if (confirm == 'y' || confirm == 'Y') {
		// 5. 调用数据层函数修改用户
		int result = Account_Srv_Modify(&modified_user);

		if (result) {
			printf("\n用户信息修改成功！\n");

			// 6. 计算用户所在的页码
			int user_page_index = 0;
			int found = 0;
			account_list_t cur = NULL;

			// 查找用户并计算索引
			List_ForEach(account_list, cur) {
				if (cur->data.id == target_user->id) {
					found = 1;
					break;
				}
				user_page_index++;
			}

			if (found) {
				// 计算页码（从1开始）
				int user_page = (user_page_index / pg->pageSize) + 1;

				// 跳转到该用户所在的页面
				int total_pages = Pageing_TotalPages(*pg);
				if (user_page > total_pages) {
					user_page = total_pages;
				}

				pg->offset = (user_page - 1) * pg->pageSize;
				List_Paging(account_list, *pg, account_list_node_t);

				printf("已自动跳转到第 %d 页（该用户所在页面）\n", user_page);
			}
			else {
				printf("无法定位用户，返回当前页面。\n");
			}
		}
		else {
			printf("\n用户信息修改失败！\n");
		}
	}
	else {
		printf("\n已取消修改。\n");
	}

	// 清理搜索结果链表
	List_Destroy(search_results, account_list_node_t);

	press_any_key();
}

// 删除用户功能
void Account_UI_Delete(paging_t* pg) {
	clear_screen();
	printf("═══════════════════════════════════════════════════════\n");
	printf("                      删除用户\n");
	printf("═══════════════════════════════════════════════════════\n");

	if (List_IsEmpty(account_list)) {
		printf("当前没有用户数据！\n");
		press_any_key();
		return;
	}

	// 1. 按用户名模糊查找
	char search_keyword[30];
	printf("请输入要查找的用户名（支持模糊搜索）: ");
	fgets(search_keyword, 30, stdin);
	search_keyword[strcspn(search_keyword, "\n")] = '\0';

	if (strlen(search_keyword) == 0) {
		printf("搜索关键字不能为空！\n");
		press_any_key();
		return;
	}

	// 使用模糊查询函数
	account_list_t search_results = Account_Srv_FindByUsrName(search_keyword);

	if (search_results == NULL) {
		printf("没有找到包含 '%s' 的用户\n", search_keyword);
		press_any_key();
		return;
	}

	// 显示搜索结果
	printf("\n═══════════════════════════════════════════════════════\n");
	printf("                   搜索结果\n");
	printf("═══════════════════════════════════════════════════════\n");

	printf("序号  ID   用户名       用户类型\n");
	printf("----  ----  ----------  --------------\n");

	int result_count = 0;
	account_list_t pos = NULL;
	List_ForEach(search_results, pos) {
		if (pos) {
			result_count++;
			char* type_str = "未知";
			switch (pos->data.type) {
			case USR_ANOMY: type_str = "匿名用户"; break;
			case USR_CLERK: type_str = "销售员"; break;
			case USR_MANG:  type_str = "经理"; break;
			case USR_ADMIN: type_str = "管理员"; break;
			}

			printf("%-4d  %-4d  %-10s  %s\n",
				result_count,
				pos->data.id,
				pos->data.username,
				type_str);
		}
	}

	printf("═══════════════════════════════════════════════════════\n");
	printf("共找到 %d 个匹配的用户\n", result_count);

	// 2. 让用户指定要删除的用户ID
	int target_id;
	printf("\n请输入要删除的用户ID: ");
	scanf("%d", &target_id);
	getchar();  // 清空换行符

	// 检查输入的用户ID是否在搜索结果中
	int id_found_in_results = 0;
	account_list_t cur_result = NULL;
	List_ForEach(search_results, cur_result) {
		if (cur_result->data.id == target_id) {
			id_found_in_results = 1;
			break;
		}
	}

	if (!id_found_in_results) {
		printf("错误：ID为 %d 的用户不在搜索结果中！\n", target_id);
		List_Destroy(search_results, account_list_node_t);
		press_any_key();
		return;
	}

	// 查找目标用户
	account_t* target_user = Account_Srv_FindById(target_id);

	if (target_user == NULL) {
		printf("未找到ID为 %d 的用户\n", target_id);
		List_Destroy(search_results, account_list_node_t);
		press_any_key();
		return;
	}

	// 检查是否为admin用户
	if (strcmp(target_user->username, "admin") == 0) {
		printf("错误：不能删除管理员账户！\n");
		List_Destroy(search_results, account_list_node_t);
		press_any_key();
		return;
	}

	// 显示要删除的用户信息
	printf("\n═══════════════════════════════════════════════════════\n");
	printf("              将要删除的用户信息\n");
	printf("═══════════════════════════════════════════════════════\n");

	printf("用户信息：\n");
	printf("  ID: %d\n", target_user->id);
	printf("  用户名: %s\n", target_user->username);
	printf("  用户类型: %d", target_user->type);

	switch (target_user->type) {
	case USR_ANOMY: printf(" (匿名用户)\n"); break;
	case USR_CLERK: printf(" (销售员)\n"); break;
	case USR_MANG:  printf(" (经理)\n"); break;
	case USR_ADMIN: printf(" (管理员)\n"); break;
	}

	printf("═══════════════════════════════════════════════════════\n");

	// 3. 确认删除
	printf("警告：此操作不可恢复！\n");
	printf("确认删除此用户吗？(y/n): ");

	char confirm;
	scanf("%c", &confirm);
	getchar();  // 清空换行符

	if (confirm == 'y' || confirm == 'Y') {
		// 4. 在删除前先记录用户所在的页码
		int user_page_index = 0;
		int found = 0;
		account_list_t cur = NULL;

		// 查找用户并计算索引
		List_ForEach(account_list, cur) {
			if (cur->data.id == target_user->id) {
				found = 1;
				break;
			}
			user_page_index++;
		}

		int user_page_before_delete = 0;
		if (found) {
			// 计算页码（从1开始）
			user_page_before_delete = (user_page_index / pg->pageSize) + 1;
		}

		// 5. 调用数据层函数删除用户
		int result = Account_Srv_Del(target_id);

		if (result) {
			printf("\n用户删除成功！\n");

			// 6. 重新计算总记录数
			int new_total = 0;
			account_list_t cur2 = NULL;
			List_ForEach(account_list, cur2) {
				new_total++;
			}
			pg->totalRecords = new_total;

			// 7. 计算删除后应该显示的页码
			int target_page = user_page_before_delete;

			// 检查当前页是否还有数据
			if (pg->totalRecords > 0) {
				int total_pages = Pageing_TotalPages(*pg);

				// 如果目标页超过总页数，显示最后一页
				if (target_page > total_pages) {
					target_page = total_pages;
				}

				// 如果目标页是0（用户不存在），则显示第一页
				if (target_page <= 0) {
					target_page = 1;
				}

				// 跳转到目标页
				pg->offset = (target_page - 1) * pg->pageSize;
				List_Paging(account_list, *pg, account_list_node_t);

				printf("已自动跳转到第 %d 页\n", target_page);
			}
			else {
				// 没有数据了，重置到第一页
				pg->offset = 0;
				pg->curPos = NULL;
				printf("所有用户已删除，列表为空。\n");
			}
		}
		else {
			printf("\n用户删除失败！\n");
		}
	}
	else {
		printf("\n已取消删除。\n");
	}

	// 清理搜索结果链表
	List_Destroy(search_results, account_list_node_t);

	press_any_key();
}

// 查找用户功能
void Account_UI_Search(paging_t* pg) {
	clear_screen();
	printf("═══════════════════════════════════════════════════════\n");
	printf("                      查找用户\n");
	printf("═══════════════════════════════════════════════════════\n");

	if (List_IsEmpty(account_list)) {
		printf("当前没有用户数据！\n");
		press_any_key();
		return;
	}

	int search_again = 1;

	do {
		clear_screen();
		printf("═══════════════════════════════════════════════════════\n");
		printf("                      查找用户\n");
		printf("═══════════════════════════════════════════════════════\n");

		// 1. 输入搜索关键字
		char search_keyword[30];
		printf("请输入要查找的用户名（支持模糊搜索）: ");
		fgets(search_keyword, 30, stdin);
		search_keyword[strcspn(search_keyword, "\n")] = '\0';

		if (strlen(search_keyword) == 0) {
			printf("搜索关键字不能为空！\n");
			press_any_key();
			continue;
		}

		// 2. 使用模糊查询函数
		account_list_t search_results = Account_Srv_FindByUsrName(search_keyword);

		if (search_results == NULL) {
			printf("\n没有找到包含 '%s' 的用户\n", search_keyword);

			printf("\n═══════════════════════════════════════════════════════\n");
			printf("查找选项：\n");
			printf("  1. 重新搜索\n");
			printf("  0. 返回主菜单\n");
			printf("═══════════════════════════════════════════════════════\n");
			printf("请选择: ");

			int choice;
			scanf("%d", &choice);
			getchar();

			if (choice == 0) {
				search_again = 0;
			}
			continue;
		}

		// 3. 显示搜索结果
		printf("\n═══════════════════════════════════════════════════════\n");
		printf("                   搜索结果\n");
		printf("═══════════════════════════════════════════════════════\n");

		printf("序号  ID   用户名       密码       用户类型\n");
		printf("----  ----  ----------  ----------  --------------\n");

		int result_count = 0;
		account_list_t pos = NULL;
		List_ForEach(search_results, pos) {
			if (pos) {
				result_count++;
				char* type_str = "未知";
				switch (pos->data.type) {
				case USR_ANOMY: type_str = "匿名用户"; break;
				case USR_CLERK: type_str = "销售员"; break;
				case USR_MANG:  type_str = "经理"; break;
				case USR_ADMIN: type_str = "管理员"; break;
				}

				printf("%-4d  %-4d  %-10s  %-10s  %s\n",
					result_count,
					pos->data.id,
					pos->data.username,
					pos->data.password,
					type_str);
			}
		}

		printf("═══════════════════════════════════════════════════════\n");
		printf("共找到 %d 个包含 '%s' 的用户\n", result_count, search_keyword);

		// 4. 显示详细用户信息选项
		printf("\n═══════════════════════════════════════════════════════\n");
		printf("操作选项：\n");
		printf("  1. 查看用户详细信息\n");
		printf("  2. 重新搜索\n");
		printf("  0. 返回主菜单\n");
		printf("═══════════════════════════════════════════════════════\n");
		printf("请选择: ");

		int choice;
		scanf("%d", &choice);
		getchar();

		switch (choice) {
		case 1:  // 查看用户详细信息
		{
			int target_id;
			printf("请输入要查看的用户ID: ");
			scanf("%d", &target_id);
			getchar();

			// 检查输入的用户ID是否在搜索结果中
			int id_found_in_results = 0;
			account_list_t cur_result = NULL;
			List_ForEach(search_results, cur_result) {
				if (cur_result->data.id == target_id) {
					id_found_in_results = 1;
					break;
				}
			}

			if (!id_found_in_results) {
				printf("错误：ID为 %d 的用户不在搜索结果中！\n", target_id);
				press_any_key();
				break;
			}

			// 查找目标用户
			account_t* target_user = Account_Srv_FindById(target_id);

			if (target_user) {
				printf("\n═══════════════════════════════════════════════════════\n");
				printf("                  用户详细信息\n");
				printf("═══════════════════════════════════════════════════════\n");

				printf("用户ID: %d\n", target_user->id);
				printf("用户名: %s\n", target_user->username);
				printf("密码: %s\n", target_user->password);
				printf("用户类型: %d", target_user->type);

				switch (target_user->type) {
				case USR_ANOMY: printf(" (匿名用户)\n"); break;
				case USR_CLERK: printf(" (销售员)\n"); break;
				case USR_MANG:  printf(" (经理)\n"); break;
				case USR_ADMIN: printf(" (管理员)\n"); break;
				}

				printf("═══════════════════════════════════════════════════════\n");
				press_any_key();
			}
			else {
				printf("未找到ID为 %d 的用户\n", target_id);
				press_any_key();
			}
		}
		break;

		case 2:  // 重新搜索
			search_again = 1;
			break;

		case 0:  // 返回主菜单
			search_again = 0;
			break;

		default:
			printf("无效的选项，将返回主菜单。\n");
			press_any_key();
			search_again = 0;
			break;
		}

		// 清理搜索结果链表
		List_Destroy(search_results, account_list_node_t);

	} while (search_again);
}

// 分页浏览主界面
void Account_UI_FetchAll() {
	// 加载数据
	int count = loadAllFromFile();
	if (count == 0) {
		printf("没有找到用户数据！\n");
		printf("将创建默认管理员账户。\n");
		press_any_key();
	}

	// 初始化分页器
	paging_t pg;
	pg.pageSize = 5;  // 默认每页5条
	pg.totalRecords = 0;
	pg.offset = 0;

	// 计算总记录数
	recalc_total_records(&pg);

	// 定位到第一页
	Paging_Locate_FirstPage(account_list, pg);

	int choice = 0;
	int exit_browse = 0;

	do {
		clear_screen();

		// 显示分页信息和当前页数据
		display_page_info(&pg);
		display_current_page(&pg);

		// 显示操作菜单（在数据下方）
		printf("\n═══════════════════════════════════════════════════════\n");
		printf("                   用户管理操作\n");
		printf("═══════════════════════════════════════════════════════\n");
		printf("分页操作：\n");
		printf("  1. 首页 2. 末页 3. 上一页 4. 下一页\n");
		printf("  5. 跳转到指定页 6. 修改每页显示条数\n");
		printf("  7. 重新加载数据\n");
		printf("\n用户管理：\n");
		printf("  8. 新增用户 9. 修改用户 10. 删除用户 11. 查找用户\n");
		printf("\n系统操作：\n");
		printf("  0. 退出系统\n");
		printf("═══════════════════════════════════════════════════════\n");
		printf("请输入操作编号: ");

		scanf("%d", &choice);
		getchar();  // 清空缓冲区

		switch (choice) {
		case 1:  // 首页
			Paging_Locate_FirstPage(account_list, pg);
			break;

		case 2:  // 末页
			Paging_Locate_LastPage(account_list, pg, account_list_node_t);
			break;

		case 3:  // 上一页
			if (!Pageing_IsFirstPage(pg)) {
				Paging_Locate_OffsetPage(account_list, pg, -1, account_list_node_t);
			}
			else {
				printf("已经是第一页了！\n");
				press_any_key();
			}
			break;

		case 4:  // 下一页
			if (!Pageing_IsLastPage(pg)) {
				Paging_Locate_OffsetPage(account_list, pg, 1, account_list_node_t);
			}
			else {
				printf("已经是最后一页了！\n");
				press_any_key();
			}
			break;

		case 5:  // 跳转到指定页
		{
			int total_pages = Pageing_TotalPages(pg);
			if (total_pages <= 1) {
				printf("只有1页，无需跳转！\n");
				press_any_key();
				break;
			}

			int target_page;
			printf("请输入要跳转的页码 (1-%d): ", total_pages);
			scanf("%d", &target_page);
			getchar();

			if (target_page < 1) target_page = 1;
			if (target_page > total_pages) target_page = total_pages;

			pg.offset = (target_page - 1) * pg.pageSize;
			List_Paging(account_list, pg, account_list_node_t);

			printf("已跳转到第 %d 页\n", target_page);
			press_any_key();
		}
		break;

		case 6:  // 修改每页显示条数
		{
			int new_size;
			printf("当前每页显示: %d 条\n", pg.pageSize);
			printf("请输入新的每页显示条数 (1-20): ");
			scanf("%d", &new_size);
			getchar();

			if (new_size < 1) new_size = 1;
			if (new_size > 20) new_size = 20;

			pg.pageSize = new_size;

			// 重新计算总记录数
			recalc_total_records(&pg);

			// 调整当前偏移量
			if (pg.offset >= pg.totalRecords) {
				pg.offset = 0;
			}

			// 重新定位
			List_Paging(account_list, pg, account_list_node_t);

			printf("已修改为每页显示 %d 条\n", new_size);
			press_any_key();
		}
		break;

		case 7:  // 重新加载数据
			count = loadAllFromFile();
			if (count > 0) {
				// 重新计算总记录数
				recalc_total_records(&pg);

				// 重置到第一页
				Paging_Locate_FirstPage(account_list, pg);
				printf("成功重新加载 %d 个用户\n", count);
			}
			else {
				printf("加载数据失败！\n");
			}
			press_any_key();
			break;

		case 8:  // 新增用户
			Account_UI_Add(&pg);
			break;

		case 9:  // 修改用户
			Account_UI_Modify(&pg);
			break;

		case 10: // 删除用户
			Account_UI_Delete(&pg);
			break;

		case 11: // 查找用户
			Account_UI_Search(&pg);
			break;

		case 0:  // 退出系统
			exit_browse = 1;
			if (account_list) {
				List_Destroy(account_list, account_list_node_t);
			}
			printf("感谢使用，再见！\n");
			exit(0);
			break;

		default:
			printf("无效的选项，请重新选择！\n");
			press_any_key();
			break;
		}

	} while (!exit_browse);
}

// 读取密码输入，显示为*
void read_password_stars(char* password, int max_length) {
	int i = 0;
	char ch;

	while (1) {
		ch = _getch();  // 不显示输入的字符

		// 回车键结束输入
		if (ch == '\r' || ch == '\n') {
			break;
		}
		// 退格键处理
		else if (ch == '\b') {
			if (i > 0) {
				i--;
				printf("\b \b");  // 删除一个星号
			}
		}
		// 普通字符
		else if (i < max_length - 1 && isprint(ch)) {
			password[i++] = ch;
			printf("*");
		}
	}

	password[i] = '\0';
	printf("\n");
}

// 系统登录UI
int SysLogin() {
	int max_attempts = 3;  // 最大尝试次数
	int attempts = 0;

	while (attempts < max_attempts) {
		system("cls");  // Windows清屏

		printf("═══════════════════════════════════════════════════════\n");
		printf("                    用户登录\n");
		printf("═══════════════════════════════════════════════════════\n");

		// 显示剩余尝试次数
		printf("剩余尝试次数: %d/%d\n\n", max_attempts - attempts, max_attempts);

		char username[30];
		char password[30];

		// 输入用户名
		printf("用户名: ");
		fgets(username, 30, stdin);
		username[strcspn(username, "\n")] = '\0';  // 移除换行符

		if (strlen(username) == 0) {
			printf("用户名不能为空！\n");
			printf("按任意键继续...");
			_getch();
			continue;
		}

		// 输入密码（显示为*）
		printf("密码: ");
		read_password_stars(password, 30);

		if (strlen(password) == 0) {
			printf("密码不能为空！\n");
			printf("按任意键继续...");
			_getch();
			continue;
		}

		// 验证用户
		int result = Account_Srv_Vertify(username, password);

		if (result) {
			printf("\n登录成功！\n");

			// 获取当前用户信息
			account_list_t user_list = Account_Srv_FindByUsrName(username);
			if (user_list) {
				// 获取第一个用户
				account_list_t first_user = NULL;
				List_ForEach(user_list, first_user) {
					if (first_user) {
						printf("\n═══════════════════════════════════════════════════════\n");
						printf("                    欢迎回来，%s！\n", username);
						printf("═══════════════════════════════════════════════════════\n");
						printf("用户类型: %s\n", getAccountTypeString(first_user->data.type));
						printf("═══════════════════════════════════════════════════════\n");
						break;
					}
				}
				List_Destroy(user_list, account_list_node_t);
			}

			printf("按任意键进入主界面...");
			_getch();
			return 1;  // 登录成功
		}
		else {
			attempts++;
			printf("\n登录失败！用户名或密码错误。\n");

			if (attempts < max_attempts) {
				printf("按任意键重新尝试...");
				_getch();
			}
			else {
				printf("已超过最大尝试次数，系统将退出。\n");
				printf("按任意键退出...");
				_getch();
			}
		}
	}

	return 0;  // 登录失败
}
