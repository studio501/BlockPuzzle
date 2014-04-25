#include "LGTableView.h"

#define SCROLL_DEACCEL_RATE  0.95f
#define SCROLL_DEACCEL_DIST  1.0f
#define BOUNCE_DURATION      0.15f
#define INSET_RATIO          0.2f
#define MOVE_INCH            7.0f/160.0f

static float convertDistanceFromPointToInch(float pointDis)
{
    float factor = ( CCEGLView::sharedOpenGLView()->getScaleX() + CCEGLView::sharedOpenGLView()->getScaleY() ) / 2;
    return pointDis * factor / CCDevice::getDPI();
}

LGTableView *LGTableView::create(extension::CCTableViewDataSource* dataSource, CCSize size)
{
    LGTableView *table = new LGTableView();
    table->initWithViewSize(size, NULL);
    table->autorelease();
    table->setDataSource(dataSource);
    table->_updateCellPositions();
    table->_updateContentSize();

    return table;
}

LGTableView::LGTableView()
{
}

LGTableView::~LGTableView()
{
}

void LGTableView::ccTouchMoved(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible())
    {
        return;
    }

    if (m_pTouches->containsObject(touch))
    {
        if (m_pTouches->count() == 1 && m_bDragging)
        { // scrolling
            CCPoint moveDistance, newPoint, maxInset, minInset;
            CCRect  frame;
            float newX, newY;

            frame = getViewRect();

            newPoint     = this->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0));
            moveDistance = ccpSub(newPoint, m_tTouchPoint);

            float dis = 0.0f;
            if (m_eDirection == extension::kCCScrollViewDirectionVertical)
            {
                dis = moveDistance.y;
            }
            else if (m_eDirection == extension::kCCScrollViewDirectionHorizontal)
            {
                dis = moveDistance.x;
            }
            else
            {
                dis = sqrtf(moveDistance.x*moveDistance.x + moveDistance.y*moveDistance.y);
            }

            if (!m_bTouchMoved && fabs(convertDistanceFromPointToInch(dis)) < MOVE_INCH )
            {
                //CCLOG("Invalid movement, distance = [%f, %f], disInch = %f", moveDistance.x, moveDistance.y);
                return;
            }

            if (!m_bTouchMoved)
            {
                moveDistance = CCPointZero;
            }

            m_tTouchPoint = newPoint;
            m_bTouchMoved = true;

            if (frame.containsPoint(this->convertToWorldSpace(newPoint)))
            {
                switch (m_eDirection)
                {
                case extension::kCCScrollViewDirectionVertical:
                    moveDistance = ccp(0.0f, moveDistance.y);
                    break;
                case extension::kCCScrollViewDirectionHorizontal:
                    moveDistance = ccp(moveDistance.x, 0.0f);
                    break;
                default:
                    break;
                }

                maxInset = m_fMaxInset;
                minInset = m_fMinInset;

                newX     = m_pContainer->getPosition().x + moveDistance.x;
                newY     = m_pContainer->getPosition().y + moveDistance.y;

                m_tScrollDistance = moveDistance;
                this->setContentOffset2(ccp(newX, newY));
            }
        }
        else if (m_pTouches->count() == 2 && !m_bDragging)
        {
            const float len = ccpDistance(m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(0)),
                m_pContainer->convertTouchToNodeSpace((CCTouch*)m_pTouches->objectAtIndex(1)));
            this->setZoomScale(this->getZoomScale()*len/m_fTouchLength);
        }
    }

    if (m_pTouchedCell && isTouchMoved()) {
        if(m_pTableViewDelegate != NULL) {
            m_pTableViewDelegate->tableCellUnhighlight(this, m_pTouchedCell);
        }

        m_pTouchedCell = NULL;
    }
}

void LGTableView::ccTouchEnded(CCTouch* touch, CCEvent* event)
{
    if (!this->isVisible()) {
        return;
    }

    if (m_pTouchedCell){
        CCRect bb = this->boundingBox();
        bb.origin = m_pParent->convertToWorldSpace(bb.origin);

        if (bb.containsPoint(touch->getLocation()) && m_pTableViewDelegate != NULL)
        {
            m_pTableViewDelegate->tableCellUnhighlight(this, m_pTouchedCell);
            m_pTableViewDelegate->tableCellTouched(this, m_pTouchedCell);
        }

        m_pTouchedCell = NULL;
    }

    if (!this->isVisible())
    {
        return;
    }
    if (m_pTouches->containsObject(touch))
    {
        if (m_pTouches->count() == 1 && m_bTouchMoved)
        {
            this->schedule(schedule_selector(LGTableView::deaccelerateScrolling2));
        }
        m_pTouches->removeObject(touch);
    } 

    if (m_pTouches->count() == 0)
    {
        m_bDragging = false;
        m_bTouchMoved = false;
    }
}

void LGTableView::setContentOffset2(CCPoint offset, bool animated/*false*/)
{
    if (animated)
    { //animate scrolling
        this->setContentOffsetInDuration(offset, BOUNCE_DURATION);
    }
    else
    {
        //set the container position directly
        const CCPoint minOffset = this->minContainerOffset();
        const CCPoint maxOffset = this->maxContainerOffset();

        offset.x = MAX(minOffset.x, MIN(maxOffset.x, offset.x));
        offset.y = MAX(minOffset.y, MIN(maxOffset.y, offset.y));

        m_pContainer->setPosition(offset);

        if (m_pDelegate != NULL)
        {
            m_pDelegate->scrollViewDidScroll(this);   
        }
    }
}

void LGTableView::deaccelerateScrolling2(float dt)
{
    if (m_bDragging)
    {
        this->unschedule(schedule_selector(LGTableView::deaccelerateScrolling2));
        return;
    }

    float newX, newY;
    CCPoint maxInset, minInset;

    m_pContainer->setPosition(ccpAdd(m_pContainer->getPosition(), m_tScrollDistance));

    if (m_bBounceable)
    {
        maxInset = m_fMaxInset;
        minInset = m_fMinInset;
    }
    else
    {
        maxInset = this->maxContainerOffset();
        minInset = this->minContainerOffset();
    }

    //check to see if offset lies within the inset bounds
    newX     = MIN(m_pContainer->getPosition().x, maxInset.x);
    newX     = MAX(newX, minInset.x);
    newY     = MIN(m_pContainer->getPosition().y, maxInset.y);
    newY     = MAX(newY, minInset.y);

    newX = m_pContainer->getPosition().x;
    newY = m_pContainer->getPosition().y;

    m_tScrollDistance     = ccpSub(m_tScrollDistance, ccp(newX - m_pContainer->getPosition().x, newY - m_pContainer->getPosition().y));
    m_tScrollDistance     = ccpMult(m_tScrollDistance, SCROLL_DEACCEL_RATE);
    this->setContentOffset2(ccp(newX,newY));

    if ((fabsf(m_tScrollDistance.x) <= SCROLL_DEACCEL_DIST &&
        fabsf(m_tScrollDistance.y) <= SCROLL_DEACCEL_DIST) ||
        newY > maxInset.y || newY < minInset.y ||
        newX > maxInset.x || newX < minInset.x ||
        newX == maxInset.x || newX == minInset.x ||
        newY == maxInset.y || newY == minInset.y)
    {
        this->unschedule(schedule_selector(LGTableView::deaccelerateScrolling2));
        this->relocateContainer2(true);
    }
}

void LGTableView::relocateContainer2(bool animated)
{
    CCPoint oldPoint, min, max;
    float newX, newY;

    min = this->minContainerOffset();
    max = this->maxContainerOffset();

    oldPoint = m_pContainer->getPosition();

    newX     = oldPoint.x;
    newY     = oldPoint.y;
    if (m_eDirection == extension::kCCScrollViewDirectionBoth || m_eDirection == extension::kCCScrollViewDirectionHorizontal)
    {
        newX     = MAX(newX, min.x);
        newX     = MIN(newX, max.x);
    }

    if (m_eDirection == extension::kCCScrollViewDirectionBoth || m_eDirection == extension::kCCScrollViewDirectionVertical)
    {
        newY     = MIN(newY, max.y);
        newY     = MAX(newY, min.y);
    }

    if (newY != oldPoint.y || newX != oldPoint.x)
    {
        this->setContentOffset2(ccp(newX, newY), animated);
    }
}
