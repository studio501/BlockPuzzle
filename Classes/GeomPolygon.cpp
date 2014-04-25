#include "LayoutManager.h"
#include "TangramScene.h"
#include "GeomPolygon.h"

#include <vector>
#include <algorithm>

USING_NS_CC;

const int MAX_GEOMPOLYGON = 20;
const int DEFAULT_BLOCK_CAPACITY = 10;

PGeomPolygonData GeomPolygon::s_pData = NULL;
int GeomPolygon::s_DataSize = 0;;

void GeomPolygon::loadPolygonData()
{
    //GeomPolygonData extra[] = {
    //    {0,14,0,1,0,2,0,3,1,0,1,1,2,0,2,1},
    //    {0,6,0,1,1,0,1,1},
    //    {0,10,0,1,1,0,1,1,2,0,2,1},
    //    {0,10,0,1,0,2,0,3,1,2,1,3},
    //    {0,10,0,1,1,0,1,1,1,2,1,3},
    //    {0,12,0,1,1,0,1,1,2,0,2,1,2,2},
    //    {0,8,0,1,1,1,1,2,1,3},
    //    {0,12,0,1,1,0,1,1,1,2,2,0,2,1},
    //    {0,12,0,1,1,1,2,0,2,1,2,2,2,3},
    //    {0,12,0,1,0,2,1,0,1,1,1,2,2,2}
    //};
    //FILE *fp = fopen("D:\\workspace\\Git\\cocos2d-x\\projects\\blockpuzzle\\Resources\\tb", "ab+");
    //if (fp)
    //{
    //    fwrite(extra, sizeof(extra), 1, fp);
    //    fclose(fp);
    //}

    unsigned long size = 0;
    s_pData = (PGeomPolygonData)CCFileUtils::sharedFileUtils()->getFileData("tb", "rb", &size);
    s_DataSize = size / sizeof(GeomPolygonData);
}

GeomPolygon *GeomPolygon::getGeomPolygon(int id, int tile)
{
    CCLOG("GeomPolygon::getGeomPolygon id:%d", id);
    if (s_pData == NULL)
    {
        loadPolygonData();
    }

    GeomPolygon *polygon = new GeomPolygon();
    if (polygon && polygon->initWithIdAndTile(id, tile))
    {
        polygon->autorelease();
        return polygon;
    }

    CC_SAFE_DELETE(polygon);
    return NULL;
}

GeomPolygon::GeomPolygon()
    :m_Id(-1)
    ,m_Tile(-1)
    ,m_HorizontalBlockCount(0)
    ,m_VerticalBlockCount(0)
    ,m_pBlocks(NULL)
    ,m_State(kPolygonStateUngrabbed)
    ,m_OnGeomPolygonListener(NULL)
{
    m_GPosition.setPoint(-1, -1);
}

GeomPolygon::~GeomPolygon(void)
{
    CCLOG("GeomPolygon::~GeomPolygon: %d", m_Id);
}

bool GeomPolygon::initWithIdAndTile(int id, int tile)
{
    CCLOG("GeomPolygon::initWithId: %d", id);
    initWithFile("tiles.png", DEFAULT_BLOCK_CAPACITY);

    if (s_pData != NULL && id >=0  && id < s_DataSize)
    {
        GeomPolygonData data;
        memcpy(&data, s_pData + id, sizeof(GeomPolygonData));
        m_Id = id;
        m_Tile = tile;

        m_BlockCount = data.coordinateSize / 2 + 1;
        m_pBlocks = new int[m_BlockCount + 1];

        int minX = 0, minY = 0;
        int maxX = 0, maxY = 0;
        for (unsigned int i = 0; i < data.coordinateSize; i += 2)
        {
            if (minX > data.coordinates[i])
            {
                minX = data.coordinates[i];
            }
            if (maxX < data.coordinates[i])
            {
                maxX = data.coordinates[i];
            }

            if (minY > data.coordinates[i + 1])
            {
                minY = data.coordinates[i + 1];
            }
            if (maxY < data.coordinates[i + 1])
            {
                maxY = data.coordinates[i + 1];
            }
        }
        m_HorizontalBlockCount = maxX - minX + 1;
        m_VerticalBlockCount = maxY - minY + 1;
        // flip vertical
        for (unsigned int i = 0; i < data.coordinateSize; i += 2)
        {
            data.coordinates[i] = data.coordinates[i] - minX;
            data.coordinates[i + 1] = m_VerticalBlockCount - 1 - (data.coordinates[i + 1] - minY);
        }
        data.coordinates[data.coordinateSize] = -minX;
        data.coordinates[data.coordinateSize + 1] = m_VerticalBlockCount - 1 + minY;

        CCSize blockSize = LayoutManager::getInstance()->getBlockSize();
        setContentSize(CCSizeMake(m_HorizontalBlockCount * blockSize.width, m_VerticalBlockCount * blockSize.height));

        const CCSize singleTextureSize = CCSizeMake(64, 64);
        float scale = blockSize.width / singleTextureSize.width;

        for (int i = 0; i < m_BlockCount; i++)
        {
            m_pBlocks[i] = (data.coordinates[i * 2 + 1]) * m_HorizontalBlockCount + data.coordinates[i * 2];

            // boarder
            int x = m_pBlocks[i];
            if (m_HorizontalBorderLines.find(x) == m_HorizontalBorderLines.end())
            {
                m_HorizontalBorderLines.insert(x);
            }
            else
            {
                m_HorizontalBorderLines.erase(x);
            }
            x = x + m_HorizontalBlockCount;
            if (m_HorizontalBorderLines.find(x) == m_HorizontalBorderLines.end())
            {
                m_HorizontalBorderLines.insert(x);
            }
            else
            {
                m_HorizontalBorderLines.erase(x);
            }

            int y = m_pBlocks[i] / m_HorizontalBlockCount + (m_pBlocks[i] % m_HorizontalBlockCount) * m_VerticalBlockCount;
            if (m_VerticalBorderLines.find(y) == m_VerticalBorderLines.end())
            {
                m_VerticalBorderLines.insert(y);
            }
            else
            {
                m_VerticalBorderLines.erase(y);
            }
            y = y + m_VerticalBlockCount;
            if (m_VerticalBorderLines.find(y) == m_VerticalBorderLines.end())
            {
                m_VerticalBorderLines.insert(y);
            }
            else
            {
                m_VerticalBorderLines.erase(y);
            }

            CCSprite *sprite = CCSprite::createWithTexture(getTexture(), CCRectMake((m_Tile % 12) * singleTextureSize.width, 0, singleTextureSize.width, singleTextureSize.height));
            sprite->setScale(scale);
            sprite->setPosition(ccp((m_pBlocks[i] % m_HorizontalBlockCount + 0.5f) * blockSize.width, (m_pBlocks[i] / m_HorizontalBlockCount + 0.5f) * blockSize.height));
            addChild(sprite);
        }

        return true;
    }
    else
    {
        return false;
    }
}

void GeomPolygon::onEnter()
{
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    CCSpriteBatchNode::onEnter();
}

void GeomPolygon::onExit()
{
    CCDirector *pDirector = CCDirector::sharedDirector();
    pDirector->getTouchDispatcher()->removeDelegate(this);
    CCSpriteBatchNode::onExit();
}    

void GeomPolygon::setGPosition(const CCPoint &gpos)
{
    m_LastGPosition = m_GPosition;
    m_GPosition = gpos;
}

bool GeomPolygon::intersect(const CCPoint &pNewGpos, GeomPolygon *pThat)
{
    CCRect thisRect = CCRectMake(pNewGpos.x, pNewGpos.y, m_HorizontalBlockCount, m_VerticalBlockCount);
    CCRect thatRect = CCRectMake(pThat->m_GPosition.x, pThat->m_GPosition.y, pThat->m_HorizontalBlockCount, pThat->m_VerticalBlockCount);

    if (thisRect.getMaxX() <= thatRect.getMinX() || thatRect.getMaxX() <= thisRect.getMinX() || thisRect.getMaxY() <= thatRect.getMinY() || thatRect.getMaxY() <= thisRect.getMinY())
    {
        return false;
    }

    int x0 = MIN(thisRect.getMinX(), thatRect.getMinX());
    int y0 = MIN(thisRect.getMinY(), thatRect.getMinY());
    int w = MAX(thisRect.getMaxX(), thatRect.getMaxX()) - x0;

    int x, y;
    std::vector<int> blocks;
    for (int i = 0; i < pThat->m_BlockCount; i++)
    {
        x  = pThat->m_pBlocks[i] % pThat->m_HorizontalBlockCount + (pThat->m_GPosition.x - x0);
        y = pThat->m_pBlocks[i] / pThat->m_HorizontalBlockCount + (pThat->m_GPosition.y - y0);
        blocks.push_back(x + y * w);
    }
    std::sort(blocks.begin(), blocks.end());

    for (int i = 0; i < m_BlockCount; i++)
    {
        x  = m_pBlocks[i] % m_HorizontalBlockCount + (pNewGpos.x - x0);
        y = m_pBlocks[i] / m_HorizontalBlockCount + (pNewGpos.y - y0);
        if (std::binary_search(blocks.begin(), blocks.end(), x + y * w))
        {
            return true;
        }
    }

    return false;
}

bool GeomPolygon::containsTouchLocation(CCTouch *touch)
{
    CCPoint local = convertTouchToNodeSpace(touch);
    //CCLOG("touch:(%f, %f), local:(%f, %f)", touch->getLocation().x, touch->getLocation().y, local.x, local.y);

    CCObject *pChild;
    CCARRAY_FOREACH(getChildren(), pChild)
    {
        CCSprite *pSprite = dynamic_cast<CCSprite *>(pChild);
        CCRect rect = pSprite->boundingBox();
        if (rect.containsPoint(local))
        {
            return true;
        }
    }
    return false;
}

void GeomPolygon::setOnGeomPolygonListener(IOnGeomPolygonListener *listener)
{
    m_OnGeomPolygonListener = listener;
}

// draw
void GeomPolygon::draw(void)
{
    CCSpriteBatchNode::draw();

    // draw boarder
    int lineWidth = LayoutManager::getInstance()->getLineWidth();
    CCSize blockSize = LayoutManager::getInstance()->getBlockSize();
    ccDrawColor4B(200, 200, 200, 255);
    glLineWidth(lineWidth);
    for (std::set<int>::iterator iter = m_HorizontalBorderLines.begin(); iter != m_HorizontalBorderLines.end(); iter++)
    {
        int startX = (*iter % m_HorizontalBlockCount) * blockSize.width;
        int startY = (*iter / m_HorizontalBlockCount) * blockSize.height;
        ccDrawLine(ccp(startX - lineWidth / 2, startY), ccp(startX + blockSize.width + lineWidth / 2, startY));
    }
    for (std::set<int>::iterator iter = m_VerticalBorderLines.begin(); iter != m_VerticalBorderLines.end(); iter++)
    {
        int startX = (*iter / m_VerticalBlockCount) * blockSize.width;
        int startY = (*iter % m_VerticalBlockCount) * blockSize.height;
        ccDrawLine(ccp(startX, startY - lineWidth / 2), ccp(startX, startY + blockSize.height + lineWidth / 2));
    }

    if (m_State == kPolygonStateGrabbed)
    {
        for (int i = 0; i < m_BlockCount; i++)
        {
            CCPoint start = CCPointMake((m_pBlocks[i] % m_HorizontalBlockCount) * blockSize.width, (m_pBlocks[i] / m_HorizontalBlockCount) * blockSize.height);
            ccDrawSolidRect(start, start + blockSize, ccc4f(1.0f, 1.0f, 1.0f, 0.2f));
        }
    }
}

bool GeomPolygon::ccTouchBegan(CCTouch *touch, CCEvent *event)
{
    if (m_State != kPolygonStateUngrabbed)
        return false;
    if (!containsTouchLocation(touch))
        return false;

    m_State = kPolygonStateGrabbed;
    m_TouchBeganPos = getPosition();

    if (m_OnGeomPolygonListener)
    {
        m_OnGeomPolygonListener->onDragged(this, touch);
    }

    return true;
}

void GeomPolygon::ccTouchMoved(CCTouch *touch, CCEvent *event)
{
    // If it weren't for the TouchDispatcher, you would need to keep a reference
    // to the touch from touchBegan and check that the current touch is the same
    // as that one.
    // Actually, it would be even more complicated since in the Cocos dispatcher
    // you get CCSets instead of 1 UITouch, so you'd need to loop through the set
    // in each touchXXX method.

    CCAssert(m_State == kPolygonStateGrabbed, "GeomPolygon - Unexpected state!");

    if (m_OnGeomPolygonListener)
    {
        m_OnGeomPolygonListener->onMove(this, touch);
    }
}

void GeomPolygon::ccTouchEnded(CCTouch *touch, CCEvent *event)
{
    CCAssert(m_State == kPolygonStateGrabbed, "GeomPolygon - Unexpected state!");    

    m_State = kPolygonStateUngrabbed;

    if (m_OnGeomPolygonListener)
    {
        m_OnGeomPolygonListener->onDrop(this, touch);
    }
}