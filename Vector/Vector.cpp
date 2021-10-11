#include <memory>
#include <cmath>
#include "VectorImpl.h"

IVector * IVector::createVector(size_t dim, const double *const &ptr_data)
{
	if (!ptr_data || !dim)
		return nullptr;

	myv::bytes* memBuffer = new (std::nothrow) myv::bytes[sizeof(myv::MyVector) + dim * sizeof(double)];

	if (!memBuffer)
		return nullptr;

	IVector* vector = new (memBuffer) myv::MyVector(dim);
	memcpy(memBuffer + sizeof(myv::MyVector), ptr_data, dim * sizeof(double));

	return vector;
}

RC IVector::copyInstance(IVector *const dest, const IVector *const &src)
{
	if (!dest)
		return RC::NULLPTR_ERROR;

	if (dest->sizeAllocated() != src->sizeAllocated())
		return RC::MISMATCHING_DIMENSIONS;

	memcpy(reinterpret_cast<myv::bytes *>(dest), reinterpret_cast<myv::bytes const *>(src), src->sizeAllocated());

	return RC::SUCCESS;
}

RC IVector::moveInstance(IVector* const dest, IVector*& src)
{
	RC msg = copyInstance(dest, src);

	if (msg != RC::SUCCESS)
		return msg;
	else
		delete src;

	return msg;
}

IVector * IVector::add(const IVector *const &op1, const IVector *const &op2)
{
	if (op1->getDim() != op2->getDim())
		return nullptr;

	IVector* result = op1->clone();
	RC msg = result->inc(op2);
	if (msg != RC::SUCCESS)
	    return nullptr;

	return result;
}


IVector * IVector::sub(const IVector *const &op1, const IVector *const &op2)
{
	if (op1->getDim() != op2->getDim())
			return nullptr;

	IVector* result = op1->clone();
	RC msg = result->dec(op2);
    if (msg != RC::SUCCESS){
    	return nullptr;
    }

	return result;
}

double IVector::dot(const IVector *const &op1, const IVector *const &op2)
{
	if (op1->getDim() != op2->getDim())
		return std::numeric_limits<double>::quiet_NaN();

	double dot = 0;
	double const *top1 = op1->getData(), *top2 = op2->getData();

	for (size_t i = 0; i < op1->getDim(); i++)
		dot += top1[i] * top2[i];

	if (myv::MyVector::CHECK_FIELD(dot))
		return std::numeric_limits<double>::quiet_NaN();

	return dot;
}

bool IVector::equals(const IVector *const &op1, const IVector *const &op2, NORM n, double tol)
{
	if ((!op1 || !op2) || op1->getDim() != op2->getDim())
		return false;
	
    return std::fabs(op1->norm(n) - op2->norm(n)) < tol;
}

RC IVector::setLogger(ILogger *const logger)
{
    return myv::MyVector::setLogger(logger);
}

IVector::~IVector() noexcept = default;
