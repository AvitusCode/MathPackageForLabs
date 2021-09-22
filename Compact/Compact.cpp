#include "CompactImpl.h"
#include <cmath>
#include <algorithm>

ICompact* ICompact::createCompact(IVector const * vec1, IVector const * vec2, IMultiIndex const *nodeQuantities)
{
	ILogger* logger = CompactImpl::getLogger();
	
	if (!vec1 || !vec2 || !nodeQuantities)
	{
		if (logger)
			logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	if (vec1->getDim() != vec2->getDim() || vec1->getDim() != nodeQuantities->getDim())
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	return new (std::nothrow) CompactImpl(vec1, vec2, nodeQuantities);
}

// вспомогательная функция для поиска пересечений
bool isIntersect(double x1, double y1, double x2, double y2, double& x_n, double& y_n)
{
	if (x1 > y2 || y1 < x2)
	{
		x_n = y_n = 0;
		return false;
	}
	else if (x1 < x2 && y1 > x2 && y1 < y2)
	{
		x_n = x2;
		y_n = y1;
		return true;
	}
	else if (x2 < x1 && y2 > x1 && y2 < y1)
	{
		x_n = x1;
		y_n = y2;
		return true;
	}
	else if (x1 < x2 && y2 < y1)
	{
		x_n = x2;
		y_n = y2;
		return true;
	}
	else if (x2 < x1 && y1 < y2)
	{
		x_n = x1;
		y_n = y1;
	}
	else{
		x_n = y_n = 0;
		return false;
	}
	
	return false;
}

ICompact* ICompact::createIntersection(ICompact const *op1, ICompact const *op2, IMultiIndex const* const grid, double tol)
{
	ILogger* logger = CompactImpl::getLogger();
	
	if (!op1 || !op2 || !grid)
	{
		if (logger)
			logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	if (op1->getDim() != op2->getDim() || op1->getDim() != grid->getDim())
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	if (std::isnan(tol) || std::isinf(tol))
	{
		if (logger)
			logger->sever(RC::AMOUNT, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	IVector* left_op1 = nullptr, *right_op1 = nullptr, *left_op2 = nullptr, *right_op2 = nullptr,
			*lBorderNew = nullptr, *rBorderNew = nullptr, *ord = nullptr;
	
	RC msg = op1->getLeftBoundary(left_op1);
	if (msg == RC::SUCCESS)
	    msg = op1->getRightBoundary(right_op1);
	if (msg == RC::SUCCESS)
	    msg = op2->getLeftBoundary(left_op2);
	if (msg == RC::SUCCESS)
	    msg = op2->getRightBoundary(right_op2);
	if (msg == RC::SUCCESS)
		    msg = op2->getRightBoundary(lBorderNew);
	if (msg == RC::SUCCESS)
		    msg = op2->getRightBoundary(rBorderNew);
	
	if (msg == RC::SUCCESS)
	{
		double ord_data[2] = {0, 0};
		ord = IVector::createVector(2, ord_data);
	}
	
	IMultiIndex* gridNew = grid->clone();
	
	if (msg != RC::SUCCESS || !ord || !gridNew)
	{
		if (logger)
			logger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
		
		delete left_op1;
		delete right_op1;
		delete left_op2;
		delete right_op2;
		delete lBorderNew;
		delete rBorderNew;
		delete ord;
		delete gridNew;
		return nullptr;
	}
	
	const size_t dim = op1->getDim();
	
	for (size_t i = 0; i < dim; i++)
	{
		const double x1 = left_op1->getData()[i];
		const double y1 = right_op1->getData()[i];
		const double x2 = left_op2->getData()[i];
		const double y2 = right_op2->getData()[i];
		double x_n = 0, y_n = 0;
		
		if (!isIntersect(x1, y1, x2, y2, x_n, y_n))
		{
			if (logger)
				logger->info(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
			
			delete left_op1;
			delete right_op1;
			delete left_op2;
			delete right_op2;
			delete lBorderNew;
			delete rBorderNew;
			delete ord;
			delete gridNew;
			return nullptr;
		}
		
		lBorderNew->setCord(i, x_n);
		rBorderNew->setCord(i, y_n);
		ord->setCord(0, x_n);
		ord->setCord(1, y_n);
		
		if (ord->norm(IVector::NORM::INFINITE) < tol)
		{
			gridNew->setAxisIndex(i, 1);
		}
	}
	
	ICompact* result = ICompact::createCompact(lBorderNew, rBorderNew, gridNew);
	
	if (!result)
	{
		if (logger)
		    logger->info(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
	}
	
	delete left_op1;
	delete right_op1;
	delete left_op2;
	delete right_op2;
	delete lBorderNew;
	delete rBorderNew;
	delete ord;
	delete gridNew;
	
	return result != nullptr ? result : nullptr;
}

ICompact* ICompact::createCompactSpan(ICompact const *op1, ICompact const *op2, IMultiIndex const* const grid)
{
	ILogger* logger = CompactImpl::getLogger();

	if (!op1 || !op2 || !grid)
	{
		if (logger)
			logger->sever(RC::NULLPTR_ERROR, __FILE__, __func__, __LINE__);
		return nullptr;
	}

	if (op1->getDim() != op2->getDim() || op1->getDim() != grid->getDim())
	{
		if (logger)
			logger->sever(RC::MISMATCHING_DIMENSIONS, __FILE__, __func__, __LINE__);
		return nullptr;
	}
	
	IVector* left_op1 = nullptr, *right_op1 = nullptr, *left_op2 = nullptr, *right_op2 = nullptr,
				*lBorderNew = nullptr, *rBorderNew = nullptr;
		
	RC msg = op1->getLeftBoundary(left_op1);
	if (msg == RC::SUCCESS)
		msg = op1->getRightBoundary(right_op1);
	if (msg == RC::SUCCESS)
		msg = op2->getLeftBoundary(left_op2);
	if (msg == RC::SUCCESS)
		msg = op2->getRightBoundary(right_op2);
	if (msg == RC::SUCCESS)
		msg = op2->getRightBoundary(lBorderNew);
	if (msg == RC::SUCCESS)
		msg = op2->getRightBoundary(rBorderNew);

	if (msg != RC::SUCCESS)
	{
		if (logger)
			logger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);

		delete left_op1;
		delete right_op1;
		delete left_op2;
		delete right_op2;
		delete lBorderNew;
		delete rBorderNew;
		return nullptr;
	}
	
	const size_t dim = op1->getDim();
	
	for (size_t i = 0; i < dim; i++)
	{
		double x_n = left_op1->getData()[i];
		double y_n = left_op1->getData()[i];
		
		x_n = std::min(x_n ,std::min(right_op1->getData()[i], std::min(left_op2->getData()[i], right_op2->getData()[i])));
		y_n = std::max(y_n, std::max(right_op1->getData()[i], std::max(left_op2->getData()[i], right_op2->getData()[i])));
		
		lBorderNew->setCord(i, x_n);
		rBorderNew->setCord(i, y_n);
	}
	
	ICompact* result = ICompact::createCompact(lBorderNew, rBorderNew, grid);
	
	if (!result)
	{
		if (logger)
			logger->sever(RC::ALLOCATION_ERROR, __FILE__, __func__, __LINE__);
	}

	delete left_op1;
	delete right_op1;
	delete left_op2;
	delete right_op2;
	delete lBorderNew;
	delete rBorderNew;

	return result != nullptr ? result : nullptr;
}

ICompact::~ICompact() noexcept {};

