#include "LGLayer.h"


LGLayerColor *LGLayerColor::create(const ccColor4B &color)
{
    LGLayerColor * pLayer = new LGLayerColor();
    if(pLayer && pLayer->initWithColor(color))
    {
        pLayer->autorelease();
        return pLayer;
    }
    CC_SAFE_DELETE(pLayer);
    return NULL;
}

bool LGLayerColor::ccTouchBegan(CCTouch *pTouch, CCEvent *pEvent)
{
    return true;
}