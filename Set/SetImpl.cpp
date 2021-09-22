#include "SetImpl.h"
#include "../support_func.h"
#include <cmath>

// Set implementation
ILogger *SetImpl::sLogger = nullptr;

SetImpl::SetImpl() : data(nullptr), indexes(nullptr), lastIndex(0), dim(0), size(0), alloc(0) 
{
    cBlock = std::make_shared<SetControlBlockImpl>(data, indexes, dim, size);
}

ISet *SetImpl::clone() const 
{
    SetImpl *Set = new SetImpl();
    
    if (!Set) 
    {
        if (sLogger)
        	sLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    
    Set->data = new double[alloc * dim];
    
    if (!Set->data) 
    {
        if (sLogger)
        	sLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        delete Set;
        return nullptr;
    }
    Set->indexes = new size_t[alloc];
    
    if (!Set->indexes) 
    {
        if (sLogger)
        	sLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        delete Set;
        return nullptr;
    }

    // Now we are copying data
	Set->lastIndex = lastIndex;
	Set->dim = dim;
	Set->size = size;
	Set->alloc = alloc;
    memcpy(Set->data, data, dim * size * sizeof(double));
    memcpy(Set->indexes, indexes, size * sizeof(size_t));

    return Set;
}

size_t SetImpl::getDim() const {
    return dim;
}

size_t SetImpl::getSize() const {
    return size;
}

RC SetImpl::getCopy(size_t index, IVector *&val) const 
{
    if (index >= size) 
    {
    	if (sLogger)
    	    sLogger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }
    
    IVector *vector = IVector::createVector(dim, data + index * dim);
    
    if (!vector) 
    {
        if (sLogger)
        	sLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }
    
    val = vector;
    return RC::SUCCESS;
}

RC SetImpl::findFirstAndCopy(const IVector *const &pat, IVector::NORM n, double tol, IVector *&val) const 
{
    if (!pat) 
    {
        if (sLogger)
        	sLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    
    if (pat->getDim() != dim) 
    {
        if (sLogger)
        	sLogger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    
    if (n == IVector::NORM::AMOUNT || tol < 0.0) 
    {
        if (sLogger)
        	sLogger->sever(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }
    
    if (std::isnan(tol)) 
    {
        if (sLogger)
        	sLogger->sever(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
        return RC::NOT_NUMBER;
    }
    
    if (std::isinf(tol)) 
    {
        if (sLogger)
        	sLogger->sever(RC::INFINITY_OVERFLOW, __FILE__, __func__, __LINE__);
        return RC::INFINITY_OVERFLOW;
    }

    IVector *buffer = IVector::createVector(dim, data);
    
    if (!buffer) 
    {
        if (sLogger)
        	sLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }
    
    if (IVector::equals(buffer, pat, n, tol)) 
    {
        val = buffer;
        if (sLogger)
        	
        return RC::SUCCESS;
    }

    RC msg = RC::SUCCESS;
    
    for (size_t i = 1; i < size; i++) 
    {
        msg = buffer->setData(dim, data + i * dim);
        
        if (msg != RC::SUCCESS) 
        {
            delete buffer;
            return msg;
        }
        
        if (IVector::equals(buffer, pat, n, tol)) 
        {
            val = buffer;
            return RC::SUCCESS;
        }
    }

    delete buffer;
    return RC::VECTOR_NOT_FOUND;
}

RC SetImpl::getCoords(size_t index, IVector *const &val) const 
{
    if (!val) 
    {
        if (sLogger)
        	sLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    
    if (index >= size) 
    {
        if (sLogger)
        	sLogger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }

    RC msg = val->setData(dim, data + index * dim);
    
    if (msg != RC::SUCCESS)
        return msg;

    return RC::SUCCESS;
}

RC SetImpl::findFirstAndCopyCoords(const IVector *const &pat, IVector::NORM n, double tol, IVector *const &val) const 
{
    if (!val || !pat) 
    {
        if (sLogger)
        	sLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    
    if (n == IVector::NORM::AMOUNT || tol < 0.0) 
    {
        if (sLogger)
        	sLogger->sever(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }
    
    if (std::isnan(tol)) 
    {
        if (sLogger)
        	sLogger->sever(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
        return RC::NOT_NUMBER;
    }
    
    if (std::isinf(tol)) 
    {
        if (sLogger)
        	sLogger->sever(RC::INFINITY_OVERFLOW, __FILE__, __func__, __LINE__);
        return RC::INFINITY_OVERFLOW;
    }
    
    IVector *buffer = nullptr;
    
    RC msg = findFirstAndCopy(pat, n, tol, buffer);
    if (msg != RC::SUCCESS)
        return msg;
    
    val->setData(buffer->getDim(), buffer->getData());
    
    delete buffer;
    return RC::SUCCESS;
}

RC SetImpl::findFirst(const IVector *const &pat, IVector::NORM n, double tol) const 
{
    IVector *buffer = nullptr;
    RC msg = findFirstAndCopy(pat, n, tol, buffer);
    
    if (msg != RC::SUCCESS)
        return msg;
    
    delete buffer;
    return RC::SUCCESS;
}

RC SetImpl::insertFirst(const IVector *const &val) 
{
    data = new double[MAX_ALLOC * val->getDim()];
    
    if (!data) 
    {
        if (sLogger)
        	sLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }
    
    indexes = new size_t[MAX_ALLOC];
    
    if (!indexes) 
    {
        delete data;
        data = nullptr;
        
        if (sLogger)
        	sLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }
    
    support::fill(indexes, indexes + MAX_ALLOC * sizeof(size_t), 0);
    lastIndex = 0;
    dim = val->getDim();
    size = 1;
    alloc = MAX_ALLOC;
    
    memcpy(data, val->getData(), dim * sizeof(double));
    return RC::SUCCESS;
}

RC SetImpl::addVec(const IVector *const &val, IVector::NORM n, double tol) 
{
    if (val->getDim() != dim) 
    {
        if (sLogger)
        	sLogger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    
    RC msg = findFirst(val, n, tol);
    
    if (msg != RC::VECTOR_NOT_FOUND) 
    {
        msg = msg == RC::SUCCESS ? RC::VECTOR_ALREADY_EXIST : msg;
        
        if (sLogger)
        	sLogger->sever(msg, __FILE__, __func__, __LINE__);
        return msg;
    }

    if (size == alloc) 
    {
        double *newData = new double[2 * alloc * dim];
        
        if (!newData) 
        {
            if (sLogger)
            	sLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
            return RC::ALLOCATION_ERROR;
        }
        
        size_t *newIndexes = new size_t[2 * alloc];
        
        if (!newIndexes) 
        {
            delete newData;
            if (sLogger)
            	sLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
            return RC::ALLOCATION_ERROR;
        }

        memcpy(newData, data, size * dim * sizeof(double));
        memcpy(newIndexes, indexes, size * sizeof(size_t));

        delete[] data;
        delete[] indexes;

        data = newData;
        indexes = newIndexes;

        alloc *= 2;
    }

    memcpy(data + size * dim, val->getData(), dim * sizeof(double));
    indexes[size++] = ++lastIndex;
    
    return RC::SUCCESS;
}

RC SetImpl::insert(const IVector *const &val, IVector::NORM n, double tol) 
{
    if (!val) 
    {
        if (sLogger)
        	sLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    
    if (n == IVector::NORM::AMOUNT || tol < 0.0) 
    {
        if (sLogger)
        	sLogger->sever(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }
    
    if (std::isnan(tol)) 
    {
        if (sLogger)
        	sLogger->sever(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
        return RC::NOT_NUMBER;
    }
    
    if (std::isinf(tol)) 
    {
        if (sLogger)
        	sLogger->sever(RC::INFINITY_OVERFLOW, __FILE__, __func__, __LINE__);
        return RC::INFINITY_OVERFLOW;
    }
    
    
    RC msg = RC::SUCCESS;
    msg = size == 0 ? insertFirst(val) : addVec(val, n, tol);
    cBlock->updateData(data, indexes, dim, size);
    return msg;
}

RC SetImpl::remove(size_t index) 
{
    if (index >= size) 
    {
        if (sLogger)
        	sLogger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
        return RC::INDEX_OUT_OF_BOUND;
    }
    
    if (index != size - 1) 
    {
        memcpy(data + index * dim, data + (index + 1) * dim, dim * (size - index - 1) * sizeof(double));
        memcpy(indexes + index, data + index + 1, (size - index - 1) * sizeof(size_t));
    }
    
    size--;
    cBlock->updateData(data, indexes, dim, size);
  
    return RC::SUCCESS;
}

RC SetImpl::remove(const IVector *const &pat, IVector::NORM n, double tol) 
{
    if (pat == nullptr) 
    {
        if (sLogger)
        	sLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    
    if (pat->getDim() != dim) 
    {
        if (sLogger)
        	sLogger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }
    
    if (n == IVector::NORM::AMOUNT || tol < 0.0)
    {
        if (sLogger)
        	sLogger->sever(RC::INVALID_ARGUMENT, __FILE__, __func__, __LINE__);
        return RC::INVALID_ARGUMENT;
    }
    
    if (std::isnan(tol)) 
    {
        if (sLogger)
        	sLogger->sever(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
        return RC::NOT_NUMBER;
    }
    
    if (std::isinf(tol)) 
    {
        if (sLogger)
        	sLogger->sever(RC::INFINITY_OVERFLOW, __FILE__, __func__, __LINE__);
        return RC::INFINITY_OVERFLOW;
    }

    IVector *buffer = IVector::createVector(dim, data);
    if (!buffer) 
    {
        if (sLogger)
        	sLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return RC::ALLOCATION_ERROR;
    }
    
    if (IVector::equals(buffer, pat, n, tol)) 
    {
        delete buffer;
        return remove(0);
    }

    RC msg = RC::SUCCESS;
    
    for (size_t i = 1; i < size; i++) 
    {
        msg = buffer->setData(dim, data + i * dim);
        if (msg != RC::SUCCESS) 
        {
            delete buffer;
            return msg;
        }
        
        if (IVector::equals(buffer, pat, n, tol)) 
        {
            delete buffer;
            return remove(i);
        }
    }

    delete buffer;
    return RC::VECTOR_NOT_FOUND;
}

ISet::IIterator *SetImpl::getIterator(size_t index) const {
    return Iterator::makeIterator(cBlock, index, dim);
}

ISet::IIterator *SetImpl::getBegin() const {
    return Iterator::makeIterator(cBlock, 0, dim);
}

ISet::IIterator *SetImpl::getEnd() const {
    return Iterator::makeIterator(cBlock, size - 1, dim);
}

SetImpl::~SetImpl() noexcept 
{
    delete[] data;
    delete[] indexes;
    cBlock->updateData(nullptr, nullptr, dim, 0);
}

// Set Iterator implementation

ILogger* SetImpl::Iterator::iterLogger = nullptr;

SetImpl::Iterator::Iterator() noexcept : controlBlock(nullptr), buffer(nullptr), unic_index(0), dim(0) {}

ISet::IIterator* SetImpl::Iterator::makeIterator(std::shared_ptr<ISetControlBlock> controlBlock, size_t index, size_t dim) 
{
    if(!controlBlock)
    {
        if(iterLogger)
        	iterLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    double* buff = new double[dim];
    
    if(!buff)
    {
        if(iterLogger)
        	iterLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    support::fill(buff, buff + dim * sizeof(double), 0);

    IVector* buffer = IVector::createVector(dim, buff);
    delete buff;
    
    size_t indexNew = 0;
    RC msg = controlBlock->getNext(buffer, indexNew, index);
    
    if(msg != RC::SUCCESS)
    {
        delete buffer;
        
        if(iterLogger)
        	iterLogger->sever(msg, __FILE__, __func__, __LINE__);
        
        return nullptr;
    }
    
    Iterator* resIterator = new Iterator();
    
    if(!resIterator)
    {
        delete buffer;
        if(iterLogger)
        	iterLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    resIterator->controlBlock = controlBlock;
    resIterator->buffer = buffer;
    resIterator->unic_index = indexNew;
    resIterator->dim = dim;
    if (iterLogger)
        resIterator->setLogger(iterLogger);

    return resIterator;
}

ISet::IIterator* SetImpl::Iterator::getNext(size_t indexInc) const {
    return makeIterator(controlBlock, unic_index + indexInc, dim);
}

ISet::IIterator* SetImpl::Iterator::getPrevious(size_t indexInc) const {
    return makeIterator(controlBlock, unic_index - indexInc, dim);
}

ISet::IIterator* SetImpl::Iterator::clone() const 
{
    Iterator* resIterator = new Iterator();
    if(!resIterator)
    {
        if(iterLogger)
        	iterLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    resIterator->controlBlock = controlBlock;
    resIterator->buffer = buffer->clone();
    
    if(!resIterator->buffer)
    {
        delete resIterator;
        return nullptr;
    }
    
    resIterator->unic_index = unic_index;
    resIterator->dim = dim;

    return resIterator;
}

RC SetImpl::Iterator::next(size_t indexInc) {
    return controlBlock->getNext(buffer, unic_index, indexInc);
}

RC SetImpl::Iterator::previous(size_t indexInc) {
    return controlBlock->getPrevious(buffer, unic_index, indexInc);
}

bool SetImpl::Iterator::isValid() const {
    return true; // ¬ыход за пределы множества невозможен в св€зи с реализацией контейнера
}

RC SetImpl::Iterator::makeBegin() {
    return controlBlock->getBegin(buffer, unic_index);
}

RC SetImpl::Iterator::makeEnd() {
    return controlBlock->getEnd(buffer, unic_index);
}

RC SetImpl::Iterator::getVectorCopy(IVector * &val) const {
    val = buffer->clone();
    
    if (!val)
    {
    	if (iterLogger)
    		iterLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
    	return RC::ALLOCATION_ERROR;
    }
    
    return RC::SUCCESS;
}

RC SetImpl::Iterator::getVectorCoords(IVector *const &val) const 
{
    if(!val)
    {
        if(iterLogger)
        	iterLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return RC::NULLPTR_ERROR;
    }
    
    if(val->getDim() != buffer->getDim())
    {
        if(iterLogger)
        	iterLogger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return RC::MISMATCHING_DIMENSIONS;
    }

    RC msg = val->setData(buffer->getDim(), buffer->getData());
    
    if(msg !=RC::SUCCESS)
    {
        if(iterLogger)
        	iterLogger->sever(msg, __FILE__, __func__, __LINE__);
        return msg;
    }

    return RC::SUCCESS;
}

ISet::IIterator::~IIterator() noexcept {}

SetImpl::Iterator::~Iterator() noexcept {
    delete[] buffer;
}




