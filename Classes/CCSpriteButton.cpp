#include "CCSpriteButton.h"

CCSpriteButton *CCSpriteButton::create(const char *pszNormalFile, const char *pszSelectedFile)
{
    return createWithSpriteFrame(pszNormalFile ? CCSprite::create(pszNormalFile)->displayFrame() : NULL, pszSelectedFile ? CCSprite::create(pszSelectedFile)->displayFrame() : NULL);
}

CCSpriteButton *CCSpriteButton::createWithSpriteFrameName(const char *pszNormalFile, const char *pszSelectedFile)
{
    CCSpriteFrame *pNormalFrame = pszNormalFile ? CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszNormalFile) : NULL;
    CCSpriteFrame *pSelectedFrame = pszSelectedFile ? CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(pszSelectedFile) : NULL;

    return createWithSpriteFrame(pNormalFrame, pSelectedFrame);
}

CCSpriteButton *CCSpriteButton::createWithSpriteFrame(CCSpriteFrame *normalFrame, CCSpriteFrame *selectedFrame)
{
    CCSpriteButton *pRet = new CCSpriteButton();

    if(pRet && pRet->init(normalFrame, selectedFrame))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool CCSpriteButton::init(CCSpriteFrame *pNormalFrame,CCSpriteFrame *pSelectedFrame)
{
    if (pNormalFrame)
    {
        m_pNormalSprite = CCSprite::createWithSpriteFrame(pNormalFrame);
        CC_SAFE_RETAIN(m_pNormalSprite);
    }
    if (pSelectedFrame)
    {
        m_pSelectedSprite = CCSprite::createWithSpriteFrame(pSelectedFrame);
        CC_SAFE_RETAIN(m_pSelectedSprite);
    }

    if (pNormalFrame)
    {
        return initWithSpriteFrame(pNormalFrame);
    }
    else
    {
        return CCSprite::init();
    }
}

CCSpriteButton::CCSpriteButton()
    :m_pNormalSprite(NULL)
    ,m_pSelectedSprite(NULL)
    //,m_nScriptTapHandler(0)
    ,m_IsEmitTouchEvent(false)
    ,m_StartTouchPoint(CCPointZero)
    ,m_pListener(NULL)
    ,m_pfnSelector(NULL)
{
}

CCSpriteButton::~CCSpriteButton()
{
    CC_SAFE_RELEASE_NULL(m_pNormalSprite);
    CC_SAFE_RELEASE_NULL(m_pSelectedSprite);
}

void CCSpriteButton::setSpriteFrame(CCSpriteFrame *pNormalFrame, CCSpriteFrame *pSelectedFrame)
{
    CC_SAFE_RELEASE_NULL(m_pNormalSprite);
    CC_SAFE_RELEASE_NULL(m_pSelectedSprite);
    if (pNormalFrame)
    {
        m_pNormalSprite = CCSprite::createWithSpriteFrame(pNormalFrame);
        CC_SAFE_RETAIN(m_pNormalSprite);
        setDisplayFrame(m_pNormalSprite->displayFrame());
    }
    if (pSelectedFrame)
    {
        m_pSelectedSprite = CCSprite::createWithSpriteFrame(pSelectedFrame);
        CC_SAFE_RETAIN(m_pSelectedSprite);
    }
}

void CCSpriteButton::onEnter()
{
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, false);
}

void CCSpriteButton::onExit()
{
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
}

bool CCSpriteButton::ccTouchBegan(CCTouch *touch, CCEvent *event)
{
    m_StartTouchPoint = convertToNodeSpace(touch->getLocation());

    CCRect rect;
    rect.origin = CCPointZero;
    rect.size = getContentSize();
    m_IsEmitTouchEvent= rect.containsPoint(m_StartTouchPoint);
    if(m_IsEmitTouchEvent)
    {
        if (m_pSelectedSprite)
        {
            setDisplayFrame(m_pSelectedSprite->displayFrame());
        }
        return true;
    }
    else  
    {
        return false;
    }
}

void CCSpriteButton::ccTouchMoved(CCTouch *touch, CCEvent *event)
{
    float distance=m_StartTouchPoint.getDistance(convertToNodeSpace(touch->getLocation()));

    if(abs(distance)<TOUCH_SENSITIVITY)  //ÅÐ¶ÏÊÇ·ñ³¬¹ýÁËÒÆ¶¯·¶Î§
    {
        m_IsEmitTouchEvent=true;
    }
    else
    {
        m_IsEmitTouchEvent=false;
    }
}

void CCSpriteButton::ccTouchEnded(CCTouch *touch, CCEvent *event)
{
    //if(m_IsEmitTouchEvent)
    //{
    //    CCScriptEngineManager::sharedManager()->getScriptEngine()->executeEvent(m_nScriptTapHandler,"end");  
    //}
    CCRect rect;
    rect.origin = CCPointZero;
    rect.size = getContentSize();
    if(rect.containsPoint(convertToNodeSpace(touch->getLocation())) && m_pListener && m_pfnSelector)
    {
        (m_pListener->*m_pfnSelector)(this);
    }

    if (m_pNormalSprite)
    {
        setDisplayFrame(m_pNormalSprite->displayFrame()); //»Ö¸´Í¼Ïñ
    }
}

//void CCSpriteButton::registerScriptTapHandler(int nHandler)
//{
//    unregisterScriptTapHandler();
//    m_nScriptTapHandler = nHandler;
//}
//
//void CCSpriteButton::unregisterScriptTapHandler(void)
//{
//    if (m_nScriptTapHandler)
//    {
//        CCScriptEngineManager::sharedManager()->getScriptEngine()->removeScriptHandler(m_nScriptTapHandler);
//        m_nScriptTapHandler = 0;
//    }
//}

void CCSpriteButton::setTarget(CCObject *rec, SEL_CallFuncO selector)
{
    m_pListener = rec;
    m_pfnSelector = selector;
}