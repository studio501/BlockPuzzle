#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class MainScene : public CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    MainScene();
    virtual ~MainScene();
    virtual bool init();

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene *scene();

    // implement the "static node()" method manually
    static MainScene *create();

    void playCallback(CCObject *sender);
    void continueCallback(CCObject *sender);
    void moreCallback(CCObject *sender);
    void backCallback(CCObject *sender);
    void settingCallback(CCObject *sender);
    void helpCallback(CCObject *sender);
    virtual void onEnter();
    virtual void keyBackClicked();

protected:
    int m_Package;
    int m_Stage;

    CCSprite *m_Background;
    CCMenu *m_pMenu;
    CCMenuItemImage *m_pContinue;
    CCMenuItemImage *m_pMore;
};

#endif // __MAIN_SCENE_H__
