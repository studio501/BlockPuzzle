#include "LayoutManager.h"
#include "StageManager.h"
#include "GeomPolygon.h"
#include "TangramScene.h"

#include "ChooseStageScene.h"
#include "CCStrokeLabel.h"
#include "NativeBridge.h"
#include <string>
#include <set>

#include "Util.h"
#include "LGLayer.h"
#include "SimpleAudioEngine.h"
// android effect only support ogg
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#define MOVE_EFFECT_FILE        "move.ogg"
#define COMPLETE_EFFECT_FILE "complete.ogg"
#define BACKGROUND_MUSIC_FILE "background.ogg"
#else
#define MOVE_EFFECT_FILE        "move.mp3"
#define COMPLETE_EFFECT_FILE "complete.mp3"
#define BACKGROUND_MUSIC_FILE "background.mp3"
#endif // CC_PLATFORM_ANDROID

USING_NS_CC;

enum {kTagGeomPolygon = 0};

CCScene *TangramScene::scene(int package, int stage, bool pop)
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();

    // 'layer' is an autorelease object
    TangramScene *layer = TangramScene::create(package, stage, pop);

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

TangramScene *TangramScene::create(int package, int stage, bool pop)
{
    package = MIN(MAX(0, package), StageManager::getInstance()->getPackageCount() - 1);
    stage = MIN(MAX(0, stage), StageManager::getInstance()->getStageCount(package) - 1);

    TangramScene *pRet = new TangramScene();
    if (pRet && pRet->initWithPackageAndStage(package, stage, pop))
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

TangramScene::TangramScene()
    :m_GameRect(CCRectZero)
    ,m_BlockSize(CCSizeZero)
    ,m_GridironSize(CCSizeZero)
    ,m_Package(0)
    ,m_Stage(0)
    ,m_SolutionSize(CCSizeMake(-1, -1))
    ,m_SolutionPixelOrigin(CCPointZero)
    ,m_Completed(false)
    ,m_pGuideLayer(NULL)
    ,m_pRateLayer(NULL)
    ,m_OverlayShowing(false)
    ,m_Polygons(NULL)
    ,m_Complete(NULL)
    ,m_Next(NULL)
    ,m_Pop(true)
{
    LayoutManager *layoutManager = LayoutManager::getInstance();
    m_GameRect = layoutManager->getGameRect();
    m_BlockSize = layoutManager->getBlockSize();
    m_GridironSize = layoutManager->getGridironSize();

    memset((void *)m_Gridiron, 0, sizeof(unsigned int) * 48);
    memset((void *)m_TileUsage, 0, sizeof(unsigned char) * 12);
    m_Polygons = CCArray::create();
    m_Polygons->retain();
}

TangramScene::~TangramScene()
{
    m_Polygons->removeAllObjects();
    CC_SAFE_RELEASE_NULL(m_Polygons);
    CC_SAFE_RELEASE_NULL(m_Complete);
    CC_SAFE_RELEASE_NULL(m_Next);
    CC_SAFE_RELEASE_NULL(m_pGuideLayer);
    CC_SAFE_RELEASE_NULL(m_pRateLayer);
    CCLOG("TangramScene::~TangramScene 0x%x", (unsigned int)this);
}

bool TangramScene::init()
{
    return initWithPackageAndStage(0, 0, true);
}

// on "init" you need to initialize your instance
bool TangramScene::initWithPackageAndStage(int package, int stage, bool pop)
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }

    setKeypadEnabled(true);

    StageManager *stageManager = StageManager::getInstance();
    if (m_Package >= 0 && m_Package < stageManager->getPackageCount() && stage >= 0 && stage < stageManager->getStageCount(package))
    {
        m_Package = package;
        m_Stage = stage;
        m_Pop = pop;

        CCDirector *director = CCDirector::sharedDirector();
        CCSprite *m_Background = CCSprite::create(LayoutManager::getInstance()->getBackgroundAsset());
        m_Background->setZOrder(-1);
        m_Background->setPosition(director->getVisibleOrigin() + director->getVisibleSize() / 2);
        addChild(m_Background);

        m_Complete = CCSprite::create("complete.png");
        m_Complete->retain();
        m_Complete->setPosition(m_GameRect.origin + ccp(m_GameRect.size.width / 2, m_GameRect.size.height / 4));
        m_Complete->setZOrder(1);
        addChild(m_Complete);

        ccFontDefinition strokeTextDef;
        strokeTextDef.m_dimensions.setSize(0, 50);
        strokeTextDef.m_fontSize = 32;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        strokeTextDef.m_fontName = "Lithograph";
#else
        strokeTextDef.m_fontName = "fonts/Lithograph.ttf";
#endif
        strokeTextDef.m_stroke.m_strokeEnabled = true;
        strokeTextDef.m_stroke.m_strokeColor = LayoutManager::getInstance()->DefaultStroke;
        strokeTextDef.m_stroke.m_strokeSize = 15;
        strokeTextDef.m_fontFillColor = ccWHITE;
        CCLOG("font color: %d, %d, %d", strokeTextDef.m_fontFillColor.r, strokeTextDef.m_fontFillColor.g, strokeTextDef.m_fontFillColor.b);
        const char *szText = CCString::createWithFormat(" Pack %d  #%d ", m_Package + 1, m_Stage + 1)->getCString();
        CCLabelTTF *label = CCStrokeLabel::createWithFontDefinition(szText, strokeTextDef);
        label->setZOrder(1);
        label->setAnchorPoint(ccp(0.5, 0.5));
        CCPoint labelCenter = CCPointMake((m_GameRect.origin.x + m_GameRect.size.width) / 2, m_GameRect.getMaxY() - label->getContentSize().height / 2 - 4);
        label->setPosition(labelCenter);
        addChild(label);

        m_Next = CCMenuItemImage::create("next.png", NULL, this, menu_selector(TangramScene::nextCallback));
        m_Next->retain();
        m_Next->setAnchorPoint(ccp(1, 1));
        m_Next->setPosition(ccp(m_GameRect.getMaxX() - 18, m_GameRect.getMaxY() - 12));
        m_Next->setOpacity(220);

        CCMenuItemImage *pBack = CCMenuItemImage::create("back.png", NULL, this, menu_selector(TangramScene::backCallback));
        pBack->setAnchorPoint(ccp(0, 1));
        pBack->setPosition(ccp(m_GameRect.origin.x + 18, m_GameRect.getMaxY() - 12));
        pBack->setOpacity(220);

        CCMenu *pMenu = CCMenu::create(m_Next, pBack, NULL);
        pMenu->setAnchorPoint(CCPointZero);
        pMenu->setZOrder(1);
        pMenu->setPosition(CCPointZero);
        addChild(pMenu);

        PStageData pStageData = stageManager->getStageData(m_Package, m_Stage);
        if (pStageData)
        {
            std::string state = stageManager->getGameState(m_Package, m_Stage);
            if (!state.empty())
            {
                unsigned int sum = 0;
                CCPoint gposition;
                for (int i = 0; i < pStageData->polygonCount; i++)
                {
                    if ((unsigned)(i) * 2 + 1 < state.length())
                    {
                        gposition.setPoint(state[i * 2] - '0', m_GridironSize.height - 1 - (state[i * 2 + 1] - '0'));
                        addGeomPolygon(pStageData->polygons[i], &gposition);
                        sum += (state[i * 2] - '0');
                    }
                }
                m_Completed = ((state[m_Polygons->count() * 2] - '0') % 2 != sum % 2); 
            }
            else
            {
                for (int i = 0; i < pStageData->polygonCount; i++)
                {
                    addGeomPolygon(pStageData->polygons[i]);
                }
            }

            m_SolutionSize.setSize(pStageData->width, pStageData->height);

            m_SolutionPixelOrigin.setPoint(m_GameRect.origin.x + ((int)(m_GridironSize.width - pStageData->width) / 2) * m_BlockSize.width, m_GameRect.origin.y + ((int)(m_GridironSize.height - pStageData->height) / 2) * m_BlockSize.height);
        }

        m_Next->setVisible(m_Completed);
        m_Complete->setVisible(m_Completed);

        if (LayoutManager::getInstance()->getSoundEnabled())
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(MOVE_EFFECT_FILE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect(COMPLETE_EFFECT_FILE);
            CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic(BACKGROUND_MUSIC_FILE);
        }

        return true;
    }

    return false;
}

void TangramScene::onEnter()
{
    CCLayer::onEnter();

    NativeBridge::setAdMobBannerPosition(false);
    StageManager::getInstance()->saveCurrentStage(m_Package, m_Stage);

    if (LayoutManager::getInstance()->getSoundEnabled())
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(BACKGROUND_MUSIC_FILE, true);
    }

    if (!StageManager::getInstance()->getGuideShown())
    {
        scheduleOnce(schedule_selector(TangramScene::showFirstGuide), 0.5);
    }
}

void TangramScene::onExit()
{
    CCLayer::onExit();
    saveState();

    if (CocosDenshion::SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying())
    {
        CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
    }
}

void TangramScene::draw()
{
    CCLayer::draw();

    CCSize solutionPixelSize = CCPointMake(m_SolutionSize.width * m_BlockSize.width, m_SolutionSize.height * m_BlockSize.height);
    ccDrawSolidRect(m_SolutionPixelOrigin, m_SolutionPixelOrigin + solutionPixelSize, ccc4f(1.0f, 1.0f, 1.0f, 0.3f));

    drawDottedRect(m_SolutionPixelOrigin, solutionPixelSize, 10);
}

void TangramScene::keyBackClicked()
{
    CCLayer::keyBackClicked();
    if (m_Pop)
    {
        CCDirector::sharedDirector()->popScene();
    }
    else
    {
        CCDirector::sharedDirector()->replaceScene(ChooseStageScene::scene(m_Package, false));
    }
}

CCPoint TangramScene::getGPosition(const CCPoint &position)
{
    float x = (position.x - m_GameRect.origin.x) / m_BlockSize.width;
    x = int(x + (x > 0 ? 0.5f : -0.5f));
    x = MIN(MAX(0, x), m_GridironSize.width - 1);

    float y = (position.y - m_GameRect.origin.y) / m_BlockSize.height;
    y = int(y + (y > 0 ? 0.5f : -0.5f));
    y = MIN(MAX(0, y), m_GridironSize.height - 1);

    return CCPointMake(x, y);
}

void TangramScene::onDragged(GeomPolygon *pPolygon, CCTouch *touch)
{
    if (LayoutManager::getInstance()->getSlidingMode() == SlidingModeNoPush)
    {
        pPolygon->setGPosition(ccp(-1, -1));
        updateGridiron();
    }
    pPolygon->setZOrder(1000);
}

void TangramScene::onMove(GeomPolygon *pPolygon, CCTouch *touch)
{
    if (LayoutManager::getInstance()->getSlidingMode() == SlidingModeClassic)
    {
        CCPoint p = pPolygon->getTouchBeganPosition() + touch->getLocation() - touch->getStartLocation();
        const CCPoint &blockOffset = getGPosition(p);
        const CCPoint &oldGpos = pPolygon->getGPosition();
        if (!blockOffset.equals(oldGpos))
        {
            tryToSetGeomPolygonPosition(pPolygon, p, true, true, false);
            if (LayoutManager::getInstance()->getSoundEnabled())
            {
                CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(MOVE_EFFECT_FILE);
            }
        }
    }
    else if (LayoutManager::getInstance()->getSlidingMode() == SlidingModeNoPush)
    {
        pPolygon->setPosition(pPolygon->getTouchBeganPosition() + touch->getLocation() - touch->getStartLocation());
    }
}

void TangramScene::onDrop(GeomPolygon *pPolygon, CCTouch *touch)
{
    if (LayoutManager::getInstance()->getSlidingMode() == SlidingModeNoPush)
    {
        CCPoint position = pPolygon->getPosition();
        if (!tryToSetGeomPolygonPosition(pPolygon, position, false, true, false))
        {
            CCPoint lastGpos = pPolygon->getLastGPosition();
            setGeomPolygonGPosition(pPolygon, lastGpos);
        }

        if (LayoutManager::getInstance()->getSoundEnabled())
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(MOVE_EFFECT_FILE);
        }
    }
    pPolygon->setZOrder(0);
}

void TangramScene::addGeomPolygon(int id, CCPoint *gpos)
{
    int tile = -1;
    srand (m_Stage);
    do {
        tile = rand() % 12;
    } while(m_TileUsage[tile]);
    m_TileUsage[tile] = 1;

    GeomPolygon *pGeomPolygon = GeomPolygon::getGeomPolygon(id, tile);
    pGeomPolygon->setOnGeomPolygonListener(this);
    m_Polygons->addObject(pGeomPolygon);

    if (gpos == NULL)
    {
        CCPoint p = CCPointMake(0, m_GameRect.getMaxY() - pGeomPolygon->getContentSize().height - LayoutManager::getInstance()->getGameTopPadding());
        if (tryToSetGeomPolygonPosition(pGeomPolygon, p, false, false, false))
        {
            addChild(pGeomPolygon);
        }
        else
        {
            m_Polygons->removeObject(pGeomPolygon);
        }
    }
    else
    {
        if (tryToSetGeomPolygonGPosition(pGeomPolygon, *gpos, false, false, true))
        {
            addChild(pGeomPolygon);
        }
        else
        {
            m_Polygons->removeObject(pGeomPolygon);
        }
    }
}

void TangramScene::updateGridiron()
{
    memset(m_Gridiron, 0, sizeof(m_Gridiron));
    for (unsigned int j = 0; j < m_Polygons->count(); j++)
    {
        GeomPolygon *pPolygon = (GeomPolygon *)m_Polygons->objectAtIndex(j);
        const CCPoint &gpos = pPolygon->getGPosition();

        if (gpos.x >= 0 && gpos.y >= 0)
        {
            for (int i = 0; i < pPolygon->getBlockCount(); i++)
            {
                CCPoint pos = gpos + pPolygon->getBlock(i);
                CCAssert(pos.y >= 0 && pos.y < m_GridironSize.height, "updateGridiron ERROR: index exceeds the bounds");
                m_Gridiron[(int)pos.y] |= (0x1 << ((int)pos.x));
            }
        }
    }

    checkComplete();

    CCLOG("*************begin to print*************\n");
    char res[33];
    for (int i = m_GridironSize.height - 1; i >= 0; i--)
    {
        for (int j = 0; j < m_GridironSize.width; j++)
        {
            res[j] = (m_Gridiron[i] & (0x1 << j)) ? 'x' : '.';
        }
        res[(int)m_GridironSize.width] = 0;
        CCLOG("%s", res);
    }
    CCLOG("*************end print*************\n");
}

bool TangramScene::tryToSetGeomPolygonPosition(GeomPolygon *pPolygon, const CCPoint &point, bool push, bool exactly, bool biDirection)
{
    const CCPoint &blockOffset = getGPosition(point);
    const CCPoint &oldGpos = pPolygon->getGPosition();
    if (!blockOffset.equals(oldGpos))
    {
        return tryToSetGeomPolygonGPosition(pPolygon, blockOffset, push, exactly, biDirection);
    }
    return true;
}

bool TangramScene::tryToSetGeomPolygonGPosition(GeomPolygon *pPolygon, const CCPoint &point, bool push, bool exactly, bool biDirection)
{
    //CCLOG("TangramScene::setGeomPolygonPosition in blockOffset:(%d, %d), w:%d, h:%d", (int)point.x, (int)point.y, pPolygon->getHorizontalBlockCount(), pPolygon->getVerticalBlockCount());

    if (isOccupied(pPolygon, point))
    {
        if (!exactly)
        {
            int width = m_GridironSize.width - (pPolygon->getHorizontalBlockCount() - 1);
            int height = m_GridironSize.height - (pPolygon->getVerticalBlockCount() - 1);
            CCPoint blockOffsetForward = point, blockOffsetBackward = point;

            do {
                if (blockOffsetForward.y >= 0)
                {
                    blockOffsetForward.x++;
                    if (blockOffsetForward.x >= width)
                    {
                        blockOffsetForward.x = 0;
                        blockOffsetForward.y--;
                    }

                    if (blockOffsetForward.y >= 0 && !isOccupied(pPolygon, blockOffsetForward))
                    {
                        return setGeomPolygonGPosition(pPolygon, blockOffsetForward);
                    }
                }

                if (biDirection)
                {
                    if (blockOffsetBackward.y < height)
                    {
                        blockOffsetBackward.x--;
                        if (blockOffsetBackward.x < 0)
                        {
                            blockOffsetBackward.x = 0;
                            blockOffsetBackward.y++;
                        }

                        if (blockOffsetBackward.y < height && !isOccupied(pPolygon, blockOffsetBackward))
                        {
                            return setGeomPolygonGPosition(pPolygon, blockOffsetBackward);
                        }
                    }
                }
            } while (blockOffsetForward.y >= 0 || blockOffsetBackward.y < height);
        }
        else if (push)
        {
            CCPoint diff = point - pPolygon->getGPosition();
            return tryToPushGeomPolygon(pPolygon, diff.x, diff.y);
        }
    }
    else
    {
        return setGeomPolygonGPosition(pPolygon, point);
    }

    return false;
}

bool TangramScene::tryToPushGeomPolygon(GeomPolygon *pPolygon, int xStep, int yStep)
{
    if (xStep)
    {
        bool reverse = false;
        if (xStep < 0)
        {
            xStep = -xStep;
            reverse = true;
        }

        for (int i = 0; i < xStep; i++)
        {
            m_PushOneStepHelpStack.clear();
            if (tryToPushGeomPolygonOneStep(pPolygon, reverse ? -1 : 1, 0, true))
            {
                m_PushOneStepHelpStack.clear();
                tryToPushGeomPolygonOneStep(pPolygon, reverse ? -1 : 1, 0, false);
            }
            else
            {
                break;
            }
        }
    }

    if (yStep)
    {
        bool reverse = false;
        if (yStep < 0)
        {
            yStep = -yStep;
            reverse = true;
        }

        for (int i = 0; i < yStep; i++)
        {
            m_PushOneStepHelpStack.clear();
            if (tryToPushGeomPolygonOneStep(pPolygon, 0, reverse ? -1 : 1, true))
            {
                m_PushOneStepHelpStack.clear();
                tryToPushGeomPolygonOneStep(pPolygon, 0, reverse ? -1 : 1, false);
            }
            else
            {
                break;
            }
        }
    }

    m_PushOneStepHelpStack.clear();
    return true;/* always return true */
}

bool TangramScene::tryToPushGeomPolygonOneStep(GeomPolygon *pPolygon, int xStep, int yStep, bool test)
{
    if (xStep)
    {
        xStep = xStep > 0 ? 1 : -1;
        yStep = 0;
    }
    else if (yStep)
    {
        xStep = 0;
        yStep = yStep > 0 ? 1 : -1;
    }
    else
    {
        return true;
    }

    const CCPoint &gpos = pPolygon->getGPosition();
    CCPoint newGpos(gpos.x + xStep, gpos.y + yStep);

    if (newGpos.x < 0 || newGpos.x + pPolygon->getHorizontalBlockCount() - 1 >= m_GridironSize.width || newGpos.y < 0 || newGpos.y + pPolygon->getVerticalBlockCount() - 1 >= m_GridironSize.height)
    {
        return false;
    }

    m_PushOneStepHelpStack.insert(pPolygon->getId());

    GeomPolygon *pThat = NULL;
    bool intersect = false;
    for (unsigned int i = 0; i < m_Polygons->count(); i++)
    {
        pThat = (GeomPolygon *)m_Polygons->objectAtIndex(i);
        if (pPolygon != pThat && m_PushOneStepHelpStack.find(pThat->getId()) == m_PushOneStepHelpStack.end() && pPolygon->intersect(newGpos, pThat))
        {
            intersect = true;
            if (tryToPushGeomPolygonOneStep(pThat, xStep, yStep, test))
            {
                if (!test)
                {
                    setGeomPolygonGPosition(pPolygon, newGpos);
                }
            }
            else
            {
                m_PushOneStepHelpStack.erase(pPolygon->getId());
                return false;
            }
        }
    }

    if (!intersect && !test)
    {
        setGeomPolygonGPosition(pPolygon, newGpos);
    }
    m_PushOneStepHelpStack.erase(pPolygon->getId());
    return true;
}

bool TangramScene::setGeomPolygonGPosition(GeomPolygon *pPolygon, const CCPoint &position)
{
    if (position.x >= 0 && position.x < m_GridironSize.width && position.y >= 0 && position.y < m_GridironSize.height)
    {
        pPolygon->setPosition(m_GameRect.origin.x + position.x * m_BlockSize.width, m_GameRect.origin.y + position.y * m_BlockSize.height);
        pPolygon->setGPosition(position);
        updateGridiron();
    }

    return true;
}

bool TangramScene::isOccupied(GeomPolygon *pPolygon, const CCPoint &p)
{
    for (int i = 0; i < pPolygon->getBlockCount(); i++)
    {
        CCPoint t = p + pPolygon->getBlock(i);
        if (isOccupied(t))
        {
            return true;
        }
    }
    return false;
}

bool TangramScene::isOccupied(const CCPoint &p)
{
    if (p.x >= 0 && p.y >= 0 && p.x < m_GridironSize.width && p.y < m_GridironSize.height)
    {
        return (m_Gridiron[(int)p.y] & (0x1 << (int)p.x)) != 0;
    }
    else
    {
        return true;
    }
}

void TangramScene::checkComplete()
{
    int minX = m_GridironSize.width, maxX = -1, minY = m_GridironSize.height, maxY = -1;
    int occupiedCount = 0;
    for (int i = 0; i < m_GridironSize.height; i++)
    {
        for (int j = 0; j < m_GridironSize.width; j++)
        {
            if (m_Gridiron[i] & (0x1 << j))
            {
                if (minX > j)
                {
                    minX = j;
                }
                if (maxX < j)
                {
                    maxX = j;
                }

                if (minY > i)
                {
                    minY = i;
                }
                if (maxY < i)
                {
                    maxY = i;
                }
                occupiedCount++;
            }
        }
    }

    if (maxX - minX + 1 == m_SolutionSize.width && maxY - minY + 1 == m_SolutionSize.height && occupiedCount == m_SolutionSize.width * m_SolutionSize.height)
    {
        if (!m_Completed)
        {
            onGameComplete();
        }

        if (LayoutManager::getInstance()->getSoundEnabled())
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(COMPLETE_EFFECT_FILE);
        }
    }
}

void TangramScene::nextCallback(CCObject *pSender)
{
    if (!m_OverlayShowing)
    {
        saveState();
        CCDirector::sharedDirector()->replaceScene(TangramScene::scene(m_Package, m_Stage + 1, m_Pop));
    }
}

void TangramScene::backCallback(CCObject *pSender)
{
    if (!m_OverlayShowing)
    {
        if (m_Pop)
        {
            CCDirector::sharedDirector()->popScene();
        }
        else
        {
            CCDirector::sharedDirector()->replaceScene(ChooseStageScene::scene(m_Package, false));
        }
    }
}

void TangramScene::closeGuideCallback(CCObject *pSender)
{
    if (m_pGuideLayer)
    {
        m_pGuideLayer->removeFromParent();

        m_pGuideLayer->release();
        m_pGuideLayer = NULL;
    }

    m_OverlayShowing = false;
}

void TangramScene::rateCallback(CCObject *pSender)
{
    StageManager::getInstance()->saveRated(true);

    NativeBridge::rateApp();

    m_pRateLayer->removeFromParent();

    m_OverlayShowing = false;
}

void TangramScene::laterCallback(CCObject *pSender)
{
    StageManager::getInstance()->saveLaterTS(Util::getCurrentDays());

    m_pRateLayer->removeFromParent();

    m_OverlayShowing = false;
}

void TangramScene::drawDottedRect(CCPoint origin, CCSize size, int dashLength)
{
    ccDrawColor4B(162, 109, 109, 255);
    GLfloat lineWidth = LayoutManager::getInstance()->getLineWidth();
    glLineWidth(lineWidth);

    float maxX = origin.x + size.width + lineWidth / 2, maxY = origin.y + size.height + lineWidth / 2;
    CCPoint p1, p2;

    // bottom
    p1.setPoint(origin.x - lineWidth / 2, origin.y - lineWidth / 2);
    p2.setPoint(p1.x + dashLength, p1.y);
    while (p2.x < maxX)
    {
        ccDrawLine(p1, p2);
        p1.x += dashLength * 2;
        p2.x = p1.x + dashLength;
    }
    if (p1.x < maxX)
    {
        if (p2.x > maxX)
        {
            p2.x = maxX;
        }
        ccDrawLine(p1, p2);
    }

    // top
    p1.setPoint(origin.x - lineWidth / 2, maxY);
    p2.setPoint(p1.x + dashLength, p1.y);
    while (p2.x < maxX)
    {
        ccDrawLine(p1, p2);
        p1.x += dashLength * 2;
        p2.x = p1.x + dashLength;
    }
    if (p1.x < maxX)
    {
        if (p2.x > maxX)
        {
            p2.x = maxX;
        }
        ccDrawLine(p1, p2);
    }

    // left
    p1.setPoint(origin.x - lineWidth / 2, origin.y - lineWidth / 2);
    p2.setPoint(p1.x, p1.y+dashLength);
    while (p2.y < maxY)
    {
        ccDrawLine(p1, p2);
        p1.y += dashLength * 2;
        p2.y = p1.y + dashLength;
    }
    if (p1.y < maxY)
    {
        if (p2.y > maxY)
        {
            p2.y = maxY;
        }
        ccDrawLine(p1, p2);
    }

    // right
    p1.setPoint(maxX, origin.y - lineWidth / 2);
    p2.setPoint(p1.x, p1.y+dashLength);
    while (p2.y < maxY)
    {
        ccDrawLine(p1, p2);
        p1.y += dashLength * 2;
        p2.y = p1.y + dashLength;
    }
    if (p1.y < maxY)
    {
        if (p2.y > maxY)
        {
            p2.y = maxY;
        }
        ccDrawLine(p1, p2);
    }

}

void TangramScene::saveState()
{
    std::string val;
    CCObject *child = NULL;
    unsigned int sum = 0;
    CCARRAY_FOREACH(m_Polygons, child)
    {
        GeomPolygon *pPolygon = (GeomPolygon *)child;
        const CCPoint &gposition = pPolygon->getGPosition();
        val.append(1, (unsigned char)gposition.x + '0');
        val.append(1, m_GridironSize.height - 1 - (unsigned char)gposition.y + '0');
        sum = sum + gposition.x;
    }
    val.append(1, (unsigned char)(m_Completed + '0' + (sum %32)));

    StageManager::getInstance()->saveGameState(m_Package, m_Stage, val);
}

void TangramScene::onGameComplete()
{
    m_Complete->setVisible(true);
    m_Next->setVisible(true);
    m_Completed = true;

    saveState();

    StageManager *stageManager = StageManager::getInstance();
    stageManager->addGoal();

    int goalsToday = stageManager->getGoalsToday();
    CCLOG("goalsToday: %d", goalsToday);
    if (goalsToday == 5 && stageManager->getRated() == false && stageManager->getLaterTS() != Util::getCurrentDays())
    {
        showRateView();
    }
 
    if (goalsToday && goalsToday % 5 == 0 && goalsToday>9)
    {
        NativeBridge::showInterstitial();
    }
}

void TangramScene::showFirstGuide(float dt)
{
    if (m_pGuideLayer == NULL)
    {
        m_pGuideLayer = LGLayerColor::create(ccc4(0, 0, 0, 77));
        m_pGuideLayer->retain();
        m_pGuideLayer->setTouchMode(kCCTouchesOneByOne);
        m_pGuideLayer->setTouchEnabled(true);
        m_pGuideLayer->setZOrder(2);

        CCRect visibleRect = LayoutManager::getInstance()->getVisibleRect();

        CCSprite *pBg = CCSprite::create("bg_guide.png");
        pBg->setPosition(visibleRect.origin + visibleRect.size / 2);
        m_pGuideLayer->addChild(pBg);

        CCMenuItemImage *pClose = CCMenuItemImage::create("close_guide.png", NULL, this, menu_selector(TangramScene::closeGuideCallback));
        pClose->setAnchorPoint(ccp(1, 1));
        pClose->setPosition(pBg->getPosition() + pBg->getContentSize() / 2);

        CCMenu *pMenu = CCMenu::create(pClose, NULL);
        pMenu->setAnchorPoint(CCPointZero);
        pMenu->setPosition(CCPointZero);
        m_pGuideLayer->addChild(pMenu);

        getParent()->addChild(m_pGuideLayer);
        m_OverlayShowing = true;

        StageManager::getInstance()->saveGuideShown(true);
    }
    else if (m_pGuideLayer->getParent() == NULL)
    {
        getParent()->addChild(m_pGuideLayer);
        m_OverlayShowing = true;

        StageManager::getInstance()->saveGuideShown(true);
    }
}

void TangramScene::showRateView()
{
    if (m_pRateLayer == NULL)
    {
        m_pRateLayer = LGLayerColor::create(ccc4(0, 0, 0, 77));
        m_pRateLayer->retain();
        m_pRateLayer->setTouchMode(kCCTouchesOneByOne);
        m_pRateLayer->setTouchEnabled(true);
        m_pRateLayer->setZOrder(2);

        CCRect visibleRect = LayoutManager::getInstance()->getVisibleRect();

        CCSprite *pBg = CCSprite::create("bg_rate.png");
        pBg->setPosition(visibleRect.origin + visibleRect.size / 2);
        m_pRateLayer->addChild(pBg);

        float bgW = pBg->getContentSize().width, bgH = pBg->getContentSize().height;
        CCMenuItemImage *pRate = CCMenuItemImage::create("button_rate.png", NULL, this, menu_selector(TangramScene::rateCallback));
        pRate->setAnchorPoint(ccp(0, 0.5));
        pRate->setPosition(ccp(pBg->getPositionX() - 0.45 * bgW , pBg->getPositionY() - bgH / 2));

        CCMenuItemImage *pLater = CCMenuItemImage::create("button_later.png", NULL, this, menu_selector(TangramScene::laterCallback));
        pLater->setAnchorPoint(ccp(1, 0.5));
        pLater->setPosition(ccp(pBg->getPositionX() + 0.45 * bgW, pBg->getPositionY() - bgH / 2));

        CCMenu *pMenu = CCMenu::create(pRate, pLater, NULL);
        pMenu->setAnchorPoint(CCPointZero);
        pMenu->setPosition(CCPointZero);
        m_pRateLayer->addChild(pMenu);

        getParent()->addChild(m_pRateLayer);
        m_OverlayShowing = true;
    }
    else if (m_pRateLayer->getParent() == NULL)
    {
        getParent()->addChild(m_pRateLayer);
        m_OverlayShowing = true;
    }
}
