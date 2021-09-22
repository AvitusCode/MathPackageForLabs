#include "CompactControlBlockImpl.h"

CompactControlBlockImpl::CompactControlBlockImpl() : entity(nullptr) {};

RC CompactControlBlockImpl::get(IMultiIndex *const &currentIndex, IMultiIndex const *const& bypassOrder) const
{
	ILogger* logger = entity->getLogger();
	
	if (!currentIndex || !bypassOrder)
	{
		if (logger)
			logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return RC::NULLPTR_ERROR;
	}
	
	if (currentIndex->getDim() != bypassOrder->getDim())
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return RC::MISMATCHING_DIMENSIONS;
	}
	
	IMultiIndex *grid = entity->getGrid();
	
	const size_t * const gridData  = grid->getData();
	const size_t * const orderData = bypassOrder->getData();
	const size_t * const indexData = currentIndex->getData();
	
	for (size_t i = 0; i < currentIndex->getDim(); i++)
	{
		size_t index = orderData[i];
		
		if (indexData[index] == gridData[index])
		{
			currentIndex->setAxisIndex(index, 1);
		}
		else if (indexData[index] < gridData[index])
		{
			currentIndex->incAxisIndex(index, 1);
			delete grid;
			return RC::SUCCESS;
		}
	}
	
	if (logger)
		logger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
	
	delete grid;
	return RC::INDEX_OUT_OF_BOUND;
}

RC CompactControlBlockImpl::get(IMultiIndex const *const&currentIndex, IVector *const&val) const
{
	ILogger *logger = entity->getLogger();

	if (!currentIndex || !val)
	{
		if (logger)
			logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return RC::NULLPTR_ERROR;
	}

	IMultiIndex *grid = entity->getGrid();
	
	if (currentIndex->getDim() != grid->getDim())
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return RC::MISMATCHING_DIMENSIONS;
	}
	
	for (size_t i = 0; i < entity->getDim(); i++)
	{
		if (currentIndex->getData()[i] > grid->getData()[i])
		{
			if (logger)
				logger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__,__LINE__);
			delete grid;
			return RC::INDEX_OUT_OF_BOUND;
		}
	}
	
	delete grid;
	
	IVector *buffer = nullptr;
	RC msg = entity->getVectorCopy(currentIndex, buffer);
	
	if (msg != RC::SUCCESS)
	{
		if (logger)
			logger->sever(msg, __FILE__, __func__,__LINE__);
		
		return msg;
	}
	
	val->setData(currentIndex->getDim(), buffer->getData());
	
	delete buffer;
	return RC::SUCCESS;
}

ICompactControlBlock::~ICompactControlBlock() noexcept = default;



