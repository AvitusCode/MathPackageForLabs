/*
 * SetControlBlockImpl.cpp
 *
 *  Created on: 10 сент. 2021 г.
 *      Author: Blasphemous
 */
#include "SetControlBlockImpl.h"

ILogger *SetControlBlockImpl::setblockLogger = nullptr;

SetControlBlockImpl::SetControlBlockImpl() : data(nullptr), dim(0), indexes(nullptr), size(0) {}
SetControlBlockImpl::SetControlBlockImpl(double *data, size_t *indx, size_t dim, size_t size) :
		data(data), dim(dim), indexes(indx), size(size) {}

RC SetControlBlockImpl::updateData(double *data, size_t *indx, size_t dim, size_t size) 
{
    this->data = data;
    this->dim = dim;
    this->indexes = indx;
    this->size = size;
    
    return RC::SUCCESS;
}

size_t SetControlBlockImpl::getIndex(size_t unic_index) const 
{
    size_t index = unic_index;
    while (true) 
    {
        if (indexes[index] == unic_index) // индекс найден
            break;
        
        if (index == size || index == 0)
            return size;
        
        index = indexes[unic_index] > unic_index ? index - 1 : index + 1;
    }
    
    return index;
}

RC SetControlBlockImpl::getNext(IVector *const &vec, size_t &index, size_t indexInc) const 
{
    if (!data || !indexes) 
    {
        if (setblockLogger)
        	setblockLogger->sever(RC::SOURCE_SET_DESTROYED, __FILE__, __func__, __LINE__);
        return RC::SOURCE_SET_DESTROYED;
    }
    
    if (!vec)
    {
    	if (setblockLogger)
    		setblockLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
    	return RC::NULLPTR_ERROR;
    }
    
    if (index + indexInc >= size) 
    {
        if (setblockLogger)
        	setblockLogger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        index = size;
        
        return RC::INDEX_OUT_OF_BOUND;
    }
    
    size_t indexNew = getIndex(index);
    
    if (indexNew == size) 
    {
        index = size;
        if (setblockLogger)
        	setblockLogger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }
    
    indexNew += indexInc;
    index = indexes[indexNew];
    
    RC msg = vec->setData(dim, data + indexNew * dim);
    if (msg != RC::SUCCESS)
    {
    	if (setblockLogger)
    		setblockLogger->sever(msg, __FILE__, __func__,__LINE__);
        return msg;
    }
    
    return RC::SUCCESS;
}

RC SetControlBlockImpl::getPrevious(IVector *const &vec, size_t &index, size_t indexInc) const 
{
    if (!data || !indexes) 
    {
        if (setblockLogger)
        	setblockLogger->sever(RC::SOURCE_SET_DESTROYED, __FILE__, __func__, __LINE__);
        return RC::SOURCE_SET_DESTROYED;
    }
    
    if (!vec)
    {
    	if (setblockLogger)
    		setblockLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
    		    	
    	return RC::NULLPTR_ERROR;
    }
    
    if (index - indexInc >= size) 
    {
    	index = size;
        if (setblockLogger)
        	setblockLogger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
       
        return RC::INDEX_OUT_OF_BOUND;
    }
    
    size_t indexNew = getIndex(index);
    
    if (indexNew == size) 
    {
        index = size;
        if (setblockLogger)
        	setblockLogger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }
    
    indexNew -= indexInc;
    index = indexes[indexNew];
    
    RC msg = vec->setData(dim, data + indexNew * dim);
    if (msg != RC::SUCCESS)
    {if (setblockLogger)
		setblockLogger->sever(msg, __FILE__, __func__,__LINE__);
        return msg;
    }
    
    return RC::SUCCESS;
}

RC SetControlBlockImpl::getBegin(IVector *const &vec, size_t &index) const 
{
    if (size == 0) 
    {
        if (setblockLogger)
        	setblockLogger->sever(RC::SOURCE_SET_EMPTY, __FILE__, __func__, __LINE__);
        return RC::SOURCE_SET_EMPTY;
    }
    
    if (!vec)
	{
		if (setblockLogger)
			setblockLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__,__LINE__);

		return RC::NULLPTR_ERROR;
	}
    index = indexes[0];
    
    
    RC msg = vec->setData(dim, data);
    if (msg != RC::SUCCESS)
    {
    	if (setblockLogger)
    		setblockLogger->sever(msg, __FILE__, __func__,__LINE__);
         return msg;
    }
  
    return RC::SUCCESS;
}

RC SetControlBlockImpl::getEnd(IVector *const &vec, size_t &index) const {
    if (size == 0) 
    {
        if (setblockLogger)
        	setblockLogger->sever(RC::SOURCE_SET_EMPTY, __FILE__, __func__, __LINE__);
        return RC::SOURCE_SET_EMPTY;
    }
    
    if (!vec)
   	{
   		if (setblockLogger)
   			setblockLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__,__LINE__);

   		return RC::NULLPTR_ERROR;
   	}
    
    index = indexes[size - 1];
  
    RC msg = vec->setData(dim, data + (size - 1) * dim);
    if (msg != RC::SUCCESS)
    {
    	if (setblockLogger)
    		setblockLogger->sever(msg, __FILE__, __func__, __LINE__);
        return msg;
    }

    return RC::SUCCESS;
}

ISetControlBlock::~ISetControlBlock() noexcept {}



