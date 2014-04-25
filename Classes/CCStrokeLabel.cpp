#include "CCStrokeLabel.h"

CCStrokeLabel *CCStrokeLabel::createWithFontDefinition(const char *string, ccFontDefinition &textDefinition)
{
    CCStrokeLabel *pRet = new CCStrokeLabel();

    if(pRet && pRet->init(string, textDefinition))
    {
        pRet->autorelease();
        return pRet;
    }

    CC_SAFE_DELETE(pRet);
    return NULL;
}

CCStrokeLabel::CCStrokeLabel()
    :m_SubLabel(NULL)
{
}

CCStrokeLabel::~CCStrokeLabel()
{
    CC_SAFE_RELEASE_NULL(m_SubLabel);
}

bool CCStrokeLabel::init(const char *string, ccFontDefinition &textDefinition)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    ccFontDefinition definition = textDefinition;
    definition.m_stroke.m_strokeSize /= 2;
    if (CCLabelTTF::initWithStringAndTextDefinition(string, definition))
    {
        definition.m_stroke.m_strokeSize = -definition.m_stroke.m_strokeSize;
        m_SubLabel = CCLabelTTF::createWithFontDefinition(string, definition);
        m_SubLabel->retain();
        m_SubLabel->setAnchorPoint(CCPointZero);
        m_SubLabel->setPosition(CCPointZero);
        addChild(m_SubLabel);
#else
    if (CCLabelTTF::initWithStringAndTextDefinition(string, textDefinition))
    {
        ccFontDefinition definition = textDefinition;
        definition.m_stroke.m_strokeEnabled = false;
        m_SubLabel = CCLabelTTF::createWithFontDefinition(string, definition);
        m_SubLabel->retain();
        m_SubLabel->setAnchorPoint(CCPointZero);
        m_SubLabel->setPosition(CCPointZero);
        addChild(m_SubLabel);
#endif
        return true;
    }
    return false;
}

void CCStrokeLabel::setString(const char *string)
{
    CCLabelTTF::setString(string);
    if (m_SubLabel)
    {
        m_SubLabel->setString(string);
    }
}