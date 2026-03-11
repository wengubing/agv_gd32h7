/********************************************************************
 *  文 件 名:DateTime.c
 *
 *------文件信息----------------------------------------
 *  程序功能: 编译器时间和日期模块
 *  创 建 人: 李永钧
 *  创建时间: 2012年12月12日
 *  版    本: 1.00
 *  修 改 人: 孙文贤
 *  修改时间: 2012年12月12日
 *  修改说明:
 *
 *  版    本: 1.01
 *  修 改 人: 李怡东
 *  修改时间: 2026年2月11日
 *  修改说明  月份判断逻辑改为查表法
 *------版本历史-----------------------------------------
 *
 ********************************************************************/

#define DATETIME_GLOBALS
#include "includes.h"

uint8_t *sysCompileTime = __TIME__;
uint8_t *sysCompileDate = __DATE__;

uint8_t AppInfo_GetCompileYear(void)
{
    return (sysCompileDate[9] - '0') * 10 + sysCompileDate[10] - '0';
}

static const struct {
    char month_str[4];
    uint8_t month_num;
} month_table[] = {
    {"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4},
    {"May", 5}, {"Jun", 6}, {"Jul", 7}, {"Aug", 8},
    {"Sep", 9}, {"Oct", 10}, {"Nov", 11}, {"Dec", 12}
};

uint8_t AppInfo_GetCompileMonth(void)
{
    char i = 0;
    for(i = 0; i < 12; i++) {
        if(sysCompileDate[0] == month_table[i].month_str[0] &&
           sysCompileDate[1] == month_table[i].month_str[1] &&
           sysCompileDate[2] == month_table[i].month_str[2]) {
            return month_table[i].month_num;
        }
    }
    return 0; // 无效月份
}

uint8_t AppInfo_GetCompileDay(void)
{
    if(sysCompileDate[4] < '0' || sysCompileDate[4] > '3')
    {
        return sysCompileDate[5] - '0';
    }
    else
    {
        return (sysCompileDate[4] - '0') * 10 + sysCompileDate[5] - '0';
    }
}

uint8_t AppInfo_GetCompileHour(void)
{
    return (sysCompileTime[0] - '0') * 10 + sysCompileTime[1] - '0';
}

uint8_t AppInfo_GetCompileMinute(void)
{
    return (sysCompileTime[3] - '0') * 10 + sysCompileTime[4] - '0';
}

uint8_t AppInfo_GetCompileSecond(void)
{
    return (sysCompileTime[6] - '0') * 10 + sysCompileTime[7] - '0';
}

/********************************************************************
 *       文件结束
 ********************************************************************/
