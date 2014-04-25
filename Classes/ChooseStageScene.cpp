#include "LGPageView.h"
#include "LayoutManager.h"
#include "StageManager.h"
#include "GeomPolygon.h"
#include "ChooseStageScene.h"

#include "ChoosePackageScene.h"
#include "TangramScene.h"
#include "CCSpriteButton.h"
#include "CCStrokeLabel.h"
#include "LGTableView.h"
#include <string>
#include "NativeBridge.h"

USING_NS_CC;
USING_NS_CC_EXT;

#define COUNT_PER_PAGE 100
#define SELECTOR_MARGIN 10
#define SELECTOR_HEIGHT 110
#define INDICATOR_HEIGHT 96

CCScene *ChooseStageScene::scene(int package, bool pop)
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    ChooseStageScene *layer = ChooseStageScene::create(package, pop);

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

ChooseStageScene *ChooseStageScene::create(int package, bool pop)
{
    package = MIN(MAX(0, package), StageManager::getInstance()->getPackageCount() - 1);

    ChooseStageScene *pRet = new ChooseStageScene();
    if (pRet && pRet->initWithPackage(package, pop))
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

ChooseStageScene::ChooseStageScene()
    :m_Background(NULL)
    ,m_Package(0)
    ,m_StageCount(0)
    ,m_TableView(NULL)
    ,m_SelectorLabel(NULL)
    ,m_Indicators(NULL)
    ,m_CurrentPage(0)
    ,m_MaxUnlockedStageIdx(0)
    ,m_Pop(true)
{
}

ChooseStageScene::~ChooseStageScene()
{
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("indicator.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->removeSpriteFramesFromFile("stage_state.plist");

    CC_SAFE_RELEASE_NULL(m_Background);
    CC_SAFE_RELEASE_NULL(m_TableView);
    CC_SAFE_RELEASE_NULL(m_SelectorLabel);
    CC_SAFE_RELEASE_NULL(m_Indicators);
}

bool ChooseStageScene::init()
{
    return initWithPackage(0, true);
}

// on "init" you need to initialize your instance
bool ChooseStageScene::initWithPackage(int package, bool pop)
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    m_Package = package;
    m_Pop = pop;

    StageManager *stageManager = StageManager::getInstance();
    m_StageCount = stageManager->getStageCount(package);
    m_MaxUnlockedStageIdx = stageManager->getMaxUnlockedStage(package);
    m_PageCount = m_StageCount / COUNT_PER_PAGE + (m_StageCount % COUNT_PER_PAGE ? 1 : 0);

    CCDirector *director = CCDirector::sharedDirector();
    LayoutManager *layoutManager = LayoutManager::getInstance();
    CCRect visibleRect = layoutManager->getVisibleRect();

    setKeypadEnabled(true);

    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("indicator.plist");
    CCSpriteFrameCache::sharedSpriteFrameCache()->addSpriteFramesWithFile("stage_state.plist");

    m_Background = CCSprite::create("bg_common.jpg");
    m_Background->retain();
    m_Background->setPosition(director->getVisibleOrigin() + director->getVisibleSize() / 2);
    addChild(m_Background);

    int tableHeight = visibleRect.size.height - INDICATOR_HEIGHT - SELECTOR_HEIGHT - layoutManager->getAdMobHeight();

    m_TableView = LGTableView::create(this, CCSizeMake(visibleRect.size.width, tableHeight));
    m_TableView->retain();
    m_TableView->setDirection(kCCScrollViewDirectionVertical);
    m_TableView->setVerticalFillOrder(kCCTableViewFillTopDown);
    m_TableView->setPosition(CCPointMake(0, visibleRect.origin.y + INDICATOR_HEIGHT));
    m_TableView->setDelegate(this);
    addChild(m_TableView);

    updateSelector();
    updateIndicator();

    CCMenuItemImage *pBack = CCMenuItemImage::create("back.png", NULL, this, menu_selector(ChooseStageScene::backCallback));
    pBack->setAnchorPoint(CCPointZero);
    pBack->setPosition(ccp(visibleRect.origin.x + 18, visibleRect.origin.y + 12));

    CCMenu *pMenu = CCMenu::create(pBack, NULL);
    pMenu->setAnchorPoint(CCPointZero);
    pMenu->setPosition(CCPointZero);
    addChild(pMenu);

    return true;
}

void ChooseStageScene::onEnter()
{
    CCLayer::onEnter();

    NativeBridge::setAdMobBannerPosition(true);

    m_MaxUnlockedStageIdx = StageManager::getInstance()->getMaxUnlockedStage(m_Package);

    if (m_TableView != NULL)
    {
        m_TableView->reloadData();
    }
}

void ChooseStageScene::selectStageCallback(CCObject *sender)
{
    m_TouchSubCellIdx = ((CCNode *)sender)->getTag() - 110;
}

void ChooseStageScene::prevPageCallback(CCObject *sender)
{
    if (m_CurrentPage > 0)
    {
        m_CurrentPage--;

        m_TableView->reloadData();
        updateSelector();
        updateIndicator();
    }
}

void ChooseStageScene::nextPageCallback(CCObject *sender)
{
    if (m_CurrentPage < m_PageCount - 1)
    {
        m_CurrentPage++;

        m_TableView->reloadData();
        updateSelector();
        updateIndicator();
    }
}

void ChooseStageScene::backCallback(CCObject *sender)
{
    if (m_Pop)
    {
        CCDirector::sharedDirector()->popScene();
    }	
    else
    {
        CCDirector::sharedDirector()->replaceScene(ChoosePackageScene::scene());
    }
}

void ChooseStageScene::keyBackClicked()
{
    CCLayer::keyBackClicked();
    if (m_Pop)
    {
        CCDirector::sharedDirector()->popScene();
    }
    else
    {
        CCDirector::sharedDirector()->replaceScene(ChoosePackageScene::scene());
    }
}

void ChooseStageScene::tableCellTouched(CCTableView *table, CCTableViewCell *cell)
{
    int stage = cell->getIdx() * 4 + m_TouchSubCellIdx + m_CurrentPage * COUNT_PER_PAGE;
    if (stage <= m_MaxUnlockedStageIdx)
    {
        CCDirector::sharedDirector()->pushScene(TangramScene::scene(m_Package, stage));
    }
}

CCSize ChooseStageScene::tableCellSizeForIndex(CCTableView *table, unsigned int idx)
{
    return CCSizeMake(LayoutManager::getInstance()->getVisibleRect().size.width, 107);
}

CCTableViewCell *ChooseStageScene::tableCellAtIndex(CCTableView *table, unsigned int idx)
{
    StageManager *stageManager = StageManager::getInstance();

    CCTableViewCell *cell = table->dequeueCell();
    if (!cell)
    {
        cell = new CCTableViewCell();
        cell->autorelease();

        int iconWidth = 107;
        int cellWidth = LayoutManager::getInstance()->getVisibleRect().size.width;
        int originX = (cellWidth - iconWidth * 4) / 2;
        for (int i = 0; i < 4; i++)
        {
            int stage = idx * 4 + i + m_CurrentPage * COUNT_PER_PAGE;
            CCSpriteButton *sprite = CCSpriteButton::create();
            sprite->setUserData((void *)stage);
            sprite->setTarget(this, callfuncO_selector(ChooseStageScene::selectStageCallback));
            if (stageManager->isCompleted(m_Package, stage))
            {
                sprite->setSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("completed"), NULL);
            }
            else if (stage <= m_MaxUnlockedStageIdx)
            {
                sprite->setSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("current"), NULL);
            }
            else
            {
                sprite->setSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("locked"), NULL);
            }
            sprite->setAnchorPoint(CCPointZero);
            sprite->setPosition(ccp(originX + i * iconWidth, 0));
            sprite->setTag(110 + i);
            cell->addChild(sprite);

            ccFontDefinition strokeTextDef;
            strokeTextDef.m_fontSize = 36;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
            strokeTextDef.m_fontName = "Lithograph";
#else
            strokeTextDef.m_fontName = "fonts/Lithograph.ttf";
#endif
            strokeTextDef.m_stroke.m_strokeEnabled = true;
            strokeTextDef.m_stroke.m_strokeColor = LayoutManager::getInstance()->DefaultStroke;
            strokeTextDef.m_stroke.m_strokeSize = 14;
            strokeTextDef.m_fontFillColor = ccWHITE;
            CCLabelTTF *label = CCStrokeLabel::createWithFontDefinition(CCString::createWithFormat(" %d ", stage + 1)->getCString(), strokeTextDef);
            CCPoint p = sprite->getPosition() + sprite->getContentSize() * 0.5;
            label->setPosition(p);
            label->setTag(120 + i);
            cell->addChild(label);
        }
    }
    else
    {
        for (int i = 0; i < 4; i++)
        {
            int stage = idx * 4 + i + m_CurrentPage * COUNT_PER_PAGE;
            CCSpriteButton *sprite = (CCSpriteButton *)cell->getChildByTag(110 + i);
            sprite->setUserData((void *)stage);
            if (stageManager->isCompleted(m_Package, stage))
            {
                sprite->setSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("completed"), NULL);
            }
            else if (stage <= m_MaxUnlockedStageIdx)
            {
                sprite->setSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("current"), NULL);
            }
            else
            {
                sprite->setSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName("locked"), NULL);
            }

            CCLabelTTF *label = (CCLabelTTF *)cell->getChildByTag(120 + i);
            label->setString(CCString::createWithFormat(" %d ", stage + 1)->getCString());
        }
    }

    return cell;
}

unsigned int ChooseStageScene::numberOfCellsInTableView(CCTableView *table)
{
    int n = (m_CurrentPage < m_PageCount ? COUNT_PER_PAGE : m_StageCount % COUNT_PER_PAGE);
    return n / 4 + ((n % 4) ? 1 : 0);
}

void ChooseStageScene::updateSelector()
{
    int start = 1 + m_CurrentPage * COUNT_PER_PAGE;
    const char *label = CCString::createWithFormat(" %d-%d ", start, start + (m_CurrentPage < m_PageCount ? COUNT_PER_PAGE : m_StageCount % COUNT_PER_PAGE) - 1)->getCString();
    if (m_SelectorLabel == NULL)
    {
        CCPoint pointAnchor = CCPointMake(0, 0.5);
        m_Prev = CCSpriteButton::create("l.png");
        m_Prev->setTarget(this, callfuncO_selector(ChooseStageScene::prevPageCallback));
        m_Prev->setAnchorPoint(pointAnchor);
        addChild(m_Prev);

        m_Next = CCSpriteButton::create("r.png");
        m_Next->retain();
        m_Next->setTarget(this, callfuncO_selector(ChooseStageScene::nextPageCallback));
        m_Next->setAnchorPoint(pointAnchor);
        addChild(m_Next);

        ccFontDefinition strokeTextDef;
        strokeTextDef.m_fontSize = 40;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        strokeTextDef.m_fontName = "Lithograph";
#else
        strokeTextDef.m_fontName = "fonts/Lithograph.ttf";
#endif
        strokeTextDef.m_stroke.m_strokeEnabled = true;
        strokeTextDef.m_stroke.m_strokeColor = ccc3(44, 58, 115);
        strokeTextDef.m_stroke.m_strokeSize = 16;
        strokeTextDef.m_fontFillColor = ccc3(255, 220, 24);
        m_SelectorLabel = CCStrokeLabel::createWithFontDefinition(label, strokeTextDef);
        m_SelectorLabel->retain();
        m_SelectorLabel->setAnchorPoint(pointAnchor);
        addChild(m_SelectorLabel);
    }
    else
    {
        m_SelectorLabel->setString(label);
    }

    CCRect visibleRect = LayoutManager::getInstance()->getVisibleRect();
    int originX = (visibleRect.size.width - (m_Prev->getContentSize().width + SELECTOR_MARGIN + m_SelectorLabel->getContentSize().width + SELECTOR_MARGIN + m_Next->getContentSize().width)) / 2;
    int centerY = m_TableView->getPositionY() + m_TableView->getViewSize().height + m_Prev->getContentSize().height / 2 + 14;

    m_Prev->setPosition(CCPointMake(originX, centerY));
    m_SelectorLabel->setPosition(ccp(m_Prev->getPositionX() + m_Prev->getContentSize().width + SELECTOR_MARGIN, centerY));
    m_Next->setPosition(CCPointMake(m_SelectorLabel->getPositionX() + m_SelectorLabel->getContentSize().width + SELECTOR_MARGIN, centerY));
}

void ChooseStageScene::updateIndicator()
{
    if (m_Indicators == NULL)
    {
        m_Indicators = CCArray::create();
        m_Indicators->retain();

        CCRect visibleRect = LayoutManager::getInstance()->getVisibleRect();
        int indicatorWidthSum = 0, indicatorWidth = 0, originX = 0;
        for (int i = 0; i < m_PageCount; i++)
        {
            CCSprite *sprite = CCSprite::createWithSpriteFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(i == m_CurrentPage ? "active" : "inactive"));

            if (indicatorWidth == 0)
            {
                indicatorWidth = sprite->getContentSize().width;
                originX = (visibleRect.size.width - indicatorWidth * m_PageCount) / 2;
            }
            sprite->setAnchorPoint(ccp(0, 0.5));
            sprite->setPosition(ccp(originX + indicatorWidthSum, visibleRect.origin.y + INDICATOR_HEIGHT / 2));
            indicatorWidthSum += indicatorWidth;

            addChild(sprite);
            m_Indicators->addObject(sprite);
        }
    }
    else
    {
        for (unsigned int i = 0; i < m_Indicators->count(); i++)
        {
            CCSprite *sprite = (CCSprite *)m_Indicators->objectAtIndex(i);
            sprite->setDisplayFrame(CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(i == m_CurrentPage ? "active" : "inactive"));
        }
    }
}
