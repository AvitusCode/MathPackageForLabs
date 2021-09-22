#include <cmath>
#include <limits>
#include <memory>
#include "VectorImpl.h"

 ILogger* myv::MyVector::lptr = nullptr;

IVector* myv::MyVector::clone() const
{
	bytes *memBuff = new (std::nothrow) bytes[sizeof(MyVector) + dim * sizeof(double)]; // no throw overloded

	if (!memBuff)
	{
		if (myv::MyVector::lptr != nullptr)
			myv::MyVector::lptr->sever(RC::ALLOCATION_ERROR, __FILE__, __FUNCTION__, __LINE__);
		return nullptr;
	}
	myv::MyVector* tmp_ptr =  new (memBuff) MyVector(dim);
	tmp_ptr->setData(dim, getData());
	
	return tmp_ptr;
}

RC myv::MyVector::setData(size_t dim, double const *const& ptr_data)
{
	if (dim != this->dim || !ptr_data)
		return RC::INVALID_ARGUMENT;
	
	memcpy(mutableData(), ptr_data, dim * sizeof(double));

	return RC::SUCCESS;
}

RC myv::MyVector::getCord(size_t index, double &val) const
{
	if (index < 0 || index >= dim)
	{
		if (myv::MyVector::lptr != nullptr)
			myv::MyVector::lptr->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);

		return RC::INDEX_OUT_OF_BOUND;
	}

	val = getData()[index];
	return RC::SUCCESS;
}

RC myv::MyVector::setCord(size_t index, double val)
{
	if (index < 0 || index >= dim)
	{
		if (myv::MyVector::lptr != nullptr)
			myv::MyVector::lptr->sever(RC::INDEX_OUT_OF_BOUND, __FILE__, __FUNCTION__, __LINE__);

		return RC::INDEX_OUT_OF_BOUND;
	}

	mutableData()[index] = val;
	return RC::SUCCESS;
}

RC myv::MyVector::scale(double multiplier)
{
	double *vals = mutableData();
	
	// —начала провер€ем данные, потом сохран€ем их. „тобы оптимизировать код по скорости - можно завести доп массив и сохран€ть данные, а уже потом просто перекопировать их
	for (size_t i = 0; i < dim; i++)
	{
		double mul = vals[i] * multiplier;
		if (CHECK_FIELD(mul))
		{
			if (myv::MyVector::lptr != nullptr)
				myv::MyVector::lptr->warning(RC::INFINITY_OVERFLOW, __FILE__, __FUNCTION__, __LINE__);
			return RC::INFINITY_OVERFLOW;
		}
	}

	for (size_t i = 0; i < dim; i++)
		vals[i] *= multiplier;

	return RC::SUCCESS;
}

RC myv::MyVector::inc(IVector const *const& op)
{
	if (op->getDim() != dim)
		return RC::MISMATCHING_DIMENSIONS;

	double *tdata = mutableData();
	double const *untdata = op->getData();

	for (size_t i = 0; i < dim; i++)
	{
		double increment = untdata[i] + tdata[i];

		if (CHECK_FIELD(increment))
		{
			if (myv::MyVector::lptr != nullptr)
				myv::MyVector::lptr->warning(RC::INFINITY_OVERFLOW, __FILE__, __FUNCTION__, __LINE__);
			return RC::INFINITY_OVERFLOW;
		}
	}

	for (size_t i = 0; i < dim; i++)
		tdata[i] += untdata[i];

	return RC::SUCCESS;
}

RC myv::MyVector::dec(IVector const *const& op)
{
	if (op->getDim() != dim)
		return RC::MISMATCHING_DIMENSIONS;

	double *tdata = mutableData();
	double const *untdata = op->getData();

	for (size_t i = 0; i < dim; i++)
	{
		double decrement = tdata[i] - untdata[i];

		if (CHECK_FIELD(decrement))
		{
			if (myv::MyVector::lptr != nullptr)
				myv::MyVector::lptr->warning(RC::INFINITY_OVERFLOW, __FILE__, __FUNCTION__,__LINE__);
			return RC::INFINITY_OVERFLOW;
		}
	}

	for (size_t i = 0; i < dim; i++)
		tdata[i] -= untdata[i];

	return RC::SUCCESS;
}

double myv::MyVector::norm(NORM n) const
{
	double res = 0;
	double const *dt = getData();

	switch (n)
	{
	case NORM::FIRST:
		for (size_t i = 0; i < dim; i++)
			res += std::fabs(dt[i]);
		return res;
	case NORM::SECOND:
		for (size_t i = 0; i < dim; i++)
			res += dt[i] * dt[i];
		return std::sqrt(res);
	case NORM::INFINITE:
		for (size_t i = 0; i < dim; i++)
			res = res < std::fabs(dt[i]) ? std::fabs(dt[i]) : res;
		return res;
	case NORM::AMOUNT:
		return static_cast<double>(NORM::AMOUNT);
	}

	return std::numeric_limits<double>::quiet_NaN();
}

RC myv::MyVector::applyFunction(const std::function<double(double)> &fun)
{
	double *vals = mutableData();
	for (size_t i = 0; i < dim; i++)
	{
		double num = fun(vals[i]);
		if (CHECK_FIELD(num))
		{
			if (myv::MyVector::lptr != nullptr)
				myv::MyVector::lptr->warning(RC::INFINITY_OVERFLOW, __FILE__, __FUNCTION__,__LINE__);
			return RC::INFINITY_OVERFLOW;
		}
	}

	for (size_t i = 0; i < dim; i++)
		vals[i] = fun(vals[i]);

	return RC::SUCCESS;
}

RC myv::MyVector::foreach(const std::function<void(double)> &fun) const
{
	double const *data = getData();
	for (size_t i = 0; i < dim; i++)
		fun(data[i]);

	return RC::SUCCESS;
}

// ¬озможно стоит поправить
myv::MyVector::~MyVector() noexcept
{
	double* data = mutableData();
	delete[] data;
}




