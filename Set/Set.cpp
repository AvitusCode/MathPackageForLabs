/*
 * Set.cpp
 *
 *  Created on: 10 сент. 2021 г.
 *      Author: Blasphemous
 */
#include "SetImpl.h"

ISet* ISet::createSet(){
	return new SetImpl();
}

ISet* ISet::makeIntersection(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol)
{
    ILogger* const logger = SetImpl::getLogger();
    
    if(!op1|| !op2)
    {
        if(logger)
        	logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    
    if(op1->getDim() != op2->getDim())
    {
        if(logger)
        	logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return nullptr;
    }

    // Здесь и далее определяем наиб. по размеру множество
    const ISet *const first  = op1->getSize() >= op2->getSize() ? op1 : op2;
    const ISet *const second = op1->getSize() < op2->getSize() ? op1 : op2;
    
    if (op1->getSize() == 0 || op2->getSize() == 0)
    {
    	if(logger)
    	    logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
    	return nullptr;
    }
    
    ISet* result = ISet::createSet();
    if (!result)
      {
       	if(logger)
       	    logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
       	return nullptr;
      }
    
    IVector* vec1 = nullptr, *vec2 = nullptr;
    
   RC msg =  first->getCopy(0, vec1);
   if (msg != RC::SUCCESS)
   {
		if (logger)
			logger->sever(msg, __FILE__, __func__, __LINE__);
		delete result;
		return nullptr;
   }
   
    msg = second->getCopy(0, vec2);
	if (msg != RC::SUCCESS)
	{
		if (logger)
			logger->sever(msg, __FILE__, __func__, __LINE__);
		delete vec1;
		delete result;
		return nullptr;
	}
    
    if (IVector::equals(vec1, vec2, n, tol))
    {
    	result->insert(vec1, n, tol);
    }
    
    
    for (size_t i = 1; i < first->getSize(); i++)
    {
    	first->getCoords(i, vec1);
    	for (size_t j = 1; j < second->getSize(); j++)
    	{
    		second->getCoords(i, vec2);
    		if (IVector::equals(vec1, vec2, n, tol))
    		{
    			result->insert(vec1, n, tol);
    		}
    	}
    }
    
    delete vec1;
    delete vec2;
    return result;
}

ISet* ISet::makeUnion(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) 
{
    ILogger* const logger = SetImpl::getLogger();
    if(!op1 || !op2)
    {
        if(logger)
        	logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
        return nullptr;
    }
    
    if(op1->getDim() != op2->getDim())
    {
        if(logger)
        	logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return nullptr;
    }

	const ISet *const first = op1->getSize() >= op2->getSize() ? op1 : op2;
	const ISet *const second = op1->getSize() < op2->getSize() ? op1 : op2;

	if (op1->getSize() == 0 || op2->getSize() == 0)
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__,__LINE__);
		return nullptr;
	}

	ISet *result = first->clone();
	if (!result)
	{
		if (logger)
			logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}

	// Больше логгирования
	IVector *vec = nullptr;
	RC msg = second->getCopy(0, vec);
	if (msg != RC::SUCCESS)
	{
		if (logger)
			logger->sever(msg, __FILE__, __func__, __LINE__);
		delete result;
		return nullptr;
	}
	
	msg = result->insert(vec, n, tol);
	if (msg != RC::SUCCESS)
	{
		if (logger)
			logger->sever(msg, __FILE__, __func__, __LINE__);
		delete vec;
		delete result;
		return nullptr;
	}
	
	for (size_t i = 1; i < second->getSize(); i++)
	{
		second->getCoords(i, vec);
		msg = result->insert(vec, n, tol);
		if (msg != RC::SUCCESS)
		{
			if (logger)
				logger->sever(msg, __FILE__, __func__, __LINE__);
			delete vec;
			delete result;
			return nullptr;
		}
	}

	delete vec;
    return result;
}

ISet* ISet::sub(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) 
{
	ILogger* const logger = SetImpl::getLogger();
	if (!op1 || !op2)
	{
		if (logger)
			logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}

	if (op1->getDim() != op2->getDim())
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__,  __LINE__);
		return nullptr;
	}
	
	if (op1->getSize() < op2->getSize() || (op1->getSize() == 0 || op2->getSize() == 0))
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return nullptr;
	}

    ISet* result = op1->clone();
    IVector* vec = nullptr;
    
    RC msg = op2->getCopy(0, vec);
    if (msg != RC::SUCCESS)
	{
		if (logger)
			logger->sever(msg, __FILE__, __func__, __LINE__);
		delete result;
		return nullptr;
	}
    
    msg = result->findFirst(vec, n, tol);
    if (msg == RC::VECTOR_ALREADY_EXIST)
    {
    	result->remove(vec, n, tol);
    }
    
    for (size_t i = 1; i < op2->getSize(); i++)
	{
		op2->getCoords(i, vec);
		RC msg = result->findFirst(vec, n, tol);
		    
		if (msg == RC::VECTOR_ALREADY_EXIST){
			result->remove(vec, n, tol);
		}
	}
    
    //delete vec;
    return result;
}

ISet* ISet::symSub(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) 
{
	ILogger *const logger = SetImpl::getLogger();

	if (!op1 || !op2)
	{
		if (logger)
			logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}

	if (op1->getDim() != op2->getDim())
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return nullptr;
	}

	const ISet *const first = op1->getSize() >= op2->getSize() ? op1 : op2;
	const ISet *const second = op1->getSize() < op2->getSize() ? op1 : op2;

	if (op1->getSize() == 0 || op2->getSize() == 0)
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return nullptr;
	}

	ISet *result = ISet::createSet();
	if (!result)
	{
		if (logger)
			logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}

	IVector *vec1 = nullptr, *vec2 = nullptr;

	// Тоже стоит залогировать
	RC msg = first->getCopy(0, vec1);
	if (msg != RC::SUCCESS)
	{
		if (logger)
			logger->sever(msg, __FILE__, __func__, __LINE__);
		delete result;
		return nullptr;
	}
	
	msg = second->getCopy(0, vec2);
	if (msg != RC::SUCCESS)
	{
		if (logger)
			logger->sever(msg, __FILE__, __func__, __LINE__);
		delete vec1;
		delete result;
		return nullptr;
	}

	if (!IVector::equals(vec1, vec2, n, tol))
	{
		msg = result->insert(vec1, n, tol);
		if (msg != RC::SUCCESS)
		{
			if (logger)
				logger->sever(msg, __FILE__, __func__, __LINE__);
			delete vec1;
			delete vec2;
			delete result;
			return nullptr;
		}
	}

	bool isIn = false;
	for (size_t i = 1; i < first->getSize(); i++)
	{
		first->getCoords(i, vec1);
		for (size_t j = 1; j < second->getSize(); j++)
		{
			second->getCoords(i, vec2);
			if (IVector::equals(vec1, vec2, n, tol))
			{
				isIn = true;
				break;
			}
			
			msg = result->insert(vec2, n, tol);
			if (msg != RC::SUCCESS)
			{
				if (logger)
					logger->sever(msg, __FILE__, __func__,__LINE__);
				delete vec1;
				delete vec2;
				delete result;
				return nullptr;
			}
		}
		
		if (!isIn)
		{
		    msg = result->insert(vec1, n, tol);
		    if (msg != RC::SUCCESS)
			{
				if (logger)
					logger->sever(msg, __FILE__, __func__,__LINE__);
				delete vec1;
				delete vec2;
				delete result;
				return nullptr;
			}
		}
		isIn = false;
	}

	delete vec1;
	delete vec2;
	return result;
}

bool ISet::equals(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) 
{
	ILogger *const logger = SetImpl::getLogger();

	if (!op1 || !op2)
	{
		if (logger)
			logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return false;
	}

	if (op1->getDim() != op2->getDim())
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return false;
	}
    if(op1->getSize() != op2->getSize())
    {
        if(logger)
        	logger->info(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
        return false;
    }

    IVector* vec = nullptr;
    if (op1->getCopy(0, vec) != RC::SUCCESS)
    {
    	if (logger)
    		logger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return false;
    }
    
    for (size_t i = 1; i < op1->getSize(); i++)
    {
    	op1->getCoords(i, vec);
    	
    	if (op2->findFirst(vec, n, tol) != RC::SUCCESS)
    	{
    		delete vec;
    		return false;
    	}
    }

    delete vec;
    return true;
}

bool ISet::subSet(const ISet *const &op1, const ISet *const &op2, IVector::NORM n, double tol) 
{
	ILogger *const logger = SetImpl::getLogger();

	if (!op1 || !op2)
	{
		if (logger)
			logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return false;
	}

	if (op1->getDim() != op2->getDim())
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return false;
	}
	if (op1->getSize() > op2->getSize())
	{
		if (logger)
			logger->info(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return false;
	}

    IVector* vec = nullptr;
    if(op1->getCopy(0, vec) != RC::SUCCESS)
    {
    	if (logger)
    		logger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
        return false;
    }
    
    for (size_t i = 1; i < op1->getSize(); i++)
	{
		op1->getCoords(i, vec);

		if (op2->findFirst(vec, n, tol) != RC::SUCCESS)
		{
			delete vec;
			return false;
		}
	}

    delete vec;
    return true;
}

ISet::~ISet() noexcept {}
