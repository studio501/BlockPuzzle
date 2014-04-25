#include "LayoutManager.h"
#include "QuitScene.h"

#include "NativeBridge.h"

USING_NS_CC;

CCScene *QuitScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    QuitScene *layer = QuitScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

QuitScene *QuitScene::create()
{
    QuitScene *pRet = new QuitScene();
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

QuitScene::QuitScene()
{
}

QuitScene::~QuitScene()
{
}

bool QuitScene::init()
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

    CCSprite *sprite = CCSprite::create("bg_common.jpg");
    sprite->setPosition(visibleRect.origin + visibleRect.size / 2);
    addChild(sprite);

    sprite = CCSprite::create("quit_title.png");
    sprite->setAnchorPoint(ccp(0.5, 1));
    sprite->setPosition(ccp(visibleRect.origin.x + visibleRect.size.width / 2, visibleRect.getMaxY() - layoutManager->getAdMobHeight()));
    addChild(sprite);

    CCMenuItemImage *pNo = CCMenuItemImage::create("quit_no.png", NULL, this, menu_selector(QuitScene::noCallback));
    pNo->setAnchorPoint(CCPointZero);
    pNo->setPosition(ccp(visibleRect.origin.x + 18, visibleRect.origin.y + 12));

    CCMenuItemImage *pYes = CCMenuItemImage::create("quit_yes.png", NULL, this, menu_selector(QuitScene::yesCallback));
    pYes->setAnchorPoint(ccp(1, 0));
    pYes->setPosition(ccp(visibleRect.getMaxX() - 18, pNo->getPositionY()));

    CCMenuItemImage *pMore = CCMenuItemImage::create("quit_more.png", NULL, this, menu_selector(QuitScene::moreCallback));
    pMore->setAnchorPoint(ccp(0.5, 0));
    pMore->setPosition(ccp(visibleRect.origin.x + visibleRect.size.width / 2, pNo->getPositionY()));

    CCMenu *pMenu = CCMenu::create(pNo, pYes, pMore, NULL);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    addChild(pMenu);

    return true;
}

void QuitScene::onEnter()
{
    CCLayer::onEnter();

    NativeBridge::showAdMobMRect(true);
    NativeBridge::showAdMobBanner(false);
}

void QuitScene::onExit()
{
    CCLayer::onExit();
    NativeBridge::showAdMobMRect(false);
    NativeBridge::showAdMobBanner(true);
}

void QuitScene::yesCallback(CCObject *sender)
{
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

void QuitScene::noCallback(CCObject *sender)
{
    CCDirector::sharedDirector()->popScene();
}

void QuitScene::moreCallback(CCObject *sender)
{
    NativeBridge::onMoreClicked();
}

void QuitScene::keyBackClicked()
{
    CCLOG("QuitScene::keyBackClicked");
    CCLayer::keyBackClicked();
    CCDirector::sharedDirector()->popScene();
}
