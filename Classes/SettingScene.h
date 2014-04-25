#ifndef __SETTING_SCENE_H__
#define __SETTING_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class CCStrokeLabel;

class SettingScene : public CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    SettingScene();
    virtual ~SettingScene();
    virtual bool init();

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene *scene();

    // implement the "static node()" method manually
    static SettingScene *create();

    virtual void onEnter();
    void soundCallback(CCObject *sender);
    void themeCallback(CCObject *sender);
    void sizePrevCallback(CCObject *sender);
    void sizeNextCallback(CCObject *sender);
    void slidingModePrevCallback(CCObject *sender);
    void slidingModeNextCallback(CCObject *sender);
    void backCallback(CCObject *sender);
    virtual void keyBackClicked();

protected:
    CCSprite *m_Background;
    CCMenuItemImage *m_Sound;
    CCMenuItemImage *m_Theme;
    CCLabelTTF *m_GridironSize;
    CCLabelTTF *m_SlidingMode;
};

#endif // __SETTING_SCENE_H__
