#pragma once
#include "ICompact.h"
#include "CompactControlBlockImpl.h"
#include <memory>

class CompactImpl : public ICompact 
{
  public:
	CompactImpl() = delete;
	CompactImpl(const IVector *left, const IVector *right, const IMultiIndex *nodes);
    ICompact *clone() const override;

    static RC setLogger(ILogger *const logger){
    	if (!logger)
    		return RC::NULLPTR_ERROR;
    	cLogger = logger;
    	return RC::SUCCESS;
    }
    static ILogger *getLogger(){
    	return cLogger;
    }

    bool isInside(IVector const *const &vec) const override;
    RC getVectorCopy(IMultiIndex const *index, IVector *&val) const override;
    RC getVectorCoords(IMultiIndex const *index, IVector *const &val) const override;

    RC getLeftBoundary(IVector *&vec) const override;
    RC getRightBoundary(IVector *&vec) const override;
    size_t getDim() const override;
    IMultiIndex *getGrid() const override;

    class Iterator : public IIterator {
      public:
    	Iterator() noexcept;
    	
        static IIterator* makeIterator(std::shared_ptr<ICompactControlBlock> cb, IVector *vec, IMultiIndex *idx, IMultiIndex *ord);
        IIterator *getNext() override;
        IIterator *clone() const override;

        static RC setLogger(ILogger *const pLogger){
        	if (!pLogger)
        		return RC::NULLPTR_ERROR;
        	iterLogger = pLogger;
        	return RC::SUCCESS;
        }
        static ILogger *getLogger(){
        	return iterLogger;
        }

        RC next() override;

        bool isValid() const override;

        RC getVectorCopy(IVector *&val) const override;
        RC getVectorCoords(IVector *const &val) const override;
        
       ~Iterator() noexcept;
      private:
        static ILogger *iterLogger;
        
        std::shared_ptr<ICompactControlBlock> controlBlock;
        IVector *vector;
        IMultiIndex *index;
        IMultiIndex *order;
        bool descriptor = true;
        
        Iterator(const Iterator&) = delete;
        Iterator& operator=(const Iterator&) = delete;
    };

    IIterator *getIterator(IMultiIndex const *const &index, IMultiIndex const *const &bypassOrder) const override;
    IIterator *getBegin(IMultiIndex const *const &bypassOrder) const override;
    IIterator *getEnd(IMultiIndex const *const &bypassOrder) const override;

    // Доработать
    ~CompactImpl() = default;

  private:
    static ILogger *cLogger;
    
    const IVector *leftBorder;
    const IVector *rightBorder;
    const IMultiIndex *grid;
    
    size_t dim;
    std::shared_ptr<ICompactControlBlock> cBlock;

};
