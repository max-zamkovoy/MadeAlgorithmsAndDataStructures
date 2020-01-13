#include <algorithm>
#include <assert.h>
#include <iostream>
#include <stack>
#include <vector>

using coord = double;

/* 2D point */
struct planePoint
{
    coord x;
    coord y;

    planePoint(coord _x, coord _y) :
        x(_x), y(_y)
    {
    }
};

constexpr std::size_t boolCalc(std::size_t searchDepth)
{
    return searchDepth < 1 ? 1 : 2 * boolCalc(searchDepth - 1);
}

constexpr std::size_t getBinCount(std::size_t searchDepth)
{
    return boolCalc(searchDepth);
}

constexpr std::size_t sizeOfScale(std::size_t searchDepth)
{
    return boolCalc(searchDepth) - 1;
}
template <std::size_t depthSearch>
struct treeInOrder {

    std::size_t ind[sizeOfScale(depthSearch)]; // the ind

    constexpr treeInOrder();
};
template <>
constexpr treeInOrder<1>::treeInOrder(): ind { 0 }
{
}
template <std::size_t depthSearch>
constexpr treeInOrder<depthSearch>::treeInOrder() : ind {}
{
    static_assert(depthSearch > 1);
    constexpr treeInOrder<depthSearch - 1> prev;
    const std::size_t expandNodeMask = 1 << (depthSearch - 2);

    for (std::size_t i = 0, j = 0; i < sizeOfScale(depthSearch - 1); ++i)
    {
        if ((prev.ind[i] + 1) & expandNodeMask)
        {
            ind[j++] = (prev.ind[i] + 1) * 2 - 1;
            ind[j++] = prev.ind[i];
            ind[j++] = (prev.ind[i] + 1) * 2;
        }
        else
        {
            ind[j++] = prev.ind[i];
        }
    }
}
template <std::size_t depthSearch, std::size_t TCoupledSearchDepth>
class GeoScale
{
public:
    using GeoHashChildScale = GeoScale<TCoupledSearchDepth, depthSearch>;
private:
    static constexpr treeInOrder<depthSearch> TREE_INORDER_INDEX = treeInOrder<depthSearch>();

    coord min_; 
    coord max_; 
    coord tips_[sizeOfScale(depthSearch)]; 
    GeoHashChildScale* inner_scales[getBinCount(depthSearch)]; // scales of the subranges
    void calcTips(coord start, coord end, std::size_t treeIndex);
    void createInnerScale(std::size_t ind);
public:
    GeoScale(coord min, coord max);
    GeoScale(const GeoScale&) = delete;
    GeoScale& operator=(const GeoScale&) = delete;
    GeoScale(GeoScale&&) = delete;
    GeoScale& operator=(GeoScale&&) = delete;
    ~GeoScale();
    std::size_t findBinIndex(coord coord) const;
    GeoHashChildScale* getChildScale(std::size_t ind);
    coord getMin() const
    {
        return min_;
    }
    coord getMax() const
    {
        return max_;
    }
};

/* Geohash cell, rectangular area, set of poins with the same geohash prefix */
template <std::size_t TXSearchDepth, std::size_t TYSearchDepth>
class GeoCell
{
public:
    using GeoScaleX = GeoScale<TXSearchDepth, TYSearchDepth>;
    using GeoScaleY = GeoScale<TYSearchDepth, TXSearchDepth>;
    using GeoChildCell = GeoCell<TYSearchDepth, TXSearchDepth>;
private:
    /* Determines the number of points that causes division of the cell */
    const std::size_t SPLIT_BIN_COUNT = 32;

    GeoScaleX* x_scale;      
    GeoScaleY* y_scale;      
    std::size_t count;      
    GeoChildCell* cells;
    std::vector<planePoint>* points; 
    void addNestedPoints(const planePoint& point);
    /* Create child cells, pass all points into them */
    void splitHashBin();
public:
    GeoCell();
    GeoCell(GeoScaleX* scaleX, GeoScaleY* scaleY);
    GeoCell(const GeoCell&) = delete;
    GeoCell& operator=(const GeoCell&) = delete;
    GeoCell(GeoCell&&) = delete;
    GeoCell& operator=(GeoCell&& other);
    ~GeoCell();
    /* Add a point into the cell */
    void addPointToPlane(const planePoint& point);
    /* Calculate points in the rectangle area */
    std::size_t countInRectangle(const planePoint& minplanePoint, const planePoint& maxplanePoint, const bool minXUnderflow, const bool maxXOverflow, const bool minYUnderflow, const bool maxYOverflow) const;
    std::size_t getCount() const
    {
        return count;
    }
};

class GeoHash
{
private:
    static const std::size_t X_DEPTH = 3; 
    static const std::size_t Y_DEPTH = 2; 
    const coord x_min;
    const coord x_max; 
    const coord y_min; 
    const coord y_max; 

    GeoScale<X_DEPTH, Y_DEPTH> xtips_; // root X coord scale
    GeoScale<Y_DEPTH, X_DEPTH> ytips_; // root Y coord scale
    GeoCell<X_DEPTH, Y_DEPTH> root_;   // the root cell, contains the whole addressed area

public:
    GeoHash(coord minX, coord maxX, coord minY, coord maxY);
    /* Add a point into the geohash */
    void addPointToPlane(const planePoint& point);
    /* Calculate points in the rectangle area */
    std::size_t countInRectangle(const planePoint& minplanePoint, const planePoint& maxplanePoint);
};

template<std::size_t depthSearch, std::size_t TComplementDepth>
void GeoScale<depthSearch, TComplementDepth>::calcTips(coord start, coord end, std::size_t treeIndex)
{
    const coord middle = start + (end - start) / 2;
    tips_[treeIndex] = middle;
    if (2 * treeIndex + 1 >= sizeOfScale(depthSearch))
        return;
    calcTips(start, middle, 2 * treeIndex + 1);
    calcTips(middle, end, 2 * treeIndex + 2);
}

template<std::size_t depthSearch, std::size_t TComplementDepth>
void GeoScale<depthSearch, TComplementDepth>::createInnerScale(std::size_t ind)
{
    assert(ind < getBinCount(depthSearch));
    coord min, max;
    if (!ind)
    {
        min = min_;
        max = tips_[TREE_INORDER_INDEX.ind[0]];
    }
    else if (ind == getBinCount(depthSearch) - 1)
    {
        min = tips_[TREE_INORDER_INDEX.ind[getBinCount(depthSearch) - 1]];
        max = max_;
    }
    else
    {
        min = tips_[TREE_INORDER_INDEX.ind[ind - 1]];
        max = tips_[TREE_INORDER_INDEX.ind[ind]];
    }
    assert(!inner_scales[ind]);
    inner_scales[ind] = new GeoHashChildScale(min, max);
}
template<std::size_t depthSearch, std::size_t TComplementDepth>
GeoScale<depthSearch, TComplementDepth>::GeoScale(coord min, coord max) :
    min_(min), max_(max)
{
    std::fill(inner_scales, inner_scales + getBinCount(depthSearch), nullptr);
    calcTips(min, max, 0);
}
template<std::size_t depthSearch, std::size_t TComplementDepth>
GeoScale<depthSearch, TComplementDepth>::~GeoScale()
{
    for (std::size_t i = 0; i < getBinCount(depthSearch); ++i)
    {
        delete inner_scales[i];
    }
}
template<std::size_t depthSearch, std::size_t TComplementDepth>
std::size_t GeoScale<depthSearch, TComplementDepth>::findBinIndex(coord coord) const
{
    assert(coord >= min_ && coord < max_);
    std::size_t ind = 0, result = 0;
    while (ind < sizeOfScale(depthSearch))
    {
        result <<= 1;
        if (tips_[ind] <= coord)
        {
            ind = 2 * ind + 2;
            ++result;
        }
        else
            ind = 2 * ind + 1;
    }
    assert(result < getBinCount(depthSearch));
    assert(!inner_scales[result] || inner_scales[result]->getMin() <= coord && inner_scales[result]->getMax() > coord);
    return result;
}
template<std::size_t depthSearch, std::size_t TComplementDepth>
typename GeoScale<depthSearch, TComplementDepth>::GeoHashChildScale* GeoScale<depthSearch, TComplementDepth>::getChildScale(std::size_t ind)
{
    if (!inner_scales[ind])
        createInnerScale(ind);
    return inner_scales[ind];
}

template <std::size_t TXSearchDepth, std::size_t TYSearchDepth>
void GeoCell<TXSearchDepth, TYSearchDepth>::addNestedPoints(const planePoint& point)
{
    assert(cells);
    std::size_t i = x_scale->findBinIndex(point.x);
    std::size_t j = y_scale->findBinIndex(point.y);
    cells[i * getBinCount(TYSearchDepth) + j].addPointToPlane(point);
}
template <std::size_t TXSearchDepth, std::size_t TYSearchDepth>
void GeoCell<TXSearchDepth, TYSearchDepth>::splitHashBin()
{
    assert(points && !cells);
    cells = new GeoChildCell[getBinCount(TXSearchDepth) * getBinCount(TYSearchDepth)];
    for (std::size_t i = 0; i < getBinCount(TXSearchDepth); ++i)
    {
        for (std::size_t j = 0; j < getBinCount(TYSearchDepth); ++j)
        {
            cells[i * getBinCount(TYSearchDepth) + j] = GeoChildCell(x_scale->getChildScale(i), y_scale->getChildScale(j));
        }
    }
    for (const planePoint& point : *points)
    {
        addNestedPoints(point);
    }
    delete points;
    points = nullptr;
}
template <std::size_t TXSearchDepth, std::size_t TYSearchDepth>
GeoCell<TXSearchDepth, TYSearchDepth>::GeoCell():
    cells(nullptr), points(nullptr)
{
}
template <std::size_t TXSearchDepth, std::size_t TYSearchDepth>
GeoCell<TXSearchDepth, TYSearchDepth>::GeoCell(GeoScaleX* scaleX, GeoScaleY* scaleY) :
    x_scale(scaleX), y_scale(scaleY), count(0), cells(nullptr), points(new std::vector<planePoint>)
{
    assert(scaleX && scaleY);
}
template <std::size_t TXSearchDepth, std::size_t TYSearchDepth>
GeoCell<TXSearchDepth, TYSearchDepth>& GeoCell<TXSearchDepth, TYSearchDepth>::operator=(GeoCell&& other)
{
    if (&other != this)
    {
        count = other.count;
        x_scale = other.x_scale;
        y_scale = other.y_scale;
        std::swap(cells, other.cells);
        std::swap(points, other.points);
    }
    return *this;
}
template <std::size_t TXSearchDepth, std::size_t TYSearchDepth>
GeoCell<TXSearchDepth, TYSearchDepth>::~GeoCell()
{
    delete[] cells;
    delete points;

}
template <std::size_t TXSearchDepth, std::size_t TYSearchDepth>
void GeoCell<TXSearchDepth, TYSearchDepth>::addPointToPlane(const planePoint& point)
{
    ++count;
    if (points)
    {
        points->push_back(point);
        if (count >= SPLIT_BIN_COUNT)
            splitHashBin();
    }
    else
        addNestedPoints(point);
}
template <std::size_t TXSearchDepth, std::size_t TYSearchDepth>
std::size_t GeoCell<TXSearchDepth, TYSearchDepth>::countInRectangle(const planePoint& minplanePoint, const planePoint& maxplanePoint,
    const bool minXUnderflow, const bool maxXOverflow, const bool minYUnderflow, const bool maxYOverflow) const
{
    assert (minplanePoint.x <= maxplanePoint.x && minplanePoint.y <= maxplanePoint.y);
    if (minXUnderflow && maxXOverflow && minYUnderflow && maxYOverflow)
    {
        return count;
    }

    std::size_t result = 0;

    if (points)
    {
        for (const planePoint& point : *points)
        {
            if (point.x >= minplanePoint.x && point.y >= minplanePoint.y && point.x < maxplanePoint.x && point.y < maxplanePoint.y )
            {
                ++result;
            }
        }
        return result;
    }

    std::size_t minXIndex = minXUnderflow ? 0 : x_scale->findBinIndex(minplanePoint.x);
    std::size_t maxXIndex = maxXOverflow ? getBinCount(TXSearchDepth) - 1 : x_scale->findBinIndex(maxplanePoint.x);
    std::size_t minYIndex = minYUnderflow ? 0 : y_scale->findBinIndex(minplanePoint.y);
    std::size_t maxYIndex = maxYOverflow ? getBinCount(TYSearchDepth) - 1: y_scale->findBinIndex(maxplanePoint.y);
    for (std::size_t i = minXIndex + 1; i < maxXIndex; ++i)
    {
        for (std::size_t j = minYIndex + 1; j < maxYIndex; ++j)
        {
            result += cells[i * getBinCount(TYSearchDepth) + j].getCount();
        }
    }
    // 1. edges
    if (minXIndex != maxXIndex)
    {
        for (std::size_t j = minYIndex + 1; j < maxYIndex; ++j) // 6
        {
            result += cells[minXIndex * getBinCount(TYSearchDepth) + j].countInRectangle(minplanePoint, maxplanePoint, minXUnderflow, true, true, true);
        }
        for (std::size_t j = minYIndex + 1; j < maxYIndex; ++j) // 2
        {
            result += cells[maxXIndex * getBinCount(TYSearchDepth) + j].countInRectangle(minplanePoint, maxplanePoint, true, maxXOverflow, true, true);
        }
    }
    else
    {
        for (std::size_t j = minYIndex + 1; j < maxYIndex; ++j) // 2, 6
        {
            result += cells[minXIndex * getBinCount(TYSearchDepth) + j].countInRectangle(minplanePoint, maxplanePoint, minXUnderflow, maxXOverflow, true, true);
        }
    }
    if (minYIndex != maxYIndex)
    {
        for (std::size_t i = minXIndex + 1; i < maxXIndex; ++i) // 0
        {
            result += cells[i * getBinCount(TYSearchDepth) + minYIndex].countInRectangle(minplanePoint, maxplanePoint, true, true, minYUnderflow, true);
        }
        for (std::size_t i = minXIndex + 1; i < maxXIndex; ++i) // 4
        {
            result += cells[i * getBinCount(TYSearchDepth) + maxYIndex].countInRectangle(minplanePoint, maxplanePoint, true, true, true, maxYOverflow);
        }
    }
    else
    {
        for (std::size_t i = minXIndex + 1; i < maxXIndex; ++i) // 0, 4
        {
            result += cells[i * getBinCount(TYSearchDepth) + minYIndex].countInRectangle(minplanePoint, maxplanePoint, true, true, minYUnderflow, maxYOverflow);
        }
    }
    // 2. vertices
    if (minXIndex != maxXIndex)
    {
        if (minYIndex != maxYIndex)
        {
            result += cells[minXIndex * getBinCount(TYSearchDepth) + minYIndex].countInRectangle(minplanePoint, maxplanePoint, minXUnderflow, true,         minYUnderflow, true        ); // 7
            result += cells[maxXIndex * getBinCount(TYSearchDepth) + minYIndex].countInRectangle(minplanePoint, maxplanePoint, true,          maxXOverflow, minYUnderflow, true        ); // 1
            result += cells[minXIndex * getBinCount(TYSearchDepth) + maxYIndex].countInRectangle(minplanePoint, maxplanePoint, minXUnderflow, true,         true,          maxYOverflow); // 5
            result += cells[maxXIndex * getBinCount(TYSearchDepth) + maxYIndex].countInRectangle(minplanePoint, maxplanePoint, true,          maxXOverflow, true,          maxYOverflow); // 3
        }
        else
        {
            result += cells[minXIndex * getBinCount(TYSearchDepth) + minYIndex].countInRectangle(minplanePoint, maxplanePoint, minXUnderflow, true,         minYUnderflow, maxYOverflow); // 5, 7
            result += cells[maxXIndex * getBinCount(TYSearchDepth) + minYIndex].countInRectangle(minplanePoint, maxplanePoint, true,          maxXOverflow, minYUnderflow, maxYOverflow); // 1, 3
        }
    }
    else
    {
        if (minYIndex != maxYIndex)
        {
            result += cells[minXIndex * getBinCount(TYSearchDepth) + minYIndex].countInRectangle(minplanePoint, maxplanePoint, minXUnderflow, maxXOverflow, minYUnderflow, true        ); // 1, 7
            result += cells[minXIndex * getBinCount(TYSearchDepth) + maxYIndex].countInRectangle(minplanePoint, maxplanePoint, minXUnderflow, maxXOverflow, true,          maxYOverflow); // 3, 5
        }
        else
        {
            result += cells[minXIndex * getBinCount(TYSearchDepth) + minYIndex].countInRectangle(minplanePoint, maxplanePoint, minXUnderflow, maxXOverflow, minYUnderflow, maxYOverflow); // 1, 3, 5, 7
        }
    }
    return result;
}

GeoHash::GeoHash(coord minX, coord maxX, coord minY, coord maxY) :
    x_min(minX), x_max(maxX), y_min(minY), y_max(maxY), xtips_(minX, maxX), ytips_(minY, maxY), root_(&xtips_, &ytips_)
{
}
void GeoHash::addPointToPlane(const planePoint& point)
{
    assert(point.x >= x_min && point.x < x_max && point.y >= y_min && point.y < y_max);
    root_.addPointToPlane(point);
}
std::size_t GeoHash::countInRectangle(const planePoint& minplanePoint, const planePoint& maxplanePoint)
{
     assert (minplanePoint.x <= maxplanePoint.x && minplanePoint.y <= maxplanePoint.y);
     if (minplanePoint.x >= x_max || minplanePoint.y >= y_max || maxplanePoint.x < x_min || maxplanePoint.y < y_min)
     {
         return 0;
     }
     bool minXUnderflow = minplanePoint.x < x_min;
     bool maxXOverflow  = maxplanePoint.x >= x_max;
     bool minYUnderflow = minplanePoint.y < y_min;
     bool maxYOverflow  = maxplanePoint.y >= y_max;
     if (minXUnderflow && maxXOverflow && minYUnderflow && maxYOverflow)
         return root_.getCount();
     else
        return root_.countInRectangle(minplanePoint, maxplanePoint, minXUnderflow, maxXOverflow, minYUnderflow, maxYOverflow);
}

int main(void)
{
    GeoHash geoHash(-180, 180, -90, 90);
    std::size_t pCount;
    std::cin >> pCount;
    for (std::size_t i = 0; i < pCount; ++i)
    {
        coord x, y;
        std::cin >> x >> y;
        geoHash.addPointToPlane(planePoint(x, y));
    }
    std::size_t rectangleCount;
    std::cin >> rectangleCount;
    for (std::size_t i = 0; i < rectangleCount; ++i)
    {
        coord xMin, xMax, yMin, yMax;
        std::cin >> xMin >> yMin >> xMax >> yMax;
        std::size_t pCount = geoHash.countInRectangle(planePoint(xMin, yMin), planePoint(xMax, yMax));
        std::cout << pCount << '\n';
    }
    return 0;
}