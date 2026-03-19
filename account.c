#include "Account_Srv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


int main() {
    Account_Srv_InitSys();
    if (SysLogin())
        Account_UI_FetchAll();
        //MaiAccount_UI_MgtEntry();
    return 0;
}