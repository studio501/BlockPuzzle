#ifndef _UTIL_H
#define _UTIL_H

#include "cocos2d.h"

class Util
{
public:
    static time_t getCurrentDate(int *pYear, int *pMonth, int *pDay, int *pHour, int *pMin, int *pSecond);
    static int getCurrentDays();
    static int checkCPU()
    {
        {
            union w
            {  
                int a;
                char b;
            } c;
            c.a = 1;
            return(c.b ==1);
        }
    }
};

#endif // !_UTIL_H
