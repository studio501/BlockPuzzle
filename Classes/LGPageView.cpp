#include "LGPageView.h"
#include "CCStrokeLabel.h"

#include "LayoutManager.h"
NS_CC_EXT_BEGIN

LGPageView::LGPageView():
m_nCurPageIdx(0),
m_pages(NULL),
m_touchMoveDir(PAGEVIEW_TOUCHLEFT),
m_fTouchStartLocation(0.0f),
m_fTouchEndLocation(0.0f),
m_fTouchMoveStartLocation(0.0f),
movePagePoint(CCPointZero),
m_pLeftChild(NULL),
m_pRightChild(NULL),
m_fLeftBoundary(0.0f),
m_fRightBoundary(0.0f),
m_bIsAutoScrolling(false),
m_fAutoScrollDistance(0.0f),
m_fAutoScrollSpeed(0.0f),
m_nAutoScrollDir(0),
m_fChildFocusCancelOffset(5.0f),
m_pEventListener(NULL),
m_pfnEventSelector(NULL),
/*compatible*/
m_pPageTurningListener(NULL),
m_pfnPageTurningSelector(NULL),
m_PageWidth(0),
m_Indicators(NULL),
m_SelectorEnable(false),
m_IndicatorEnable(false)
    /************/
{
    m_NormalIndicator = "bar_empty.png";
    m_SelectedIndicator = "bar_full.png";
}

LGPageView::~LGPageView()
{
    m_pages->release();
    CC_SAFE_RELEASE_NULL(m_Indicators);
}

int LGPageView::getPageWidth()
{
    return m_PageWidth == 0 ? getSize().width : m_PageWidth;
}

void LGPageView::setPageWidth(int width)
{
    m_PageWidth = width;

    m_fLeftBoundary = (getSize().width - getPageWidth()) / 2;
    m_fRightBoundary = getSize().width - (getSize().width - getPageWidth()) / 2;
}

void LGPageView::setSelectorEnable(bool enable)
{
    m_SelectorEnable = enable;
}

void LGPageView::setIndicatorEnable(bool enable)
{
    m_IndicatorEnable = enable;
}

LGPageView *LGPageView::create()
{
    LGPageView *widget = new LGPageView();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return NULL;
}

bool LGPageView::init()
{
    if (Layout::init())
    {
        m_pages = CCArray::create();
        m_pages->retain();
        setClippingEnabled(true);
        setUpdateEnabled(true);
        return true;
    }
    return false;
}

void LGPageView::addWidgetToPage(UIWidget *widget, int pageIdx, bool forceCreate)
{
    if (!widget)
    {
        return;
    }
    int pageCount = m_pages->count();
    if (pageIdx < 0 || pageIdx >= pageCount)
    {
        if (forceCreate)
        {
            if (pageIdx > pageCount)
            {
                CCLOG("pageIdx is %d, it will be added as page id [%d]",pageIdx,pageCount);
            }
            Layout *newPage = createPage();
            newPage->addChild(widget);
            addPage(newPage);
        }
    }
    else
    {
        Layout *page = dynamic_cast<Layout *>(m_pages->objectAtIndex(pageIdx));
        if (page)
        {
            page->addChild(widget);
        }
    }
    
}

Layout *LGPageView::createPage()
{
    Layout *newPage = Layout::create();
    newPage->setSize(getSize());
    return newPage;
}

void LGPageView::addPage(Layout *page)
{
    if (!page)
    {
        return;
    }
    if (page->getWidgetType() != WidgetTypeContainer)
    {
        return;
    }
    if (m_pages->containsObject(page))
    {
        return;
    }

    page->setPosition(ccp(getPositionXByIndex(m_pages->count()), 0));
    m_pages->addObject(page);
    addChild(page);
    updateBoundaryPages();
}

void LGPageView::insertPage(Layout *page, int idx)
{
    if (idx < 0)
    {
        return;
    }
    if (!page)
    {
        return;
    }
    if (page->getWidgetType() != WidgetTypeContainer)
    {
        return;
    }
    if (m_pages->containsObject(page))
    {
        return;
    }
    
    int pageCount = m_pages->count();
    if (idx >= pageCount)
    {
        addPage(page);
    }
    else
    {
        m_pages->insertObject(page, idx);
        page->setPosition(ccp(getPositionXByIndex(idx), 0));
        addChild(page);

        ccArray *arrayPages = m_pages->data;
        int length = arrayPages->num;
        for (int i=(idx+1); i<length; i++) {
            UIWidget *behindPage = dynamic_cast<UIWidget *>(arrayPages->arr[i]);
            CCPoint formerPos = behindPage->getPosition();
            behindPage->setPosition(ccp(formerPos.x+getSize().width, 0));
        }
        updateBoundaryPages();
    }
}

void LGPageView::removePage(Layout *page)
{
    if (!page)
    {
        return;
    }
    removeChild(page);
    updateChildrenPosition();
    updateBoundaryPages();
}

void LGPageView::removePageAtIndex(int index)
{
    if (index < 0 || index >= (int)(m_pages->count()))
    {
        return;
    }
    Layout *page = dynamic_cast<Layout *>(m_pages->objectAtIndex(index));
    if (page)
    {
        removePage(page);
    }
}

void LGPageView::updateBoundaryPages()
{
    if (m_pages->count() <= 0)
    {
        m_pLeftChild = NULL;
        m_pRightChild = NULL;
        return;
    }
    m_pLeftChild = dynamic_cast<UIWidget *>(m_pages->objectAtIndex(0));
    m_pRightChild = dynamic_cast<UIWidget *>(m_pages->lastObject());
}

float LGPageView::getPositionXByIndex(int idx)
{
    return (getSize().width - getPageWidth()) / 2 + (getPageWidth() * (idx - m_nCurPageIdx));
}

bool LGPageView::addChild(UIWidget *widget)
{
    return Layout::addChild(widget);
}

bool LGPageView::removeChild(UIWidget *widget)
{
    if (m_pages->containsObject(widget))
    {
        m_pages->removeObject(widget);
        return Layout::removeChild(widget);
    }
    return false;
}

void LGPageView::onSizeChanged()
{
    Layout::onSizeChanged();
    m_fRightBoundary = getSize().width - (getSize().width - getPageWidth()) / 2;
    updateSelector();
    updateIndicator();
    updateChildrenSize();
    updateChildrenPosition();
}

void LGPageView::updateChildrenSize()
{
    if (!m_pages)
    {
        return;
    }
    
    CCSize selfSize = getSize();
    for (unsigned int i=0; i<m_pages->count(); i++)
    {
        Layout *page = dynamic_cast<Layout *>(m_pages->objectAtIndex(i));
        page->setSize(CCSizeMake(getPageWidth(), getSize().height));
    }
}

void LGPageView::updateChildrenPosition()
{
    if (!m_pages)
    {
        return;
    }
    
    int pageCount = m_pages->data->num;
    if (pageCount <= 0)
    {
        m_nCurPageIdx = 0;
        return;
    }
    if (m_nCurPageIdx >= pageCount)
    {
        m_nCurPageIdx = pageCount-1;
    }
    ccArray *arrayPages = m_pages->data;
    for (int i=0; i<pageCount; i++)
    {
        Layout *page = dynamic_cast<Layout *>(arrayPages->arr[i]);
        page->setPosition(ccp(getPositionXByIndex(i), 0));
    }
}

void LGPageView::removeAllChildren()
{
    m_pages->removeAllObjects();
    Layout::removeAllChildren();
}

void LGPageView::scrollToPage(int idx)
{
    if (idx < 0 || idx >= (int)(m_pages->count()))
    {
        return;
    }
    m_nCurPageIdx = idx;
    UIWidget *curPage = dynamic_cast<UIWidget *>(m_pages->objectAtIndex(idx));
    m_fAutoScrollDistance = (getSize().width - getPageWidth()) / 2 - (curPage->getPosition().x);
    m_fAutoScrollSpeed = fabs(m_fAutoScrollDistance)/0.2f;
    m_nAutoScrollDir = m_fAutoScrollDistance > 0 ? 1 : 0;
    m_bIsAutoScrolling = true;
}

void LGPageView::update(float dt)
{
    if (m_bIsAutoScrolling)
    {
        switch (m_nAutoScrollDir)
        {
            case 0:
            {
                float step = m_fAutoScrollSpeed * dt;
                if (m_fAutoScrollDistance + step >= 0.0f)
                {
                    step = -m_fAutoScrollDistance;
                    m_fAutoScrollDistance = 0.0f;
                    m_bIsAutoScrolling = false;
                    pageTurningEvent();
                }
                else
                {
                    m_fAutoScrollDistance += step;
                }
                scrollPages(-step);
                break;
            }
                break;
            case 1:
            {
                float step = m_fAutoScrollSpeed * dt;
                if (m_fAutoScrollDistance - step <= 0.0f)
                {
                    step = m_fAutoScrollDistance;
                    m_fAutoScrollDistance = 0.0f;
                    m_bIsAutoScrolling = false;
                    pageTurningEvent();
                }
                else
                {
                    m_fAutoScrollDistance -= step;
                }
                scrollPages(step);
                break;
            }
            default:
                break;
        }
    }
}

bool LGPageView::onTouchBegan(const CCPoint &touchPoint)
{
    bool pass = Layout::onTouchBegan(touchPoint);
    handlePressLogic(touchPoint);
    return pass;
}

void LGPageView::onTouchMoved(const CCPoint &touchPoint)
{
    m_touchMovePos.x = touchPoint.x;
    m_touchMovePos.y = touchPoint.y;
    handleMoveLogic(touchPoint);
    if (m_pWidgetParent)
    {
        m_pWidgetParent->checkChildInfo(1,this,touchPoint);
    }
    moveEvent();
    if (!hitTest(touchPoint))
    {
        setFocused(false);
        onTouchEnded(touchPoint);
    }
}

void LGPageView::onTouchEnded(const CCPoint &touchPoint)
{
    Layout::onTouchEnded(touchPoint);
    handleReleaseLogic(touchPoint);
}

void LGPageView::movePages(float offset)
{
    ccArray *arrayPages = m_pages->data;
    int length = arrayPages->num;
    for (int i = 0; i < length; i++)
    {
        UIWidget *child = (UIWidget *)(arrayPages->arr[i]);
        movePagePoint.x = child->getPosition().x + offset;
        movePagePoint.y = child->getPosition().y;
        child->setPosition(movePagePoint);
    }
}

bool LGPageView::scrollPages(float touchOffset)
{
    if (m_pages->count() <= 0)
    {
        return false;
    }
    
    if (!m_pLeftChild || !m_pRightChild)
    {
        return false;
    }
    
    float realOffset = touchOffset;
    
    switch (m_touchMoveDir)
    {
        case PAGEVIEW_TOUCHLEFT: // left
            if (m_pRightChild->getRightInParent() + touchOffset <= m_fRightBoundary)
            {
                realOffset = m_fRightBoundary - m_pRightChild->getRightInParent();
                movePages(realOffset);
                return false;
            }
            break;
            
        case PAGEVIEW_TOUCHRIGHT: // right
            if (m_pLeftChild->getLeftInParent() + touchOffset >= m_fLeftBoundary)
            {
                realOffset = m_fLeftBoundary - m_pLeftChild->getLeftInParent();
                movePages(realOffset);
                return false;
            }
            break;
        default:
            break;
    }
    
    movePages(realOffset);
    return true;
}

void LGPageView::onTouchCancelled(const CCPoint &touchPoint)
{
    Layout::onTouchCancelled(touchPoint);
}

void LGPageView::handlePressLogic(const CCPoint &touchPoint)
{
    CCPoint nsp = m_pRenderer->convertToNodeSpace(touchPoint);
    m_fTouchMoveStartLocation = nsp.x;
    m_fTouchStartLocation = nsp.x;
}

void LGPageView::handleMoveLogic(const CCPoint &touchPoint)
{
    CCPoint nsp = m_pRenderer->convertToNodeSpace(touchPoint);
    float offset = 0.0;
    float moveX = nsp.x;
    offset = moveX - m_fTouchMoveStartLocation;
    m_fTouchMoveStartLocation = moveX;
    if (offset < 0)
    {
        m_touchMoveDir = PAGEVIEW_TOUCHLEFT;
    }
    else if (offset > 0)
    {
        m_touchMoveDir = PAGEVIEW_TOUCHRIGHT;
    }
    scrollPages(offset);
}

void LGPageView::handleReleaseLogic(const CCPoint &touchPoint)
{
    UIWidget *curPage = dynamic_cast<UIWidget *>(m_pages->objectAtIndex(m_nCurPageIdx));
    if (curPage)
    {
        int pageCount = m_pages->count();
        UIWidget *prevPage = NULL, *nextPage = NULL;
        if (m_nCurPageIdx < pageCount - 1)
        {
            nextPage = dynamic_cast<UIWidget *>(m_pages->objectAtIndex(m_nCurPageIdx + 1));
        }
        if (m_nCurPageIdx > 0)
        {
            prevPage = dynamic_cast<UIWidget *>(m_pages->objectAtIndex(m_nCurPageIdx - 1));
        }

        int pageWidth = getPageWidth();
        float pvCenterX = getSize().width / 2;
        float curCenterX = curPage->getPosition().x + pageWidth / 2;
        if (curCenterX < pvCenterX)
        {
            if (nextPage == NULL)
            {
                scrollToPage(m_nCurPageIdx);
            }
            else
            {
                float nextCenterX = nextPage->getPosition().x + pageWidth / 2;
                if (fabs(curCenterX - pvCenterX) < fabs(nextCenterX - pvCenterX))
                {
                    scrollToPage(m_nCurPageIdx);
                }
                else
                {
                    scrollToPage(m_nCurPageIdx + 1); // switch to next page
                }
            }
        }
        else
        {
            if (prevPage == NULL)
            {
                //scrollPages(pvCenterX - curCenterX); // center current
                scrollToPage(m_nCurPageIdx);
            }
            else
            {
                float prevCenterX = prevPage->getPosition().x + pageWidth / 2;
                if (fabs(curCenterX - pvCenterX) < fabs(prevCenterX - pvCenterX))
                {
                    //scrollPages(pvCenterX - curCenterX); // center current
                    scrollToPage(m_nCurPageIdx);
                }
                else
                {
                    scrollToPage(m_nCurPageIdx - 1); // switch to previous page
                }
            }
        }
    }
}

void LGPageView::checkChildInfo(int handleState,UIWidget *sender, const CCPoint &touchPoint)
{
    interceptTouchEvent(handleState, sender, touchPoint);
}

void LGPageView::interceptTouchEvent(int handleState, UIWidget *sender, const CCPoint &touchPoint)
{
    switch (handleState)
    {
        case 0:
            handlePressLogic(touchPoint);
            break;
        case 1:
        {
            float offset = 0;
            offset = fabs(sender->getTouchStartPos().x - touchPoint.x);
            if (offset > m_fChildFocusCancelOffset)
            {
                sender->setFocused(false);
                handleMoveLogic(touchPoint);
            }
        }
            break;
        case 2:
            handleReleaseLogic(touchPoint);
            break;
            
        case 3:
            break;
    }
}

void LGPageView::pageTurningEvent()
{
    updateSelector();
    updateIndicator();

    /*Compatible*/
    if (m_pPageTurningListener && m_pfnPageTurningSelector)
    {
        (m_pPageTurningListener->*m_pfnPageTurningSelector)(this);
    }
    /************/
    if (m_pEventListener && m_pfnEventSelector)
    {
        (m_pEventListener->*m_pfnEventSelector)(this, PAGEVIEW_EVENT_TURNING);
    }
}

void LGPageView::addEventListener(CCObject *target, SEL_PageViewEvent selector)
{
    m_pEventListener = target;
    m_pfnEventSelector = selector;
}

/*Compatible*/
void LGPageView::addPageTurningEvent(CCObject *target, SEL_PageViewPageTurningEvent selector)
{
    m_pPageTurningListener = target;
    m_pfnPageTurningSelector = selector;
}
/************/

int LGPageView::getCurPageIndex() const
{
    return m_nCurPageIdx;
}

const char *LGPageView::getDescription() const
{
    return "PageView";
}

void LGPageView::updateSelector()
{
    if (m_SelectorEnable && m_pages && m_pages->count() > 1)
    {
        UIImageView *imageViewLeft = UIImageView::create();
        imageViewLeft->setTexture("l.png");
        UIImageView *imageViewRight = UIImageView::create();
        imageViewRight->setTexture("r.png");
        ccFontDefinition strokeTextDef;
        strokeTextDef.m_fontSize = 40;
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        strokeTextDef.m_fontName = "Lithograph";
#else
        strokeTextDef.m_fontName = "fonts/Lithograph.ttf";
#endif
        strokeTextDef.m_stroke.m_strokeEnabled = true;
        strokeTextDef.m_stroke.m_strokeColor = LayoutManager::getInstance()->DefaultStroke;
        strokeTextDef.m_stroke.m_strokeSize = 6;
        strokeTextDef.m_fontFillColor = ccWHITE;
        CCLabelTTF *label = CCStrokeLabel::createWithFontDefinition(CCString::createWithFormat(" %d-%d ", 1, 100)->getCString(), strokeTextDef);

        int margin = 10;
        int originX = (getSize().width - (imageViewLeft->getSize().width + margin + label->getContentSize().width + margin + imageViewRight->getSize().width)) / 2;

        CCPoint pointAnchor = CCPointMake(0, 0.5);
        int centerY = getSize().height - imageViewLeft->getSize().height / 2;

        imageViewLeft->setAnchorPoint(pointAnchor);
        imageViewLeft->setPosition(CCPointMake(originX, centerY));
        addChild(imageViewLeft);

        label->setAnchorPoint(pointAnchor);
        label->setPosition(ccp(imageViewLeft->getPosition().x + imageViewLeft->getSize().width + margin, centerY));
        addCCNode(label);

        imageViewRight->setAnchorPoint(pointAnchor);
        imageViewRight->setPosition(CCPointMake(label->getPositionX() + label->getContentSize().width + margin, centerY));
        addChild(imageViewRight);
    }
}

void LGPageView::updateIndicator()
{
    if (m_IndicatorEnable && m_pages && m_pages->count() > 1)
    {
        if (m_Indicators == NULL || m_Indicators->count() != m_pages->count())
        {
            CC_SAFE_RELEASE_NULL(m_Indicators);
            m_Indicators = CCArray::create();
            m_Indicators->retain();

            int indicatorWidthSum = 0, indicatorWidth = 0, originX = 0;
            for (unsigned int i = 0; i < m_pages->count(); i++)
            {
                UIImageView *imageView = UIImageView::create();
                imageView->setTexture(m_NormalIndicator.c_str());
                if (indicatorWidth == 0)
                {
                    indicatorWidth = imageView->getSize().width;
                    originX = (getSize().width - indicatorWidth * m_pages->count()) / 2;
                }
                imageView->setAnchorPoint(CCPointZero);
                imageView->setPosition(ccp(originX + indicatorWidthSum, 0));
                indicatorWidthSum += indicatorWidth;

                addChild(imageView);
                m_Indicators->addObject(imageView);
            }
        }

        for (unsigned int i = 0; i < m_Indicators->count(); i++)
        {
            ((UIImageView *)m_Indicators->objectAtIndex(i))->setTexture(m_nCurPageIdx == i ? m_SelectedIndicator.c_str() : m_NormalIndicator.c_str());
        }
    }

}
NS_CC_EXT_END