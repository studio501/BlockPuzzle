#ifndef __LG_LAYER__
#define __LG_LAYER__

#include "cocos2d.h"

USING_NS_CC;

class LGLayerColor : public CCLayerColor
{
public:
    virtual bool ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent);

    static LGLayerColor *create(const ccColor4B &color);
};

#endif /* defined(__LG_LAYER__) */
