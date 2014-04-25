#ifndef __TANGRAM_SCENE_H__
#define __TANGRAM_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class GeomPolygon;

struct IOnGeomPolygonListener
{
    virtual void onDragged(GeomPolygon *pPolygon, CCTouch *touch) = 0;
    virtual void onMove(GeomPolygon *pPolygon, CCTouch *touch) = 0;
    virtual void onDrop(GeomPolygon *pPolygon, CCTouch *touch) = 0;
    virtual ~IOnGeomPolygonListener(){};
};

class TangramScene : public cocos2d::CCLayer, public IOnGeomPolygonListener
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    TangramScene();
    virtual ~TangramScene();
    virtual bool init();
    bool initWithPackageAndStage(int pcakge, int stage, bool pop);

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static CCScene *scene(int package, int stage, bool pop = true);

    //// implement the "static node()" method manually
    static TangramScene *create(int package, int stage, bool pop);

    CCPoint getGPosition(const CCPoint &position);
    virtual void onDragged(GeomPolygon *pPolygon, CCTouch *touch);
    virtual void onMove(GeomPolygon *pPolygon, CCTouch *touch);
    virtual void onDrop(GeomPolygon *pPolygon, CCTouch *touch);

    virtual void onEnter();
    virtual void onExit();
    virtual void draw();
    virtual void keyBackClicked();

private:
    void addGeomPolygon(int id, CCPoint *gpos = NULL);
    void updateGridiron();
    bool alignToGrid(CCPoint &p);
    bool tryToSetGeomPolygonPosition(GeomPolygon *pPolygon, const CCPoint &point, bool push, bool exactly, bool biDirection);
    bool tryToSetGeomPolygonGPosition(GeomPolygon *pPolygon, const CCPoint &point, bool push, bool exactly, bool biDirection);
    bool tryToPushGeomPolygon(GeomPolygon *pPolygon, int xStep, int yStep);
    bool tryToPushGeomPolygonOneStep(GeomPolygon *pPolygon, int xStep, int yStep, bool test);
    bool setGeomPolygonGPosition(GeomPolygon *pPolygon, const CCPoint &position);
    bool isOccupied(GeomPolygon *pPolygon, const CCPoint &p);
    inline bool isOccupied(const CCPoint &p);
    void checkComplete();
    void nextCallback(CCObject *pSender);
    void backCallback(CCObject *pSender);
    void closeGuideCallback(CCObject *pSender);
    void rateCallback(CCObject *pSender);
    void laterCallback(CCObject *pSender);
    void drawDottedRect(CCPoint origin, CCSize size, int dashLength);
    void saveState();
    void onGameComplete();
    void showFirstGuide(float dt);
    void showRateView();

private:
    bool m_Pop;

    CCRect m_GameRect;
    CCSize m_BlockSize;
    CCSize m_GridironSize;

    int m_Package;
    int m_Stage;
    CCPoint m_SolutionPixelOrigin;
    CCSize m_SolutionSize;
    unsigned int m_Gridiron[48]; // max width(16), max height(48)
    bool m_Completed;
    unsigned char m_TileUsage[12];

    CCArray *m_Polygons;
    CCSprite *m_Complete;
    CCMenuItemImage *m_Next;
    CCLayer *m_pGuideLayer;
    CCLayer *m_pRateLayer;

    bool m_OverlayShowing;
    std::set<int> m_PushOneStepHelpStack;
};

#endif // __TANGRAM_SCENE_H__
