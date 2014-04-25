#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class LoadingScene : public CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    LoadingScene();
    virtual ~LoadingScene();
    virtual bool init();

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene *scene();

    // implement the "static node()" method manually
    static LoadingScene *create();

    void start(float dt);
};

#endif // __QUIT_SCENE_H__
