#ifndef __LG_TABLE_VIEW__
#define __LG_TABLE_VIEW__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;

class LGTableView : public extension::CCTableView
{
public:  
    LGTableView();
    virtual ~LGTableView();
    
    virtual void ccTouchMoved(CCTouch* touch, CCEvent* event);
    virtual void ccTouchEnded(CCTouch* touch, CCEvent* event);
    void setContentOffset2(CCPoint offset, bool animated = false);
    void relocateContainer2(bool animated);
    void deaccelerateScrolling2(float dt);

    static LGTableView *create(extension::CCTableViewDataSource* dataSource, CCSize size);
};

#endif /* defined(__LG_TABLE_VIEW__) */
