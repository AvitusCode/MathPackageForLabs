#include "MultiIndexImpl.h"
#include <cmath>
#include <memory>

ILogger* MultiIndexImpl::mLogger = nullptr;


IMultiIndex* MultiIndexImpl::clone() const
{
	return IMultiIndex::createMultiIndex(dim, getData());
}

size_t MultiIndexImpl::getDim() const{
	return dim;
}

const size_t* MultiIndexImpl::getData() const{
	return reinterpret_cast<const size_t*>(reinterpret_cast<const size_t*>(this) + sizeof(MultiIndexImpl));
}

RC MultiIndexImpl::setData(size_t dim, size_t const *const& ptr_data)
{
	if (this->dim != dim)
	{
		if (mLogger)
			mLogger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return RC::MISMATCHING_DIMENSIONS;
	}
	
	if (!ptr_data)
	{
		if (mLogger)
			mLogger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return RC::NULLPTR_ERROR;
	}
	
	// Проверка целостности данных
	for (size_t i = 0; i < dim; i++)
	{
		if (std::isinf(ptr_data[i]) || std::isnan(ptr_data[i]))
		{
			if (mLogger)
				mLogger->sever(RC::NOT_NUMBER, __FILE__, __func__, __LINE__);
			return RC::NOT_NUMBER;
		}
	}
	
	memcpy(mutableData(), ptr_data, dim * sizeof(size_t));
	
	return RC::SUCCESS;
}

RC MultiIndexImpl::getAxisIndex(size_t axisIndex, size_t &val) const
{
	if (axisIndex >= dim)
	{
		if (mLogger)
			mLogger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
		return RC::INDEX_OUT_OF_BOUND;
	}
	
	val = getData()[axisIndex];
	return RC::SUCCESS;
}

RC MultiIndexImpl::setAxisIndex(size_t axisIndex, size_t val)
{
	if (axisIndex >= dim)
	{
		if (mLogger)
			mLogger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
		return RC::INDEX_OUT_OF_BOUND;
	}

	mutableData()[axisIndex] = val;
	return RC::SUCCESS;
}

RC MultiIndexImpl::incAxisIndex(size_t axisIndex, ssize_t val)
{
	if (axisIndex >= dim)
	{
		if (mLogger)
			mLogger->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __func__, __LINE__);
		return RC::INDEX_OUT_OF_BOUND;
	}

	mutableData()[axisIndex] += val;
	return RC::SUCCESS;
}

IMultiIndex::~IMultiIndex() = default;
