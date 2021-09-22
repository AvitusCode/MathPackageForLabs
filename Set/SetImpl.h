#pragma once

#include "ISet.h"
#include "../Vector/IVector.h"
#include "../Logger/ILogger.h"
#include "SetControlBlockImpl.h"

#include <memory>

class SetImpl : public ISet {
public:
    SetImpl();

    static RC setLogger(ILogger *const logger){
    	if (!logger){
    		return RC::NULLPTR_ERROR;
    	}
    	
    	sLogger = logger;
    	return RC::SUCCESS;
    }
    static ILogger *const getLogger(){
    	return sLogger;
    }

    ISet *clone() const override;
    size_t getDim() const override;
    size_t getSize() const override;

    /*
     * Method creating new IVector and assigning new address to val
     */
    RC getCopy(size_t index, IVector *&val) const override;

    RC findFirstAndCopy(IVector const *const &pat, IVector::NORM n, double tol, IVector *&val) const override;

    /*
     * Method copy data from vector in ISet to vector val
     */
    RC getCoords(size_t index, IVector *const &val) const override;

    RC findFirstAndCopyCoords(IVector const *const &pat, IVector::NORM n, double tol, IVector *const &val) const override;

    RC findFirst(IVector const *const &pat, IVector::NORM n, double tol) const override;

    RC insert(IVector const *const &val, IVector::NORM n, double tol) override;

    RC remove(size_t index) override;

    RC remove(IVector const *const &pat, IVector::NORM n, double tol) override;

    class Iterator : public IIterator
    {
    public:
    	Iterator() noexcept;
    	static ISet::IIterator* makeIterator(std::shared_ptr<ISetControlBlock> cBlock, size_t index, size_t dim);
		IIterator* getNext(size_t indexInc = 1) const override;
		IIterator* getPrevious(size_t indexInc = 1) const override;
		IIterator* clone() const override;

		static RC setLogger(ILogger *const pLogger){
			if (!pLogger){
				return RC::NULLPTR_ERROR;
			}
			iterLogger = pLogger;
			return RC::SUCCESS;
		}
		RC next(size_t indexInc = 1) override;
		RC previous(size_t indexInc = 1) override;

		bool isValid() const override;
		RC makeBegin() override;
		RC makeEnd() override;

		RC getVectorCopy(IVector *&val) const override;
		RC getVectorCoords(IVector *const&val) const override;
		~Iterator() noexcept;
    private:
        static ILogger* iterLogger;
        std::shared_ptr<ISetControlBlock> controlBlock; //контрольных блок
        IVector* buffer;                                //буфферный вектор
        
        size_t unic_index;                              //уникальный индекс вектора
        size_t dim;                                     //размерность вектора
        
        Iterator(const Iterator&) = delete;
        Iterator& operator=(const Iterator&) = delete;
    };
    IIterator *getIterator(size_t index) const override;
    IIterator *getBegin() const override;
    IIterator *getEnd() const override;

    ~SetImpl();
    
private:
    static const size_t MAX_ALLOC = 100;   //кол-во векторов для изначального выделения
    static ILogger *sLogger;

    double *data;       //массив данных векторов
    size_t *indexes;    //массив уникальных индексов векторов
    
    size_t lastIndex;   //индекс последнего добавленного вектора, будем поддерживать уникальность всех индексов (присваение по возрст)
    size_t dim;         //размерность векторов
    size_t size;        //кол-во активных векторов
    size_t alloc;       //кол-во выделенной памяти под вектора
    std::shared_ptr<SetControlBlockImpl> cBlock;

    RC insertFirst(const IVector *const &val);
    RC addVec(const IVector *const &val, IVector::NORM n, double tol);
    
    SetImpl(const SetImpl &other) = delete;
    SetImpl &operator=(const SetImpl &other) = delete;
};
