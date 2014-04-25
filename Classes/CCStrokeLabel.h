#ifndef __CC_STROKE_LABEL__
#define __CC_STROKE_LABEL__

#include "cocos2d.h"

USING_NS_CC;

class CCStrokeLabel : public CCLabelTTF
{
public:  
    CCStrokeLabel();
    virtual ~CCStrokeLabel();
    virtual bool init(const char *string, ccFontDefinition &textDefinition);
    
    virtual void setString(const char *string);

    static CCStrokeLabel *createWithFontDefinition(const char *string, ccFontDefinition &textDefinition);

protected:
    CCLabelTTF *m_SubLabel;
};

#endif /* defined(__CC_STROKE_LABEL__) */
