#ifndef __CC_SPRITE_BUTTON__
#define __CC_SPRITE_BUTTON__

#include "cocos2d.h"

#define TOUCH_SENSITIVITY  20 //����������

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

    //int m_nScriptTapHandler;//�ű��������
    bool m_IsEmitTouchEvent; //�Ƿ��䴥���¼�
    CCPoint m_StartTouchPoint; //��ʼ�Ĵ�������

    CCObject *m_pListener;
    SEL_CallFuncO m_pfnSelector;
};

#endif /* defined(__CC_SPRITE_BUTTON__) */
