#ifndef __CHOOSE_PACKAGE_SCENE_H__
#define __CHOOSE_PACKAGE_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class ChoosePackageScene : public CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    ChoosePackageScene();
    virtual ~ChoosePackageScene();
    virtual bool init();

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene *scene();

    // implement the "static node()" method manually
    static ChoosePackageScene *create();

    virtual void onEnter();

    void selectPackageCallback(CCObject *sender);
    void backCallback(CCObject *sender);
    virtual void keyBackClicked();

protected:
    CCArray *m_LabelCompletions;
    CCSprite *m_Background;
};

#endif // __CHOOSE_PACKAGE_SCENE_H__
