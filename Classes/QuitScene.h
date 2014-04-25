#ifndef __QUIT_SCENE_H__
#define __QUIT_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class QuitScene : public CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    QuitScene();
    virtual ~QuitScene();
    virtual bool init();
    virtual void onEnter();
    virtual void onExit();

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene *scene();

    // implement the "static node()" method manually
    static QuitScene *create();

    void yesCallback(CCObject *sender);
    void noCallback(CCObject *sender);
    void moreCallback(CCObject *sender);
    virtual void keyBackClicked();
};

#endif // __QUIT_SCENE_H__
