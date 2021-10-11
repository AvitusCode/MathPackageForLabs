#include "Vector/IVector.h"
#include <iostream>
#include "Logger/LoggerImpl.h"
#include "Set/ISet.h"

void printOk(long line)
{
	std::cerr << "Test ok in line: " << line << std::endl; 
}

// Можно добавить еще тесты на "краевые" условия
void testVector()
{
	auto printData = [](double val){
		std::cout << val << " ";
	};

	ILogger *plgr = ILogger::createLogger("log.txt", true);
	if(plgr == nullptr){
        std::cerr << "Logger was not created" << std::endl;
        return;
	}
	printOk(__LINE__);
	
	RC msg = IVector::setLogger(plgr);
	if (msg != RC::SUCCESS)
		std::cerr << "Logger set error" << std::endl;
	printOk(__LINE__);

	double someData1[] = {1, 2, 3, 4, 5};
	double someData2[] = {1, 1, 1, 1, 1};
	double someData3[] = {1, 33, 9, 100, 5};
	double tolerance = 1e-3;


	IVector* vec1 = IVector::createVector(0, someData1);
	if(vec1){
		std::cerr << "createVector does not work correctly" << std::endl;
        return;
	}
	printOk(__LINE__);

    vec1 = IVector::createVector(5, someData1);
    
    if (!vec1){
    	std::cerr << "createVector does not work correctly" << std::endl;
    	return;
    }
    printOk(__LINE__);
   
    vec1->foreach(printData); std::cout << std::endl;

    IVector* vec2 = vec1->clone();
	if (!vec2){
		std::cerr << "createVector does not work correctly" << std::endl;
		delete vec1;
		return;
	}
	printOk(__LINE__);
	
    for(size_t i = 0; i < vec1->getDim(); i++)
    {
        if(vec1->getData()[i] != vec2->getData()[i])
        {
        	std::cerr << "clone does not work correctly" << std::endl;
            delete vec1;
            delete vec2;
        }
    }
    printOk(__LINE__);
    vec2->foreach(printData); std::cout << std::endl;
    delete vec2;
    printOk(__LINE__);
    
    vec2 = IVector::createVector(5, someData2);

    msg = RC::SUCCESS;
    double val = 5;
    msg = vec2->setCord(100, 100);
    if(msg != RC::INDEX_OUT_OF_BOUND){
    	std::cerr << "setCord does not work correctly" << std::endl;
        delete vec1;
        delete vec2;
        return;
    }
    printOk(__LINE__);
    
    msg = vec2->getCord(100, val);
    if(msg != RC::INDEX_OUT_OF_BOUND){
    	std::cerr << "getCord does not work correctly" << std::endl;
        delete vec1;
        delete vec2;
        return;
    }
    printOk(__LINE__);

    IVector::copyInstance(vec1, vec2);
    for(size_t i = 0; i < vec1->getDim(); i++){
        if(vec1->getData()[i] != vec2->getData()[i]){
        	std::cerr << "copyInstance does not work correctly" << std::endl;
            delete vec1;
            delete vec2;
            return;
        }
    }
    vec2->foreach(printData); std::cout << std::endl;
    printOk(__LINE__);

    msg = vec2->setData(5, someData3);
    if(msg != RC::SUCCESS){
    	std::cerr << "setData does not work correctly" << std::endl;
		delete vec1;
		delete vec2;
		return;
    }
    printOk(__LINE__);

    IVector* vec3 = vec2->clone();
    if (!vec3){
    	std::cerr << "createVector does not work correctly" << std::endl;
		delete vec2;
		delete vec1;
		return;
    }
    
    vec2->inc(vec1);
    
    for(size_t i = 0; i < vec2->getDim(); i++){
        if(vec2->getData()[i] != vec1->getData()[i] + vec3->getData()[i])
        {
        	std::cerr << "inc does not work correctly" << std::endl;
            delete vec1;
            delete vec2;
            delete vec3;
            return;
        }
    }
    printOk(__LINE__);

    vec3 = vec2->clone();
    vec2->dec(vec1);
    for(size_t i = 0; i < vec2->getDim(); i++){
        if(vec2->getData()[i] != vec3->getData()[i] - vec1->getData()[i])
        {
        	std::cerr << "dec does not work correctly" << std::endl;
            delete vec1;
			delete vec2;
			delete vec3;
			return;
        }
    }
    printOk(__LINE__);
    
    vec2->foreach(printData); std::cout << std::endl;

    vec3 = IVector::add(vec1, vec2);
    for(size_t i = 0; i < vec1->getDim(); i++){
        if(vec3->getData()[i] != vec1->getData()[i] + vec2->getData()[i])
        {
        	std::cerr << "add does not work correctly" << std::endl;
			delete vec1;
			delete vec2;
			delete vec3;
			return;
        }
    }
    printOk(__LINE__);

    vec3 = IVector::sub(vec2, vec1);
    vec3->foreach(printData); std::cout << std::endl;
    for(size_t i = 0; i < vec2->getDim(); i++){
        if(vec3->getData()[i] != vec2->getData()[i] - vec1->getData()[i])
        {
        	std::cerr << "sub does not work correctly" << std::endl;
			delete vec1;
			delete vec2;
			delete vec3;
			return;
        }
    }
    printOk(__LINE__);

    vec3->scale(10);
    vec3->foreach(printData); std::cout << std::endl;

    vec1->setData(vec1->getDim(), someData1);
    vec2->setData(vec2->getDim(), someData2);
    
    bool check_norm = IVector::equals(vec1, vec2, IVector::NORM::SECOND, tolerance);
    if(check_norm != false)
    {
    	std::cerr << "equals does not work correctly" << std::endl;
		delete vec1;
		delete vec2;
		delete vec3;
		return;
    }
    printOk(__LINE__);

    std::cout <<"Test ok" << std::endl;
    delete vec1;
    delete vec2;
    delete vec3;
    delete plgr;
}


// Написать тесты для сета, для компакта
int main(void)
{
	testVector();

	return EXIT_SUCCESS;
}



