#include "cocos2d.h"
#include "LayoutManager.h"
#include "NativeBridge.h"

USING_NS_CC;

LayoutManager *LayoutManager::sInstance = NULL;

const char BG_ASSETS[][10] = {"bg1.jpg", "bg2.jpg"};
const char THEME_ICON_ASSETS[][15] = {"theme1.png", "theme2.png"};
const unsigned int BOARD_SIZES[] = {10, 12, 15};

LayoutManager::LayoutManager(void)
{
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();
    m_VisibleRect.setRect(origin.x, origin.y, visibleSize.width, visibleSize.height);

    m_SoundEnabled = CCUserDefault::sharedUserDefault()->getBoolForKey("sound", true);
    m_ThemeIndex = CCUserDefault::sharedUserDefault()->getIntegerForKey("theme", 0);
    m_GridironSizeIndex = CCUserDefault::sharedUserDefault()->getIntegerForKey("gsize", 1);
    m_SlidingMode = CCUserDefault::sharedUserDefault()->getIntegerForKey("slidingmode", SlidingModeNoPush);

    m_AdMobHeight = NativeBridge::getAdMobBannerHeight();

    updateConfig();

    DefaultStroke = ccc3(84, 26, 1);
}

LayoutManager::~LayoutManager()
{

}

void LayoutManager::toggleSound()
{
    m_SoundEnabled = !m_SoundEnabled;
    CCUserDefault::sharedUserDefault()->setBoolForKey("sound", m_SoundEnabled);
}

void LayoutManager::nextTheme()
{
    m_ThemeIndex = (m_ThemeIndex + 1) % (sizeof(BG_ASSETS) / sizeof(BG_ASSETS[0]));
    CCUserDefault::sharedUserDefault()->setIntegerForKey("theme", m_ThemeIndex);
}

void LayoutManager::switchBoardSize(bool forward)
{
    int boardSizeCount = sizeof(BOARD_SIZES) / sizeof(BOARD_SIZES[0]);
    m_GridironSizeIndex = (m_GridironSizeIndex + (forward ? 1 : -1) + boardSizeCount) % boardSizeCount;
    CCUserDefault::sharedUserDefault()->setIntegerForKey("gsize", m_GridironSizeIndex);
    updateConfig();
}

void LayoutManager::switchSlidingMode(bool forward)
{
    m_SlidingMode = (m_SlidingMode + (forward ? 1 : -1) + SlidingModeCount) % SlidingModeCount;
    CCUserDefault::sharedUserDefault()->setIntegerForKey("slidingmode", m_SlidingMode);
}

char const *LayoutManager::getBackgroundAsset()
{
    return BG_ASSETS[m_ThemeIndex];
}

char const *LayoutManager::getThemeIcon()
{
    return THEME_ICON_ASSETS[m_ThemeIndex];
}

int LayoutManager::getHorizontalBlockCount()
{
    return BOARD_SIZES[m_GridironSizeIndex];
}

void LayoutManager::updateConfig()
{
    int horizontalPadding = 0;

    float gameRectWidth = m_VisibleRect.size.width;
    m_BlockSize.width = m_BlockSize.height =  gameRectWidth / getHorizontalBlockCount();

    m_GridironSize.width = getHorizontalBlockCount();
    m_GridironSize.height = (int)((m_VisibleRect.size.height - getAdMobHeight()) / m_BlockSize.width);

    float gameRectHeight = m_GridironSize.height * m_BlockSize.height;
    m_GameRect.setRect(m_VisibleRect.origin.x + horizontalPadding, m_VisibleRect.origin.y + getAdMobHeight(), gameRectWidth, gameRectHeight);
}
