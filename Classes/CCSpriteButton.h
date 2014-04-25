#ifndef __CC_SPRITE_BUTTON__
#define __CC_SPRITE_BUTTON__

#include "cocos2d.h"

#define TOUCH_SENSITIVITY  20 //触摸灵敏度

USING_NS_CC;

class CCSpriteButton : public CCSprite, public CCTouchDelegate
{
public:  
    CCSpriteButton();
    virtual ~CCSpriteButton();
    virtual bool init(CCSpriteFrame *pszNormalFile, CCSpriteFrame *pszSelectedFile);

    static CCSpriteButton *create(const char *pszNormalFile = NULL, const char *pszSelectedFile = NULL);
    static CCSpriteButton *createWithSpriteFrameName(const char *pszNormalFile, const char  *pszSelectedFile);
    static CCSpriteButton *createWithSpriteFrame(CCSpriteFrame *pNormalFrame, CCSpriteFrame *pSelectedFrame);

    void setSpriteFrame(CCSpriteFrame *pNormalFrame, CCSpriteFrame *pSelectedFrame);

    virtual void onEnter();
    virtual void onExit();
    virtual bool ccTouchBegan(CCTouch *touch, CCEvent *event);
    virtual void ccTouchMoved(CCTouch *touch, CCEvent *event);
    virtual void ccTouchEnded(CCTouch *touch, CCEvent *event);

    void registerScriptTapHandler(int nHandler);
    void unregisterScriptTapHandler(void);

    void setTarget(CCObject *rec, SEL_CallFuncO selector);

private:
    CCSprite *m_pNormalSprite;
    CCSprite *m_pSelectedSprite;

    //int m_nScriptTapHandler;//脚本函数句柄
    bool m_IsEmitTouchEvent; //是否发射触摸事件
    CCPoint m_StartTouchPoint; //开始的触摸坐标

    CCObject *m_pListener;
    SEL_CallFuncO m_pfnSelector;
};

#endif /* defined(__CC_SPRITE_BUTTON__) */
