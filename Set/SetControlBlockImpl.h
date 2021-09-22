#pragma once
#include "ISetControlBlock.h"

class SetControlBlockImpl : public ISetControlBlock 
{
public:
	/*
	 * data - данные
	 * indx - ун. индексы
	 * 
	 * */
	SetControlBlockImpl();
    SetControlBlockImpl(double *data, size_t *indx, size_t dim, size_t size);

    RC updateData(double *data, size_t *indx, size_t dim, size_t size);
    
    static RC setLogger(ILogger *const logger){
    	if (!logger)
    		return RC::NULLPTR_ERROR;
    	setblockLogger = logger;
    	return RC::SUCCESS;
    }
    
    RC getNext(IVector *const &vec, size_t &index, size_t indexInc = 1) const override;
    RC getPrevious(IVector *const &vec, size_t &index, size_t indexInc = 1) const override;
    RC getBegin(IVector *const &vec, size_t &index) const override;
    RC getEnd(IVector *const &vec, size_t &index) const override;
    
private:
    static ILogger *setblockLogger;
    double *data;
    size_t dim;
    
    size_t *indexes;
    size_t size;

    size_t getIndex(size_t unic_index) const; //находит уникальный индекс
};
