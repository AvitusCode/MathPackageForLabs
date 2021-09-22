#include "Vector/IVector.h"
#include <iostream>
#include "Logger/LoggerImpl.h"

void testFunc()
{

	auto printData = [](double val){
		std::cout << val << " ";
	};
	
	ILogger *plgr = ILogger::createLogger("log.txt", true);
	double someData1[] = {1, 2, 3, 4, 5};
	double someData2[] = {1, 1, 1, 1, 1};

	IVector* vec1 = IVector::createVector(5, someData1);
	IVector* vec2 = IVector::createVector(5, someData2);
	vec1->setLogger(plgr);

	vec1->foreach(printData); std::cout << std::endl; // 1 2 3 4 5
	vec1->scale(2);
	vec1->foreach(printData); std::cout << std::endl; // 2 4 6 8 10

	IVector::copyInstance(vec2, vec1);
	vec2->foreach(printData); std::cout << std::endl; // 2 4 6 8 10

	IVector* vec3 = IVector::sub(vec2, vec1);
	vec3->foreach(printData); std::cout << std::endl; // 0 0 0 0 0 
	
	delete vec1;
	delete vec2;
	delete vec3;
	delete plgr;
}


// Написать тесты для сета, для компакта
int main(void)
{
	testFunc();
	
	return EXIT_SUCCESS;
}



