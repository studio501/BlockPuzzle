#include "LayoutManager.h"
#include "HelpScene.h"
#include "NativeBridge.h"

USING_NS_CC;

CCScene *HelpScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    HelpScene *layer = HelpScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

HelpScene *HelpScene::create()
{
    HelpScene *pRet = new HelpScene();
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

HelpScene::HelpScene()
    :m_Background(NULL)
{
}

HelpScene::~HelpScene()
{
    CC_SAFE_RELEASE_NULL(m_Background);
}

bool HelpScene::init()
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

    CCDirector *director = CCDirector::sharedDirector();
    m_Background = CCSprite::create("help.png");
    m_Background->retain();
    m_Background->setPosition(director->getVisibleOrigin() + director->getVisibleSize() / 2);
    addChild(m_Background);

    CCMenuItemImage *pBack = CCMenuItemImage::create("back.png", NULL, this, menu_selector(HelpScene::backCallback));
    pBack->setAnchorPoint(CCPointZero);
    pBack->setPosition(ccp(visibleRect.origin.x + 18, visibleRect.origin.y + 12));

    CCMenu *pMenu = CCMenu::create(pBack, NULL);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    addChild(pMenu);

    return true;
}

void HelpScene::onEnter()
{
    CCLayer::onEnter();

    NativeBridge::setAdMobBannerPosition(true);
}

void HelpScene::backCallback(CCObject *sender)
{
    CCDirector::sharedDirector()->popScene();
}

void HelpScene::keyBackClicked()
{
    CCLayer::keyBackClicked();
    CCDirector::sharedDirector()->popScene();
}
