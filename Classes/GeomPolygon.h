#ifndef _GEOM_POLYGON_H_
#define _GEOM_POLYGON_H_

#include "cocos2d.h"
#include <set>

USING_NS_CC;

typedef struct _tagGeomPolygonData
{
    unsigned int unused;
    unsigned int coordinateSize;
    signed char coordinates[16];
} GeomPolygonData, *PGeomPolygonData;

typedef enum _tagPolygonState 
{
    kPolygonStateGrabbed,
    kPolygonStateUngrabbed
} PolygonState; 

struct IOnGeomPolygonListener;

class GeomPolygon : public CCSpriteBatchNode, public CCTargetedTouchDelegate
{
private:
    static PGeomPolygonData s_pData;
    static int s_DataSize;

public:
    static void loadPolygonData();
    static GeomPolygon *getGeomPolygon(int id, int tile);

public:
    GeomPolygon();
    virtual ~GeomPolygon(void);

    bool initWithIdAndTile(int id, int tile);
    virtual void onEnter();
    virtual void onExit();
    bool containsTouchLocation(CCTouch *touch);
    void setOnGeomPolygonListener(IOnGeomPolygonListener *listener);

    virtual void draw(void);
    virtual bool ccTouchBegan(CCTouch *touch, CCEvent *event);
    virtual void ccTouchMoved(CCTouch *touch, CCEvent *event);
    virtual void ccTouchEnded(CCTouch *touch, CCEvent *event);

    inline int getId(){return m_Id;}
    inline int getBlockCount()
    {
        return m_BlockCount;
    }

    inline CCPoint getBlock(int index)
    {
        int offset = m_pBlocks[index];
        return CCPointMake(offset % m_HorizontalBlockCount, offset / m_HorizontalBlockCount);
    }

    inline int getHorizontalBlockCount()
    {
        return m_HorizontalBlockCount;
    }

    inline int getVerticalBlockCount()
    {
        return m_VerticalBlockCount;
    }

    inline const CCPoint &getTouchBeganPosition()
    {
        return m_TouchBeganPos;
    }
    void setGPosition(const CCPoint &gpos);
    inline const CCPoint &getGPosition()
    {
        return m_GPosition;
    }
    inline const CCPoint &getLastGPosition()
    {
        return m_LastGPosition;
    }
    bool intersect(const CCPoint &pNewGpos, GeomPolygon *pThat);

private:
    int m_Id;
    int m_Tile;
    int m_HorizontalBlockCount, m_VerticalBlockCount;
    int m_BlockCount;
    int *m_pBlocks;
    PolygonState m_State;
    IOnGeomPolygonListener *m_OnGeomPolygonListener;

    CCPoint m_TouchBeganPos;
    CCPoint m_GPosition;
    CCPoint m_LastGPosition;

    std::set<int> m_HorizontalBorderLines;
    std::set<int> m_VerticalBorderLines;
};

#endif // !_GEOM_POLYGON_H_
