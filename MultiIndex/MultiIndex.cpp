#include "MultiIndexImpl.h"
#include <new>
#include <memory>

IMultiIndex * IMultiIndex::createMultiIndex(size_t dim, const size_t* indices)
{
	if (dim == 0 || !indices)
	{
		return nullptr;
	}
	
	MultiIndexImpl::bytes* data = new (std::nothrow) MultiIndexImpl::bytes[sizeof(MultiIndexImpl) + dim * sizeof(size_t)];
	
	if (!data){
		return nullptr;
	}
	
	IMultiIndex* result = new (data) MultiIndexImpl(dim);
	memcpy(reinterpret_cast<MultiIndexImpl::bytes*>(data) + sizeof(MultiIndexImpl), indices, dim * sizeof(size_t));
	
	return result;
}
