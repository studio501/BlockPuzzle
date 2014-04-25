#include "CommonDef.h"
#include "StageManager.h"

#include "Util.h"
USING_NS_CC;

StageManager *StageManager::sInstance = NULL;

StageManager *StageManager::getInstance()
{
    if (sInstance == NULL)
    {
        sInstance = new StageManager();
    }

    return sInstance;
}

StageManager::StageManager(void)
{
    memset(m_pPackageDatas, 0, sizeof(m_pPackageDatas));
    memset(m_PackageStageCount, 0, sizeof(m_PackageStageCount));

    for (int i = 0; i < sizeof(RAW_RES) / sizeof(std::string); i++)
    {
        initPackageData(i, RAW_RES[i].c_str());
    }

    //FILE *fp = fopen("D:\\workspace\\Git\\cocos2d-x\\projects\\blockpuzzle\\Resources\\p0_", "wb+");
    //if (fp)
    //{
    //    PStageData pStageData = m_pPackageDatas[0];
    //    pStageData->width = 3;
    //    pStageData->height = 6;
    //    pStageData->polygonCount = 3;
    //    pStageData->polygons = new unsigned short[pStageData->polygonCount];
    //    pStageData->polygons[0] = 1067;
    //    pStageData->polygons[1] = 1066;
    //    pStageData->polygons[2] = 1070;

    //    pStageData = &m_pPackageDatas[0][1];
    //    pStageData->width = 3;
    //    pStageData->height = 6;
    //    pStageData->polygonCount = 3;
    //    pStageData->polygons = new unsigned short[pStageData->polygonCount];
    //    pStageData->polygons[0] = 1069;
    //    pStageData->polygons[1] = 1070;
    //    pStageData->polygons[2] = 1068;

    //    pStageData = &m_pPackageDatas[0][2];
    //    pStageData->width = 3;
    //    pStageData->height = 6;
    //    pStageData->polygonCount = 3;
    //    pStageData->polygons = new unsigned short[pStageData->polygonCount];
    //    pStageData->polygons[0] = 1072;
    //    pStageData->polygons[1] = 1071;
    //    pStageData->polygons[2] = 1069;

    //    pStageData = &m_pPackageDatas[0][3];
    //    pStageData->width = 3;
    //    pStageData->height = 7;
    //    pStageData->polygonCount = 3;
    //    pStageData->polygons = new unsigned short[pStageData->polygonCount];
    //    pStageData->polygons[0] = 1074;
    //    pStageData->polygons[1] = 1075;
    //    pStageData->polygons[2] = 1073;

    //    unsigned short count = getStageCount(0);
    //    fwrite(&count, sizeof(count), 1, fp);

    //    signed char *pData = (signed char *)m_pPackageDatas[0];

    //    for (int i = 0; i < count; i++)
    //    {
    //        int polygonCount = pData[2];
    //        fwrite(pData, 3, 1, fp);
    //        pData += sizeof(StageData);

    //        for (int j = 0; j < polygonCount; j++)
    //        {
    //            fwrite((void *)(&(m_pPackageDatas[0][i].polygons[j])), 2, 1, fp);
    //        }
    //    }

    //    fclose(fp);
    //}
}

StageManager::~StageManager(void)
{
    for (int i = 0; i < PACAKGE_COUNT; i++)
    {
        if (m_pPackageDatas[i])
        {
            for (int j = 0; j < m_PackageStageCount[i]; j++)
            {
                SAFE_ARRAY_DELETE(m_pPackageDatas[i][j].polygons);
            }
        }

        SAFE_ARRAY_DELETE(m_pPackageDatas[i]);
        m_PackageStageCount[i] = 0;
    }
}

int StageManager::getPackageCount()
{
    return sizeof(RAW_RES) / sizeof(std::string);
}

int StageManager::getStageCount(int package)
{
    if (package >= 0 && package < getPackageCount())
    {
        return m_PackageStageCount[package];
    }

    return 0;
}

int StageManager::getMaxUnlockedStage(int package)
{
    int count = 0;
    int stageCount = getStageCount(package);
    for (int i = 0; i < stageCount; i++)
    {
        if (!isCompleted(package, i))
        {
            count++;
            if (count == 3)
            {
                return i;
            }
        }
    }

    return stageCount - 1;
}

int StageManager::getCompletedStageCount(int package)
{
    int completedCount = 0, unCompletedCount = 0;
    int stageCount = getStageCount(package);
    for (int i = 0; i < stageCount; i++)
    {
        if (!isCompleted(package, i))
        {
            unCompletedCount++;
            if (unCompletedCount == 3)
            {
                return completedCount;
            }
        }
        else
        {
            completedCount++;
        }
    }

    return completedCount;
}

const int STAGE_MAP[PACAKGE_COUNT][20] = {
    {1, 0, 2, 3, 5, 10, 19, 4, 8, 6, 14, 11, 15, 16, 9, 12, 18, 13, 7, 17},
    {10, 7, 9, 4, 3, 15, 16, 0, 13, 1, 2, 5, 11, 14, 19, 6, 12, 17, 8, 18},
    {3, 1, 0, 9, 12, 17, 7, 2, 13, 15, 4, 5, 11, 14, 18, 6, 16, 19, 8, 10},
    {4, 17, 1, 7, 5, 10, 0, 2, 15, 11, 8, 16, 3, 14, 18, 19, 13, 6, 9, 12},
    {17, 14, 19, 1, 11, 10, 5, 9, 7, 16, 6, 4, 15, 2, 3, 12, 8, 0, 13, 18},
    {5, 7, 14, 4, 9, 1, 13, 11, 18, 2, 6, 3, 17, 10, 12, 19, 0, 15, 16, 8},
    {18, 0, 11, 10, 19, 15, 1, 2, 5, 12, 4, 14, 6, 7, 17, 3, 9, 8, 16, 13},
    {19, 18, 5, 8, 7, 16, 10, 15, 1, 14, 2, 13, 9, 11, 3, 17, 6, 0, 12, 4},
    {4, 1, 13, 6, 5, 18, 17, 14, 3, 0, 16, 2, 19, 8, 11, 15, 12, 9, 10, 7},
    {19, 16, 6, 15, 10, 4, 11, 18, 13, 2, 12, 14, 5, 7, 0, 17, 3, 9, 1, 8},
};

PStageData StageManager::getStageData(int package, int stage)
{
    if (package >= 0 && package < PACAKGE_COUNT && m_pPackageDatas[package] != NULL && stage >=0 && stage < m_PackageStageCount[package])
    {
        if (stage < 20)
        {
            stage = STAGE_MAP[package][stage];
        }
        return &m_pPackageDatas[package][stage];
    }
    return NULL;
}

void StageManager::saveGameState(int package, int stage, const std::string val)
{
    CCUserDefault::sharedUserDefault()->setStringForKey(CCString::createWithFormat("p%ds%d", package, stage)->getCString(), val);
}

std::string StageManager::getGameState(int package, int stage)
{
    return CCUserDefault::sharedUserDefault()->getStringForKey(CCString::createWithFormat("p%ds%d", package, stage)->getCString());
}

void StageManager::saveCurrentStage(int package, int stage)
{
    CCUserDefault::sharedUserDefault()->setIntegerForKey("currentPackage", package);
    CCUserDefault::sharedUserDefault()->setIntegerForKey("currentStage", stage);
}

void StageManager::getCurrentStage(int &package, int &stage)
{
    package = CCUserDefault::sharedUserDefault()->getIntegerForKey("currentPackage", -1);
    stage = CCUserDefault::sharedUserDefault()->getIntegerForKey("currentStage", -1);
}

bool StageManager::isCompleted(int package, int stage)
{
    if (package >= 0 && package < PACAKGE_COUNT && stage >=0 && stage < m_PackageStageCount[package])
    {
        unsigned int sum = 0;
        std::string state = getGameState(package, stage);
        if (!state.empty())
        {
            for (unsigned int i = 0; i < state.length() - 1; i += 2)
            {
                sum += (state[i] - '0');
            }
            return ((state[state.length() - 1] - '0') % 2 != sum % 2); 
        }
    }

    return false;
}

void StageManager::saveGuideShown(bool show)
{
    CCUserDefault::sharedUserDefault()->setBoolForKey("guideShown", show);
}

bool StageManager::getGuideShown()
{
    return CCUserDefault::sharedUserDefault()->getBoolForKey("guideShown", false);
}

void StageManager::addGoal()
{
    CCUserDefault::sharedUserDefault()->setStringForKey("goals", CCString::createWithFormat("%d|%d", Util::getCurrentDays(), getGoalsToday() + 1)->getCString());
}

int StageManager::getGoalsToday()
{
    std::string strGoals = CCUserDefault::sharedUserDefault()->getStringForKey("goals", "");
    if (strGoals.length() > 0)
    {
        size_t pos = strGoals.find_first_of('|');
        CCString *strDays = CCString::create(strGoals.substr(0, pos));

        if (strDays->intValue() == Util::getCurrentDays())
        {
            return CCString::create(strGoals.substr(pos + 1))->intValue();
        }
    }

    return 0;
}

void StageManager::saveRated(bool rated)
{
    CCUserDefault::sharedUserDefault()->setBoolForKey("rated", rated);
}

bool StageManager::getRated()
{
    return CCUserDefault::sharedUserDefault()->getBoolForKey("rated", false);
}

void StageManager::saveLaterTS(int days)
{
    CCUserDefault::sharedUserDefault()->setIntegerForKey("laterTS", days);
}

int StageManager::getLaterTS()
{
    return CCUserDefault::sharedUserDefault()->getIntegerForKey("laterTS", 0);
}

void StageManager::initPackageData(int package, const char *pszFileName)
{
    CCAssert(package >= 0 && package < PACAKGE_COUNT, "initPackageData package not in the rang of 0-PACKAGE_COUNT");

    unsigned long size = 0;
    unsigned char *pRawData = CCFileUtils::sharedFileUtils()->getFileData(pszFileName, "rb", &size);
    if (pRawData)
    {
        unsigned char * pData = pRawData;
        unsigned short count = *(unsigned short *)pData;
        pData += 2;

        PStageData pStageDatas = new StageData[count];
        for (int i = 0; i < count; i++)
        {
            pStageDatas[i].width = pData[0];
            pStageDatas[i].height = pData[1];
            pStageDatas[i].polygonCount = pData[2];
            pData += 3;

            pStageDatas[i].polygons = new unsigned short[pStageDatas[i].polygonCount];
            for (int j = 0; j < pStageDatas[i].polygonCount; j++, pData += 2)
            {
                pStageDatas[i].polygons[j] = *(unsigned short *)pData;
            }
        }

        m_pPackageDatas[package] = pStageDatas;
        m_PackageStageCount[package] = count;
        CCAssert(pData - pRawData == size, "initPackageData size not equal");
        delete []pRawData;
    }
}
