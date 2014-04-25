#ifndef __LG_PAGE_VIEW_H__
#define __LG_PAGE_VIEW_H__

#include "cocos-ext.h"

NS_CC_EXT_BEGIN

//typedef enum
//{
//    LGPAGEVIEW_EVENT_TURNING,
//}LGPageViewEventType;

//typedef void (CCObject::*SEL_PageViewEvent)(CCObject *, LGPageViewEventType);
//#define pagevieweventselector(_SELECTOR)(SEL_PageViewEvent)(&_SELECTOR)
//
///*******Compatible*******/
//typedef void (CCObject::*SEL_PageViewPageTurningEvent)(CCObject *);
//#define coco_PageView_PageTurning_selector(_SELECTOR) (SEL_PageViewPageTurningEvent)(&_SELECTOR)
///************************/

//typedef enum {
//    PAGEVIEW_TOUCHLEFT,
//    PAGEVIEW_TOUCHRIGHT
//}PVTouchDir;

class LGPageView : public Layout , public UIScrollInterface
{
    
public:
    /**
     * Default constructor
     */
    LGPageView();
    
    /**
     * Default destructor
     */
    virtual ~LGPageView();
    
    /**
     * Allocates and initializes.
     */
    static LGPageView *create();
    
    /**
     * Add a widget to a page of pageview.
     *
     * @param widget    widget to be added to pageview.
     *
     * @param pageIdx   index of page.
     *
     * @param forceCreate   if force create and there is no page exsit, pageview would create a default page for adding widget.
     */
    void addWidgetToPage(UIWidget *widget, int pageIdx, bool forceCreate);
    
    /**
     * Push back a page to pageview.
     *
     * @param page    page to be added to pageview.
     */
    void addPage(Layout *page);
    
    /**
     * Inert a page to pageview.
     *
     * @param page    page to be added to pageview.
     */
    void insertPage(Layout *page, int idx);
    
    /**
     * Remove a page of pageview.
     *
     * @param page    page which will be removed.
     */
    void removePage(Layout *page);

    /**
     * Remove a page at index of pageview.
     *
     * @param index    index of page.
     */
    void removePageAtIndex(int index);
    
    /**
     * scroll pageview to index.
     *
     * @param idx    index of page.
     */
    void scrollToPage(int idx);
    
    /**
     * Gets current page index.
     *
     * @return current page index.
     */
    int getCurPageIndex() const;
    
    // event
    void addEventListener(CCObject *target, SEL_PageViewEvent selector);
    /*******Compatible*******/
    //Add call back function called when page turning.
    void addPageTurningEvent(CCObject *target, SEL_PageViewPageTurningEvent selector);
    /**************/
    
    //override "removeChild" method of widget.
    virtual bool removeChild(UIWidget *widget);
    
    //override "removeAllChildrenAndCleanUp" method of widget.
    virtual void removeAllChildren();
    
    //override "onTouchBegan" method of widget.
    virtual bool onTouchBegan(const CCPoint &touchPoint);
    
    //override "onTouchMoved" method of widget.
    virtual void onTouchMoved(const CCPoint &touchPoint);
    
    //override "onTouchEnded" method of widget.
    virtual void onTouchEnded(const CCPoint &touchPoint);
    
    //override "onTouchCancelled" method of widget.
    virtual void onTouchCancelled(const CCPoint &touchPoint);
    
    //override "update" method of widget.
    virtual void update(float dt);
    
    virtual void doLayout(){};
    
    /**
     * Returns the "class name" of widget.
     */
    virtual const char *getDescription() const;
    
    /*compatible*/
    /**
     * These methods will be removed
     */
    int getPage() const{return getCurPageIndex();};
    void removePage(Layout *page, bool cleanup){removePage(page);};
    void removePageAtIndex(int index, bool cleanup){removePageAtIndex(index);};

    int getPageWidth();
    void setPageWidth(int width);
    void setSelectorEnable(bool enable);
    void setIndicatorEnable(bool enable);

    /************/
protected:
    virtual bool addChild(UIWidget *widget);
    virtual bool init();
    Layout *createPage();
    float getPositionXByIndex(int idx);
    void updateBoundaryPages();
    virtual void handlePressLogic(const CCPoint &touchPoint);
    virtual void handleMoveLogic(const CCPoint &touchPoint);
    virtual void handleReleaseLogic(const CCPoint &touchPoint);
    virtual void interceptTouchEvent(int handleState, UIWidget *sender, const CCPoint &touchPoint);
    virtual void checkChildInfo(int handleState, UIWidget *sender, const CCPoint &touchPoint);
    virtual bool scrollPages(float touchOffset);
    void movePages(float offset);
    void pageTurningEvent();
    void updateChildrenSize();
    void updateChildrenPosition();
    virtual void onSizeChanged();
//    virtual bool isInScrollDegreeRange(UIWidget *widget);
    /*compatible*/
    /**
     * These methods will be removed
     */
    virtual void setClippingEnable(bool is){setClippingEnabled(is);};
    /************/
    virtual void setClippingEnabled(bool able){Layout::setClippingEnabled(able);};

    void updateSelector();
    void updateIndicator();

protected:
    int m_nCurPageIdx;
    CCArray *m_pages;
    PVTouchDir m_touchMoveDir;
    float m_fTouchStartLocation;
    float m_fTouchEndLocation;
    float m_fTouchMoveStartLocation;
    CCPoint movePagePoint;
    UIWidget *m_pLeftChild;
    UIWidget *m_pRightChild;
    float m_fLeftBoundary;
    float m_fRightBoundary;
    bool m_bIsAutoScrolling;
    float m_fAutoScrollDistance;
    float m_fAutoScrollSpeed;
    int m_nAutoScrollDir;
    float m_fChildFocusCancelOffset;
    CCObject *m_pEventListener;
    SEL_PageViewEvent m_pfnEventSelector;
    /*compatible*/
    CCObject *m_pPageTurningListener;
    SEL_PageViewPageTurningEvent m_pfnPageTurningSelector;
    /************/

    int m_PageWidth;
    std::string m_NormalIndicator;
    std::string m_SelectedIndicator;
    CCArray *m_Indicators;
    bool m_SelectorEnable;
    bool m_IndicatorEnable;
};

NS_CC_EXT_END

#endif /* defined(__LG_PAGE_VIEW_H__) */
