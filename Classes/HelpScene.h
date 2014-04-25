#ifndef __HELP_SCENE_H__
#define __HELP_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class HelpScene : public CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    HelpScene();
    virtual ~HelpScene();
    virtual bool init();

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene *scene();

    // implement the "static node()" method manually
    static HelpScene *create();

    virtual void onEnter();
    void backCallback(CCObject *sender);
    virtual void keyBackClicked();

protected:
    CCSprite *m_Background;
};

#endif // __HELP_SCENE_H__
