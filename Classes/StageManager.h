#ifndef _STAGE_MANAGER_H
#define _STAGE_MANAGER_H

#include "cocos2d.h"
#include <string.h>

#define PACAKGE_COUNT 10

typedef struct _tagStageData
{
    signed char width;
    signed char height;
    signed char polygonCount;
    unsigned short *polygons;
}StageData, *PStageData;//关卡数据

const std::string RAW_RES[9] = {"p0", "p1", "p2", "p3", "p4", "p5", "p6", "p7", "p8",};//关卡编号
class StageManager
{
public:
    StageManager(void);
    ~StageManager(void);

    int getPackageCount();//获得包总数
    int getStageCount(int package);//获取每个包关卡总数
    int getMaxUnlockedStage(int package);//获取每个包中最大未解锁关卡数量
    int getCompletedStageCount(int package);//获取每个包中已经完成的关卡数量
    PStageData getStageData(int package, int stage);//获取指定包，指定关上的数值
    void saveGameState(int package, int stage, const std::string val);//保存头目状态
    std::string getGameState(int package, int stage);//获取关卡状态
    void saveCurrentStage(int package, int stage);//保存当前关卡状态
    void getCurrentStage(int &package, int &stage);//获取当前关卡状态
    bool isCompleted(int package, int stage);//是否已经完成该关卡

    void saveGuideShown(bool show);//保存引导显示
    bool getGuideShown();//获取引导显示
    void addGoal();//添加分数
    int getGoalsToday();//获取今天分数
    void saveRated(bool rated);//保存等级
    bool getRated();//获取等级
    void saveLaterTS(int days);//保存
    int getLaterTS();

    static StageManager *getInstance();

private:
    void initPackageData(int package, const char *pszFileName);

private:
    PStageData m_pPackageDatas[PACAKGE_COUNT];
    int m_PackageStageCount[PACAKGE_COUNT];

    static StageManager *sInstance;
};

#endif // !_STAGE_MANAGER_H
