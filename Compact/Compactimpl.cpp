#include "CompactImpl.h"
#include "../support_func.h"
// Compact implementation

ILogger* CompactImpl::cLogger = nullptr;

CompactImpl::CompactImpl(const IVector *left, const IVector *right, const IMultiIndex *nodes)
{
	leftBorder = left->clone();
	rightBorder = right->clone();
	grid = nodes->clone();
	dim = left->getDim();
	
	cBlock = std::make_shared<CompactControlBlockImpl>(this);
}

ICompact* CompactImpl::clone() const{
	return ICompact::createCompact(leftBorder, rightBorder, grid);
}

bool CompactImpl::isInside(IVector const *const& vec) const
{
	if (!vec)
	{
		if (cLogger)
			cLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return false;
	}
	
	if (vec->getDim() != dim)
	{
		if (cLogger)
			cLogger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return false;
	}
	
	const double* pleftBorder  = leftBorder->getData();
	const double* prightBorder = rightBorder->getData();
	const double* pData = vec->getData();
	
	for (size_t i = 0; i < dim; i++)
	{
		if (pData[i] < pleftBorder[i] || pData[i] > prightBorder[i]){
			return false;
		}
	}
	
	return true;
}

RC CompactImpl::getVectorCopy(IMultiIndex const *index, IVector *&val) const
{
	if (val != nullptr)
	{
		// может стоит обрабатывать случай, когда пам€ть по адресу зан€та?
	}
	
	val = leftBorder->clone();
	
	if (!val)
	{
		if (cLogger)
			cLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return RC::NULLPTR_ERROR;
	}
	
	RC msg = getVectorCoords(index, val);
	
	if (msg != RC::SUCCESS)
	{
		if (cLogger)
			cLogger->sever(msg, __FILE__, __func__, __LINE__);
		delete val;
		return msg;
	}
	
	return RC::SUCCESS;
}

RC CompactImpl::getVectorCoords(IMultiIndex const *index, IVector *const& val) const
{
	if (!index || !val)
	{
		if (cLogger)
			cLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return RC::NULLPTR_ERROR;
	}

	if (index->getDim() != val->getDim())
	{
		if (cLogger)
			cLogger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return RC::MISMATCHING_DIMENSIONS;
	}

	const size_t *pindex = index->getData();
	const size_t *pgrid = grid->getData();

	for (size_t i = 0; i < dim; i++)
	{
		// ≈сли вне сетки
		if (pindex[i] > pgrid[i])
		{
			if (cLogger)
				cLogger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__,__LINE__);
			return RC::INDEX_OUT_OF_BOUND;
		}
	}

	const double *pleftBorder = leftBorder->getData();
	const double *prightBorder = rightBorder->getData();

	for (size_t i = 0; i < dim; i++)
	{
		// Ќам нужно забрать некоторое подмножество
		const double param = static_cast<double>(pindex[i])
				/ (static_cast<double>(pgrid[i]) - 1.0);

		val->setCord(i, (1.0 - param) * pleftBorder[i] + param * prightBorder[i]); // (1-liambda)*a + liambda*b in [a, b], liambda in [0, 1]
	}

	return RC::SUCCESS;
}

// ¬ принципе здесь имеетс€ дублирование кода. ћожно исправить?

RC CompactImpl::getLeftBoundary(IVector *&vec) const
{
	IVector *result = leftBorder->clone();
	
	if (!result)
	{
		if (cLogger)
			cLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return RC::NULLPTR_ERROR;
	}
	
	vec = result;
	return RC::SUCCESS;
}

RC CompactImpl::getRightBoundary(IVector *&vec) const
{
	IVector *result = rightBorder->clone();

	if (!result)
	{
		if (cLogger)
			cLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return RC::NULLPTR_ERROR;
	}

	vec = result;
	return RC::SUCCESS;
}

size_t CompactImpl::getDim() const{
	return dim;
}

// Ѕудем передавать копию сетки, чтобы не было возможности ее случайно помен€ть
IMultiIndex* CompactImpl::getGrid() const{
	return grid->clone();
}

ICompact::IIterator* CompactImpl::getIterator(IMultiIndex const *const&index, IMultiIndex const *const& bypassOrder) const
{
	if (!index || !bypassOrder)
	{
		if (cLogger)
			cLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	if (index->getDim() != bypassOrder->getDim())
	{
		if (cLogger)
			cLogger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
	    return nullptr;
	}
	
	IVector* vec = nullptr;
	RC msg = getVectorCopy(index, vec);
	
	if (msg != RC::SUCCESS)
	{
		if (cLogger)
			cLogger->sever(msg, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	IMultiIndex* ind = index->clone();
	IMultiIndex* ord = bypassOrder->clone();
	
	if (!ind || !ord)
	{
		if (cLogger)
			cLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		delete ind;
		delete ord;
		delete vec;
		return nullptr;
	}
	
	ICompact::IIterator *iter = Iterator::makeIterator(cBlock, vec, ind, ord);
	
	if (!iter)
	{
		if (cLogger)
			cLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		delete ind;
		delete ord;
		delete vec;
		return nullptr;
	}
	
	return iter;
}

ICompact::IIterator* CompactImpl::getBegin(IMultiIndex const *const& bypassOrder) const
{
	size_t *buffer = new (std::nothrow) size_t[dim];
	
	if (!buffer)
	{
		if (cLogger)
			cLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
	    return nullptr;
	}
	
	support::fill(buffer, buffer + dim, 1);
	IMultiIndex* index = IMultiIndex::createMultiIndex(dim, buffer);
	delete[] buffer;
	
	if (!index)
	{
		if (cLogger)
			cLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	IIterator* iter = getIterator(index, bypassOrder);
	
	delete index;
	return iter;
}

ICompact::IIterator* CompactImpl::getEnd(IMultiIndex const *const&bypassOrder) const
{
	IMultiIndex* index = IMultiIndex::createMultiIndex(dim, grid->getData());
	
	if (!index)
	{
		if (cLogger)
			cLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}

	IIterator *iter = getIterator(index, bypassOrder);

	delete index;
	return iter;
}

// Iterator implementation

ILogger* CompactImpl::Iterator::iterLogger = nullptr;

CompactImpl::Iterator::Iterator() noexcept : controlBlock(nullptr), vector(nullptr), index(nullptr), order(nullptr){}

ICompact::IIterator* CompactImpl::Iterator::makeIterator(std::shared_ptr<ICompactControlBlock> cb, IVector *vec, IMultiIndex *idx, IMultiIndex *ord)
{
	if(!cb || !vec || !idx || !ord)
	{
		if (iterLogger)
			iterLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	IVector* buffer = vec->clone();
	IMultiIndex* temp_indx = idx->clone();
	IMultiIndex* temp_ord = ord->clone();
	Iterator* resIterator = new Iterator(); // need to make noexcept
	
	if (!buffer || !temp_indx || !temp_ord || !resIterator)
	{
		if (iterLogger)
			iterLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		delete buffer;
		delete temp_indx;
		delete temp_ord;
		delete resIterator;
		return nullptr;
	}
	
	resIterator->controlBlock = cb;
	resIterator->vector = buffer;
	resIterator->index = temp_indx;
	resIterator->order = temp_ord;
	if (iterLogger)
		resIterator->setLogger(iterLogger);
	
	return resIterator;
}

ICompact::IIterator* CompactImpl::Iterator::getNext()
{
	ICompact::IIterator* iter = clone();
	
	if (!iter)
	{
		if (iterLogger)
			iterLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	RC msg = iter->next();
	
	if (msg != RC::SUCCESS)
	{
		if (iterLogger)
			iterLogger->sever(msg, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	return iter;
}

ICompact::IIterator* CompactImpl::Iterator::clone() const{	
	return makeIterator(controlBlock, vector, index, order);
}

RC CompactImpl::Iterator::next()
{
	RC msg = controlBlock->get(index, order);
	
	if (msg != RC::SUCCESS)
	{
		if (iterLogger)
			iterLogger->sever(msg, __FILE__, __func__, __LINE__);
		
		if (msg == RC::INDEX_OUT_OF_BOUND)
			descriptor = false; // Ѕольше не действителен
		return msg;
	}
	
	return controlBlock->get(index, vector);
}

bool CompactImpl::Iterator::isValid() const{
	return descriptor;
}

RC CompactImpl::Iterator::getVectorCopy(IVector *&val) const
{
	val = vector->clone();
	
	if (!val)
	{
		if (iterLogger)
			iterLogger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		return RC::ALLOCATION_ERROR;
	}
	
	return RC::SUCCESS;
}

RC CompactImpl::Iterator::getVectorCoords(IVector *const& val) const
{
	if (!val)
	{
		if (iterLogger)
			iterLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return RC::NULLPTR_ERROR;
	}
	
	if (val->getDim() != vector->getDim())
	{
		if (iterLogger)
			iterLogger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return RC::MISMATCHING_DIMENSIONS;
	}
	
	RC msg = val->setData(vector->getDim(), vector->getData());
	
	if (msg != RC::SUCCESS)
	{
		if (iterLogger)
			iterLogger->sever(msg, __FILE__, __func__, __LINE__);
		return msg;
	}
	
	return RC::SUCCESS;
}

ICompact::IIterator::~IIterator() noexcept {}

CompactImpl::Iterator::~Iterator() noexcept
{
	 delete vector;
	 delete index;
	 delete order;
}

