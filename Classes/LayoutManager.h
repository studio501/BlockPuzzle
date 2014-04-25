#ifndef _LAYOUT_MANAGER_H
#define _LAYOUT_MANAGER_H

#include "cocos2d.h"

#define HORIZONTAL_BLOCKS 12

USING_NS_CC;

typedef enum _tagSlidingModeClassic
{
    SlidingModeClassic = 0,
    SlidingModeNoPush,
    SlidingModeCount
} SlidingMode;

class LayoutManager
{
private:
    CCRect m_GameRect;
    CCRect m_VisibleRect;
    CCSize m_BlockSize;
    CCSize m_GridironSize;

    int m_AdMobHeight;
    bool m_SoundEnabled;
    int m_ThemeIndex;
    int m_GridironSizeIndex;
    int m_SlidingMode;

    inline int getHorizontalBlockCount();
    void updateConfig();

    static LayoutManager *sInstance;

public:
    ccColor3B DefaultStroke;

public:
    LayoutManager(void);
    ~LayoutManager();

    inline const CCRect &getGameRect()
    {
        return m_GameRect;
    }
    inline const CCRect &getVisibleRect()
    {
        return m_VisibleRect;
    }
    inline int getAdMobHeight()
    {
        return m_AdMobHeight;
    }
    inline const CCSize &getBlockSize()
    {
        return m_BlockSize;
    }
    inline const CCSize &getGridironSize()
    {
        return m_GridironSize;
    }
    inline int getLineWidth()
    {
        return m_GameRect.size.width / 160;
    }
    inline int getGameTopPadding()
    {
        return m_BlockSize.height * 2;
    }
    void toggleSound();
    inline bool getSoundEnabled() {return m_SoundEnabled;}
    void nextTheme();
    void switchBoardSize(bool forward);
    void switchSlidingMode(bool forward);
    inline int getSlidingMode() {return m_SlidingMode;}
    inline const char *getGridironSizeString()
    {
        return m_GridironSizeIndex == 0 ? "SMALL" : (m_GridironSizeIndex == 1 ? "NORMAL" : "LARGE");
    }
    inline const char *getSlidingModeString()
    {
        return m_SlidingMode == SlidingModeClassic ? "CLASSIC" : "NO PUSH";
    }
    char const *getBackgroundAsset();
    char const *getThemeIcon();

    static LayoutManager *getInstance()
    {
        if (sInstance == NULL)
        {
            sInstance = new LayoutManager();
        }
        return sInstance;
    }
};

#endif /*_LAYOUT_MANAGER_H*/
