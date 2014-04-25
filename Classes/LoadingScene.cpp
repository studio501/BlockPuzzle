#include "LayoutManager.h"
#include "LoadingScene.h"
#include "MainScene.h"

USING_NS_CC;

CCScene *LoadingScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    LoadingScene *layer = LoadingScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

LoadingScene *LoadingScene::create()
{
    LoadingScene *pRet = new LoadingScene();
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

LoadingScene::LoadingScene()
{
}

LoadingScene::~LoadingScene()
{
}

bool LoadingScene::init()
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

    CCSprite *sprite = CCSprite::create("bg_main.jpg");
    sprite->setPosition(visibleRect.origin + visibleRect.size / 2);
    addChild(sprite);

    sprite = CCSprite::create("loading.png");
    sprite->setAnchorPoint(ccp(0.5, 1));
    sprite->setPosition(visibleRect.origin + visibleRect.size / 2);
    addChild(sprite);

    scheduleOnce(schedule_selector(LoadingScene::start), 5);
    return true;
}

void LoadingScene::start(float dt)
{
    CCDirector::sharedDirector()->replaceScene(MainScene::scene());
}
