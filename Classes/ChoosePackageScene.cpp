#include "LayoutManager.h"
#include "StageManager.h"
#include "GeomPolygon.h"
#include "ChoosePackageScene.h"

#include <string>
#include "LGPageView.h"
#include "CCStrokeLabel.h"
#include "ChooseStageScene.h"
#include "NativeBridge.h"

USING_NS_CC;
USING_NS_CC_EXT;

const char PACKAGE_IMAGE[9][20]={"level_pack1.png", "level_pack2.png", "level_pack3.png", "level_pack4.png", "level_pack5.png", "level_pack6.png", "level_pack7.png", "level_pack8.png", "level_packEXC.png"};

CCScene *ChoosePackageScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    ChoosePackageScene *layer = ChoosePackageScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

ChoosePackageScene *ChoosePackageScene::create()
{
    ChoosePackageScene *pRet = new ChoosePackageScene();
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

ChoosePackageScene::ChoosePackageScene()
    :m_Background(NULL)
{
    m_LabelCompletions = CCArray::create();
    m_LabelCompletions->retain();
}

ChoosePackageScene::~ChoosePackageScene()
{
    CC_SAFE_RELEASE_NULL(m_LabelCompletions);
    CC_SAFE_RELEASE_NULL(m_Background);
}

bool ChoosePackageScene::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    setKeypadEnabled(true);

    CCDirector *director = CCDirector::sharedDirector();
    LayoutManager *layoutManager = LayoutManager::getInstance();
    CCRect visibleRect = layoutManager->getVisibleRect();

    m_Background = CCSprite::create("bg_common.jpg");
    m_Background->retain();
    m_Background->setPosition(director->getVisibleOrigin() + director->getVisibleSize() / 2);
    addChild(m_Background);

    CCSprite *sprite = CCSprite::create("stage_select.png");
    sprite->setPosition(ccp(visibleRect.origin.x + visibleRect.size.width / 2, 630));
    addChild(sprite);

    // Create the page view
    int pageViewHeight = 556;
    int pageWidth = 336;
    LGPageView *pageView = LGPageView::create();
    pageView->setTouchEnabled(true);
    pageView->setSize(CCSizeMake(visibleRect.size.width, pageViewHeight));
    pageView->setPosition(CCSizeMake(visibleRect.origin.x,  + visibleRect.origin.y + (visibleRect.size.height - pageViewHeight) / 2));
    pageView->setPageWidth(pageWidth);
    pageView->setIndicatorEnable(true);

    StageManager *stageManager = StageManager::getInstance();
    for (int i = 0; i < sizeof(PACKAGE_IMAGE) / sizeof(PACKAGE_IMAGE[0]); ++i)
    {
        Layout *layout = Layout::create();
        layout->setSize(CCSizeMake(pageWidth, pageView->getSize().height));

        UIImageView *imageView = UIImageView::create();
        imageView->loadTexture(PACKAGE_IMAGE[i]);
        imageView->setPosition(ccp(layout->getRect().size.width / 2, layout->getRect().size.height / 2));
        imageView->setTag(i);
        imageView->addReleaseEvent(this, coco_releaseselector(ChoosePackageScene::selectPackageCallback));
        imageView->setTouchEnabled(true);
        layout->addChild(imageView);

        ccFontDefinition strokeTextDef;
        strokeTextDef.m_fontSize = 100;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        strokeTextDef.m_fontName = "Lithograph";
#else
        strokeTextDef.m_fontName = "fonts/Lithograph.ttf";
#endif
        strokeTextDef.m_stroke.m_strokeEnabled = true;
        strokeTextDef.m_stroke.m_strokeColor = LayoutManager::getInstance()->DefaultStroke;
        strokeTextDef.m_stroke.m_strokeSize = 30;
        strokeTextDef.m_fontFillColor = ccWHITE;
        CCLabelTTF *labelStage = CCStrokeLabel::createWithFontDefinition(CCString::createWithFormat(" %d ", (i + 1))->getCString(), strokeTextDef);
        labelStage->setPosition(ccp(layout->getRect().size.width / 2, layout->getRect().size.height / 2));
        layout->addCCNode(labelStage);

        strokeTextDef.m_fontSize = 40;
        strokeTextDef.m_stroke.m_strokeSize = 16;
        CCLabelTTF *labelCompletion = CCStrokeLabel::createWithFontDefinition(CCString::createWithFormat(" %d/%d ", stageManager->getCompletedStageCount(i), stageManager->getStageCount(i))->getCString(), strokeTextDef);
        labelCompletion->setPosition(ccp(layout->getRect().size.width / 2, layout->getRect().size.height / 2 - labelStage->getContentSize().height / 2 - labelCompletion->getContentSize().height / 2));
        layout->addCCNode(labelCompletion);

        m_LabelCompletions->addObject(labelCompletion);

        pageView->addPage(layout);
    }

    UILayer *pUiLayer = UILayer::create();
    pUiLayer->addWidget(pageView);
    addChild(pUiLayer);
    pUiLayer->scheduleUpdate();

    CCMenuItemImage *pBack = CCMenuItemImage::create("back.png", NULL, this, menu_selector(ChoosePackageScene::backCallback));
    pBack->setAnchorPoint(CCPointZero);
    pBack->setPosition(ccp(visibleRect.origin.x + 18, visibleRect.origin.y + 12));

    CCMenu *pMenu = CCMenu::create(pBack, NULL);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    addChild(pMenu);

    return true;
}

void ChoosePackageScene::onEnter()
{
    CCLayer::onEnter();

    NativeBridge::setAdMobBannerPosition(true);

    if (m_LabelCompletions)
    {
        for (unsigned int i = 0; i < m_LabelCompletions->count(); i++)
        {
            StageManager *stageManager = StageManager::getInstance();
            ((CCLabelTTF *)m_LabelCompletions->objectAtIndex(i))->setString(CCString::createWithFormat(" %d/%d ", stageManager->getCompletedStageCount(i), stageManager->getStageCount(i))->getCString());
        }
    }
}

void ChoosePackageScene::selectPackageCallback(CCObject *sender)
{
    CCDirector::sharedDirector()->pushScene(ChooseStageScene::scene(((UIWidget *)sender)->getTag()));
}

void ChoosePackageScene::backCallback(CCObject *sender)
{
    CCDirector::sharedDirector()->popScene();
}

void ChoosePackageScene::keyBackClicked()
{
    CCLayer::keyBackClicked();
    CCDirector::sharedDirector()->popScene();
}
