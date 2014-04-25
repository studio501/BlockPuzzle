#include "Util.h"

USING_NS_CC;

time_t Util::getCurrentDate(int *pYear, int *pMonth, int *pDay, int *pHour, int *pMin, int *pSecond)
{
    struct tm *tm;
    time_t timep;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)  
    time(&timep);  
#else  
    struct cc_timeval now;
    CCTime::gettimeofdayCocos2d(&now, NULL);   
    timep = now.tv_sec;
#endif 
    
    tm = localtime(&timep);
    if (pYear)
    {
        *pYear = tm->tm_year + 1900;
    }
    if (pMonth)
    {
        *pMonth = tm->tm_mon + 1;
    }
    if (pYear)
    {
        *pYear = tm->tm_year + 1900;
    }
    if (pDay)
    {
        *pDay = tm->tm_mday;
    }
    if (pHour)
    {
        *pHour = tm->tm_hour;
    }
    if (pMin)
    {
        *pMin = tm->tm_min;
    }
    if (pSecond)
    {
        *pSecond = tm->tm_sec;
    }

    return timep;
}

#define SECONDS_DAY 86400
int Util::getCurrentDays()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    time_t timep;
    time(&timep);

    return timep / SECONDS_DAY + (timep % SECONDS_DAY ? 1 : 0);
#else
    struct cc_timeval now;
    CCTime::gettimeofdayCocos2d(&now, NULL);
    
    return now.tv_sec / SECONDS_DAY + (now.tv_sec % SECONDS_DAY ? 1 : 0);
#endif
}
