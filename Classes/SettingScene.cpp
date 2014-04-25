#include "LayoutManager.h"
#include "StageManager.h"
#include "SettingScene.h"

#include <string>
#include "ChoosePackageScene.h"
#include "CCStrokeLabel.h"
#include "TangramScene.h"
#include "NativeBridge.h"

USING_NS_CC;

CCScene *SettingScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    SettingScene *layer = SettingScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

SettingScene *SettingScene::create()
{
    SettingScene *pRet = new SettingScene();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
        return pRet;
    }
    else 
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    } 
}

SettingScene::SettingScene()
    :m_Background(NULL)
    ,m_Sound(NULL)
    ,m_Theme(NULL)
    ,m_GridironSize(NULL)
    ,m_SlidingMode(NULL)
{
}

SettingScene::~SettingScene()
{
    CC_SAFE_RELEASE_NULL(m_Background);
    CC_SAFE_RELEASE_NULL(m_Sound);
    CC_SAFE_RELEASE_NULL(m_Theme);
    CC_SAFE_RELEASE_NULL(m_GridironSize);
    CC_SAFE_RELEASE_NULL(m_SlidingMode);
}

bool SettingScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    setKeypadEnabled(true);

    LayoutManager *layoutManager = LayoutManager::getInstance();
    CCRect visibleRect = layoutManager->getVisibleRect();
    CCPoint visibleCenter = visibleRect.origin + visibleRect.size / 2;

    CCDirector *director = CCDirector::sharedDirector();
    m_Background = CCSprite::create(layoutManager->getBackgroundAsset());
    m_Background->retain();
    m_Background->setPosition(director->getVisibleOrigin() + director->getVisibleSize() / 2);
    addChild(m_Background);

    CCSprite *sprite = CCSprite::create("title_setting.png");
    sprite->setPosition(ccp(visibleCenter.x, visibleCenter.y + 0.305 * visibleRect.size.height));
    addChild(sprite);
    
    ccFontDefinition strokeTextDef;
    strokeTextDef.m_fontSize = 40;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    strokeTextDef.m_fontName = "Lithograph";
#else
    strokeTextDef.m_fontName = "fonts/Lithograph.ttf";
#endif
    strokeTextDef.m_stroke.m_strokeEnabled = true;
    strokeTextDef.m_stroke.m_strokeColor = LayoutManager::getInstance()->DefaultStroke;
    strokeTextDef.m_stroke.m_strokeSize = 16;
    strokeTextDef.m_fontFillColor = ccWHITE;

    // SOUND
    CCStrokeLabel *label = CCStrokeLabel::createWithFontDefinition(" Sound ", strokeTextDef);
    label->setPosition(ccp(140, 536));
    addChild(label);
    m_Sound = CCMenuItemImage::create("off.png", "on.png", NULL, this, menu_selector(SettingScene::soundCallback));
    m_Sound->retain();
    m_Sound->setPosition(ccp(350, label->getPositionY()));
    if (layoutManager->getSoundEnabled())
    {
        m_Sound->selected();
    }
    else
    {
        m_Sound->unselected();
    }

    // THEME
    label = CCStrokeLabel::createWithFontDefinition(" Theme ", strokeTextDef);
    label->setPosition(ccp(140, 458));
    addChild(label);
    m_Theme = CCMenuItemImage::create(layoutManager->getThemeIcon(), NULL, NULL, this, menu_selector(SettingScene::themeCallback));
    m_Theme->retain();
    m_Theme->setPosition(ccp(350, label->getPositionY()));

    int selectorPadding = 12;

    // BOARD SIZE
    label = CCStrokeLabel::createWithFontDefinition(" Board Size ", strokeTextDef);
    label->setPosition(ccp(visibleCenter.x, 364));
    addChild(label);
    strokeTextDef.m_stroke.m_strokeEnabled = false;
    strokeTextDef.m_fontFillColor = ccc3(255, 204, 0);
    m_GridironSize = CCLabelTTF::createWithFontDefinition(layoutManager->getGridironSizeString(), strokeTextDef);
    m_GridironSize->retain();
    m_GridironSize->setPosition(ccp(visibleCenter.x, 304));
    CCSprite *bg = CCSprite::create("bar_background.png");
    bg->setPosition(m_GridironSize->getPosition());
    addChild(bg);
    addChild(m_GridironSize);
    CCMenuItemImage *pBoardSizePrev = CCMenuItemImage::create("setting_prev.png", NULL, NULL, this, menu_selector(SettingScene::sizePrevCallback));
    pBoardSizePrev->setAnchorPoint(ccp(1, 0.5));
    pBoardSizePrev->setPosition(ccp(bg->getPositionX() - bg->getContentSize().width / 2 - selectorPadding, bg->getPositionY()));
    CCMenuItemImage *pBoardSizeNext = CCMenuItemImage::create("setting_next.png", NULL, NULL, this, menu_selector(SettingScene::sizeNextCallback));
    pBoardSizeNext->setAnchorPoint(ccp(0, 0.5));
    pBoardSizeNext->setPosition(ccp(bg->getPositionX() + bg->getContentSize().width / 2 + selectorPadding, bg->getPositionY()));

    // SLIDING MODE
    strokeTextDef.m_stroke.m_strokeEnabled = true;
    strokeTextDef.m_fontFillColor = ccWHITE;
    label = CCStrokeLabel::createWithFontDefinition(" Sliding Mode ", strokeTextDef);
    label->setPosition(ccp(visibleCenter.x, 218));
    addChild(label);
    strokeTextDef.m_stroke.m_strokeEnabled = false;
    strokeTextDef.m_fontFillColor = ccc3(255, 204, 0);
    m_SlidingMode = CCLabelTTF::createWithFontDefinition(LayoutManager::getInstance()->getSlidingModeString(), strokeTextDef);
    m_SlidingMode->retain();
    m_SlidingMode->setPosition(ccp(visibleCenter.x, 158));
    bg = CCSprite::create("bar_background.png");
    bg->setPosition(m_SlidingMode->getPosition());
    addChild(bg);
    addChild(m_SlidingMode);
    CCMenuItemImage *pSlidingModePrev = CCMenuItemImage::create("setting_prev.png", NULL, NULL, this, menu_selector(SettingScene::slidingModePrevCallback));
    pSlidingModePrev->setAnchorPoint(ccp(1, 0.5));
    pSlidingModePrev->setPosition(ccp(bg->getPositionX() - bg->getContentSize().width / 2 - selectorPadding, bg->getPositionY()));
    CCMenuItemImage *pSlidingModeNext = CCMenuItemImage::create("setting_next.png", NULL, NULL, this, menu_selector(SettingScene::slidingModeNextCallback));
    pSlidingModeNext->setAnchorPoint(ccp(0, 0.5));
    pSlidingModeNext->setPosition(ccp(bg->getPositionX() + bg->getContentSize().width / 2 + selectorPadding, bg->getPositionY()));

    // BACK
    CCMenuItemImage *pBack = CCMenuItemImage::create("back.png", NULL, this, menu_selector(SettingScene::backCallback));
    pBack->setAnchorPoint(CCPointZero);
    pBack->setPosition(ccp(visibleRect.origin.x + 18, visibleRect.origin.y + 12));

    CCMenu *pMenu = CCMenu::create(m_Sound, m_Theme, pBack, pBoardSizePrev, pBoardSizeNext, pSlidingModePrev, pSlidingModeNext, NULL);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    addChild(pMenu);

    return true;
}

void SettingScene::onEnter()
{
    CCLayer::onEnter();

    NativeBridge::setAdMobBannerPosition(true);
}

void SettingScene::soundCallback(CCObject *sender)
{
    LayoutManager::getInstance()->toggleSound();
    if (LayoutManager::getInstance()->getSoundEnabled())
    {
        m_Sound->selected();
    }
    else
    {
        m_Sound->unselected();
    }
}

void SettingScene::themeCallback(CCObject *sender)
{
    LayoutManager::getInstance()->nextTheme();
    m_Background->setTexture(CCTextureCache::sharedTextureCache()->addImage(LayoutManager::getInstance()->getBackgroundAsset()));
    m_Theme->setNormalImage(CCSprite::create(LayoutManager::getInstance()->getThemeIcon()));
}

void SettingScene::sizePrevCallback(CCObject *sender)
{
    LayoutManager::getInstance()->switchBoardSize(false);
    m_GridironSize->setString(LayoutManager::getInstance()->getGridironSizeString());
}

void SettingScene::sizeNextCallback(CCObject *sender)
{
    LayoutManager::getInstance()->switchBoardSize(true);
    m_GridironSize->setString(LayoutManager::getInstance()->getGridironSizeString());
}

void SettingScene::slidingModePrevCallback(CCObject *sender)
{
    LayoutManager::getInstance()->switchSlidingMode(false);
    m_SlidingMode->setString(LayoutManager::getInstance()->getSlidingModeString());
}

void SettingScene::slidingModeNextCallback(CCObject *sender)
{
    LayoutManager::getInstance()->switchSlidingMode(true);
    m_SlidingMode->setString(LayoutManager::getInstance()->getSlidingModeString());
}

void SettingScene::backCallback(CCObject *sender)
{
    CCDirector::sharedDirector()->popScene();
}

void SettingScene::keyBackClicked()
{
    CCLayer::keyBackClicked();
    CCDirector::sharedDirector()->popScene();
}
