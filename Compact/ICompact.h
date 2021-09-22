#pragma once
#include "../Logger/ILogger.h"
#include "../Vector/IVector.h"
#include "../RC.h"
#include "../MultiIndex/IMultiIndex.h"


class ICompact {
public:
    static ICompact* createCompact(IVector const * vec1, IVector const * vec2, IMultiIndex const *nodeQuantities);

    virtual ICompact *clone() const = 0;

    static RC setLogger(ILogger* const logger);
    static ILogger* getLogger();

    virtual bool isInside(IVector const * const&vec) const = 0;
    /*
    * Method creating new IVector and assigning new address to val
    */
    virtual RC getVectorCopy(IMultiIndex const *index, IVector *& val) const = 0;
    /*
    * Method copy data from vector in ISet to vector val
    */
    virtual RC getVectorCoords(IMultiIndex const *index, IVector * const& val) const = 0;

    // �������� �� ���� �����������
    virtual RC getLeftBoundary(IVector *& vec) const = 0;
    // ��������� �� ���� �����������
    virtual RC getRightBoundary(IVector *& vec) const = 0;
    virtual size_t getDim() const = 0;
    virtual IMultiIndex* getGrid() const = 0;

    //  grid ������������ ��� ������� ����� �� ������������ �����������
    static ICompact* createIntersection(ICompact const *op1, ICompact const *op2, IMultiIndex const* const grid, double tol);
    /* CompactSpan - ���������� ��������: ������ ���������� ���������� ���������, ���������� 2 ���������� */
    static ICompact* createCompactSpan(ICompact const *op1, ICompact const *op2, IMultiIndex const* const grid);

    class IIterator {
    public:
        virtual IIterator * getNext() = 0;
        virtual IIterator * clone() const = 0;

        static RC setLogger(ILogger * const pLogger);
        static ILogger* getLogger();

        virtual RC next() = 0;
        
        /*
        * Iterator is invalid, if it was moved forward, when iterator wasn't able to move
        */
        virtual bool isValid() const = 0;

        /*
        * Method creating new IVector and assigning new address to val
        */
        virtual RC getVectorCopy(IVector *& val) const = 0;
        /*
        * Method copy data from vector in ISet to vector val
        */
        virtual RC getVectorCoords(IVector * const& val) const = 0;

        virtual ~IIterator() = 0;

    private:
        IIterator(const IIterator&) = delete;
        IIterator& operator=(const IIterator&) = delete;

    protected:
        IIterator() = default;
    };

    virtual IIterator* getIterator(IMultiIndex const * const&index, IMultiIndex const * const &bypassOrder) const = 0;
    // ���������� �������� �� �������� �������
    virtual IIterator* getBegin(IMultiIndex const * const &bypassOrder) const = 0;
    // ���������� �������� �� ��������� �������
    virtual IIterator* getEnd(IMultiIndex const * const &bypassOrder) const = 0;
    
    virtual ~ICompact() = 0;

private:
    ICompact(const ICompact& compact) = delete;
    ICompact& operator=(const ICompact& compact) = delete;

protected:
    ICompact() = default;
};
