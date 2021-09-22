#pragma once
#include "IVector.h"
#include "../Logger/ILogger.h"
#include <limits>

namespace myv
{
    using bytes = unsigned char; // Ётим типом обозначим байты.   тому же сократим запись

    class MyVector : public IVector
	{
	public:
    	MyVector(size_t _dim) : dim(_dim) {};

    	static bool CHECK_FIELD(double num){
    		return (num) > std::numeric_limits<double>::infinity() || -(num) > std::numeric_limits<double>::infinity() || std::isnan(num);
    	}
    	IVector* clone() const override;
    	
    	// ƒанные дл€ просмотра предоставл€ем пользователю в константном виде
    	double const* getData() const override{
    		return reinterpret_cast<double const*>(reinterpret_cast<bytes const*>(this) + sizeof(MyVector));
    	}
    	RC setData(size_t dim, double const* const& ptr_data) override;

    	RC getCord(size_t index, double &val) const override;
    	RC setCord(size_t index, double val) override;
    	RC scale(double multiplier) override;
    	size_t getDim() const override{
    		return dim;
    	}

    	RC inc(IVector const* const& op) override;
    	RC dec(IVector const* const& op) override;

    	double norm(NORM n) const override;

    	RC applyFunction(const std::function<double (double)> &fun) override;
    	RC foreach(const std::function<void (double)> &fun) const override;
    	size_t sizeAllocated() const override{
    		return (sizeof(MyVector) + dim * sizeof(double));
    	}

    	static RC setLogger(ILogger* const logger){
    		if (!logger)
    			return RC::INVALID_ARGUMENT;
    		lptr = logger;
    		return RC::SUCCESS;
    	}
    	
    	~MyVector() noexcept;

	private:
    	// ѕрлучаем данные дл€ их изменени€ внутри методов класса
    	double* mutableData(){
    		return reinterpret_cast<double*>(reinterpret_cast<bytes*>(this) + sizeof(MyVector));
    	}
    	size_t dim;
    	static ILogger *lptr;
	};
}
