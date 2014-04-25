#ifndef __CHOOSE_STAGE_SCENE_H__
#define __CHOOSE_STAGE_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;

class CCSpriteButton;

class ChooseStageScene : public CCLayer, public extension::CCTableViewDataSource, public extension::CCTableViewDelegate
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    ChooseStageScene();
    virtual ~ChooseStageScene();
    virtual bool init();
    bool initWithPackage(int package, bool pop);

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene *scene(int package, bool pop = true);

    virtual void onEnter();
    // implement the "static node()" method manually
    static ChooseStageScene *create(int package, bool pop);

    void selectStageCallback(CCObject *sender);
    void prevPageCallback(CCObject *sender);
    void nextPageCallback(CCObject *sender);
    void backCallback(CCObject *sender);
    virtual void keyBackClicked();

    virtual void scrollViewDidScroll(extension::CCScrollView *view) {};
    virtual void scrollViewDidZoom(extension::CCScrollView *view) {}
    virtual void tableCellTouched(extension::CCTableView *table, extension::CCTableViewCell *cell);
    virtual CCSize tableCellSizeForIndex(extension::CCTableView *table, unsigned int idx);
    virtual extension::CCTableViewCell *tableCellAtIndex(extension::CCTableView *table, unsigned int idx);
    virtual unsigned int numberOfCellsInTableView(extension::CCTableView *table);

protected:
    void updateSelector();
    void updateIndicator();

protected:
    CCSprite *m_Background;
    int m_Package;
    int m_StageCount;
    extension::CCTableView *m_TableView;
    CCLabelTTF *m_SelectorLabel;
    CCSpriteButton *m_Prev;
    CCSpriteButton *m_Next;
    CCArray *m_Indicators;

    int m_CurrentPage;
    int m_PageCount;
    int m_MaxUnlockedStageIdx;
    int m_TouchSubCellIdx;
    bool m_Pop;
};

#endif // __CHOOSE_STAGE_SCENE_H__
