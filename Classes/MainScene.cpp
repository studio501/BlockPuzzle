#include "LayoutManager.h"
#include "StageManager.h"
#include "MainScene.h"

#include <string>
#include "ChoosePackageScene.h"
#include "HelpScene.h"
#include "SettingScene.h"
#include "TangramScene.h"
#include "QuitScene.h"
#include "NativeBridge.h"

USING_NS_CC;

CCScene *MainScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    MainScene *layer = MainScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

MainScene *MainScene::create()
{
    MainScene *pRet = new MainScene();
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

MainScene::MainScene()
    :m_Background(NULL)
    ,m_pMenu(NULL)
    ,m_pContinue(NULL)
    ,m_pMore(NULL)
{
}

MainScene::~MainScene()
{
    CC_SAFE_RELEASE_NULL(m_Background);
    CC_SAFE_RELEASE_NULL(m_pMenu);
    CC_SAFE_RELEASE_NULL(m_pContinue);
    CC_SAFE_RELEASE_NULL(m_pMore);
}

bool MainScene::init()
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
    m_Background = CCSprite::create("bg_main.jpg");
    m_Background->retain();
    m_Background->setPosition(director->getVisibleOrigin() + director->getVisibleSize() / 2);
    addChild(m_Background);

    CCSprite *sprite = CCSprite::create("title.png");
    sprite->setAnchorPoint(CCPointZero);
    sprite->setPosition(ccp(visibleRect.origin.x + (visibleRect.size.width - sprite->getContentSize().width) / 2, visibleCenter.y + 0.01 * visibleRect.size.height));
    addChild(sprite);

    CCMenuItemImage *pPlay = CCMenuItemImage::create("button_play.png", NULL, this, menu_selector(MainScene::playCallback));
    pPlay->setAnchorPoint(ccp(1, 1));
    pPlay->setPosition(ccp(visibleCenter.x - 4, visibleCenter.y + 0.0175 * visibleRect.size.height));

    StageManager *stageManager = StageManager::getInstance();
    stageManager->getCurrentStage(m_Package, m_Stage);
    if (m_Package >= 0 && m_Package < stageManager->getPackageCount() && m_Stage >= 0 && m_Stage < stageManager->getStageCount(m_Package))
    {
        m_pContinue = CCMenuItemImage::create("button_continue.png", NULL, this, menu_selector(MainScene::continueCallback));
        m_pContinue->retain();
        m_pContinue->setAnchorPoint(ccp(0, 1));
        m_pContinue->setPosition(ccp(visibleCenter.x + 0.075 * visibleRect.size.width, visibleCenter.y - 0.0375 * visibleRect.size.height));
    }

    m_pMore = CCMenuItemImage::create("button_more.png", NULL, this, menu_selector(MainScene::moreCallback));
    m_pMore->retain();
    m_pMore->setAnchorPoint(ccp(0, 1));
    if (m_pContinue)
    {
        m_pMore->setPosition(ccp(visibleCenter.x - 12, visibleCenter.y - 0.22 * visibleRect.size.height));
    }
    else
    {
        m_pMore->setPosition(ccp(visibleCenter.x + 12, visibleCenter.y - 0.132 * visibleRect.size.height));
    }

    CCMenuItemImage *pBack = CCMenuItemImage::create("back.png", NULL, this, menu_selector(MainScene::backCallback));
    pBack->setAnchorPoint(CCPointZero);
    pBack->setPosition(ccp(visibleRect.origin.x + 18, visibleRect.origin.y + 12));

    CCMenuItemImage *pHelp = CCMenuItemImage::create("button_help.png", NULL, this, menu_selector(MainScene::helpCallback));
    pHelp->setAnchorPoint(CCPointZero);
    pHelp->setPosition(ccp(pBack->getContentSize().width + pBack->getPositionX() + 10, visibleRect.origin.y + 12));

    CCMenuItemImage *pSetting = CCMenuItemImage::create("button_setting.png", NULL, this, menu_selector(MainScene::settingCallback));
    pSetting->setAnchorPoint(CCPointZero);
    pSetting->setPosition(ccp(pHelp->getContentSize().width + pHelp->getPositionX() + 10, visibleRect.origin.y + 12));

    m_pMenu = CCMenu::create(pPlay, m_pMore, pBack, pHelp, pSetting, m_pContinue, NULL);
    m_pMenu->retain();
    m_pMenu->setAnchorPoint(CCPointZero);
    m_pMenu->setPosition(CCPointZero);
    addChild(m_pMenu);

    return true;
}

void MainScene::playCallback(CCObject *sender)
{
    CCDirector::sharedDirector()->pushScene(ChoosePackageScene::scene());
}

void MainScene::continueCallback(CCObject *sender)
{
    CCScene *scene = TangramScene::scene(m_Package, m_Stage, false);
    CCDirector::sharedDirector()->pushScene(scene);
}

void MainScene::moreCallback(CCObject *sender)
{
    NativeBridge::onMoreClicked();
}

void MainScene::backCallback(CCObject *sender)
{
    CCDirector::sharedDirector()->pushScene(QuitScene::scene());
}

void MainScene::settingCallback(CCObject *sender)
{
    CCDirector::sharedDirector()->pushScene(SettingScene::scene());
}

void MainScene::helpCallback(CCObject *sender)
{
    CCDirector::sharedDirector()->pushScene(HelpScene::scene());
}

void MainScene::onEnter()
{
    CCLayer::onEnter();

    NativeBridge::setAdMobBannerPosition(true);

    LayoutManager *layoutManager = LayoutManager::getInstance();
    CCRect visibleRect = layoutManager->getVisibleRect();
    CCPoint visibleCenter = visibleRect.origin + visibleRect.size / 2;

    StageManager *stageManager = StageManager::getInstance();
    stageManager->getCurrentStage(m_Package, m_Stage);
    if (m_Package >= 0 && m_Package < stageManager->getPackageCount() && m_Stage >= 0 && m_Stage < stageManager->getStageCount(m_Package))
    {
        if (!m_pContinue)
        {
            m_pContinue = CCMenuItemImage::create("button_continue.png", NULL, this, menu_selector(MainScene::continueCallback));
            m_pContinue->retain();
            m_pContinue->setAnchorPoint(ccp(0, 1));
        }
        if (!m_pContinue->getParent())
        {
            m_pMenu->addChild(m_pContinue);
        }
        m_pContinue->setPosition(ccp(visibleCenter.x + 0.075 * visibleRect.size.width, visibleCenter.y - 0.0375 * visibleRect.size.height));
    }
    else if (m_pContinue && m_pContinue->getParent())
    {
        m_pMenu->removeChild(m_pContinue, true);
    }
    
    if (m_pContinue)
    {
        m_pMore->setPosition(ccp(visibleCenter.x - 12, visibleCenter.y - 0.22 * visibleRect.size.height));
    }
    else
    {
        m_pMore->setPosition(ccp(visibleCenter.x + 12, visibleCenter.y - 0.132 * visibleRect.size.height));
    }
}

void MainScene::keyBackClicked()
{
    CCLayer::keyBackClicked();
    CCDirector::sharedDirector()->pushScene(QuitScene::scene());
}
