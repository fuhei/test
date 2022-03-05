/**
 * @file log.h
 * @author Rong Tao (rongtao@cestc.cn)
 * @brief 
 * @version 0.1
 * @date 2022-03-05
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#pragma once
#include <syslog.h>

#define ldebug(fmt...) _____log(LOG_DEBUG, __FILE__, __func__, __LINE__, fmt)
#define linfo(fmt...) _____log(LOG_INFO, __FILE__, __func__, __LINE__, fmt)
#define lnotice(fmt...) _____log(LOG_NOTICE, __FILE__, __func__, __LINE__, fmt)
#define lwarning(fmt...) _____log(LOG_WARNING, __FILE__, __func__, __LINE__, fmt)
#define lerror(fmt...) _____log(LOG_ERR, __FILE__, __func__, __LINE__, fmt)
#define lcrit(fmt...) _____log(LOG_CRIT, __FILE__, __func__, __LINE__, fmt)
#define lalert(fmt...) _____log(LOG_ALERT, __FILE__, __func__, __LINE__, fmt)
#define lemerg(fmt...) _____log(LOG_EMERG, __FILE__, __func__, __LINE__, fmt)

#if TEST
void log_test()
{
	ldebug("debug %d\n", 1);
	linfo("info %d\n", 1);
	lnotice("notice %d\n", 1);
	lwarning("warning %d\n", 1);
	lerror("error %d\n", 1);
	lcrit("crit %d\n", 1);
	lalert("alert %d\n", 1);
	lemerg("emerg %d\n", 1);
}
#endif

int
_____log(int level, const char *file, const char *func, unsigned long int line, char *fmt, ...);

