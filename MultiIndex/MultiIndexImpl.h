#pragma once

#include "IMultiIndex.h"

class MultiIndexImpl : public IMultiIndex
{
public:
	using bytes = unsigned char;
	MultiIndexImpl() = delete;
	MultiIndexImpl(size_t d) : dim(d) {};
	
	IMultiIndex * clone() const override;
	size_t getDim() const override;
	const size_t* getData() const override;
	RC setData(size_t dim, size_t const *const&ptr_data) override;
	
	static RC setLogger(ILogger * const pLogger){
		if (!pLogger)
			return RC::NULLPTR_ERROR;
		mLogger = pLogger;
		return RC::SUCCESS;
	}
	static ILogger* getLogger(){
		return mLogger;
	}
	
	
	RC getAxisIndex(size_t axisIndex, size_t &val) const override;
	RC setAxisIndex(size_t axisIndex, size_t val) override;
	RC incAxisIndex(size_t axisIndex, ssize_t val) override;
	
private:
	size_t* mutableData(){
		return reinterpret_cast<size_t*>(reinterpret_cast<bytes*>(this) + sizeof(MultiIndexImpl));
	}
	static ILogger* mLogger;
	size_t dim;
};
