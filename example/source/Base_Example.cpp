#include <iostream>
#include <ThreadPool.h>
#include <vector>
#include <MutexQueue.h>
#include <IOBuffer.h>
#include <ConfigFile.h>
using namespace XZBEN;


int main()
{
	ConfigFile file;
	file.ReadFile("f:/test.ini");

	std::cout<<file.GetDouble("section1", "value-1", 0)<<std::endl;
	std::cout<<file.GetDouble("section2", "value-1", 0)<<std::endl;
	std::cout<<file.GetDouble("section1", "value-2", 0)<<std::endl;
	std::cout<<file.GetDouble("section1", "value-3", 0)<<std::endl;
	std::cout<<file.GetDouble("section2", "value-2", 0)<<std::endl;
	std::cout<<file.GetDouble("section1", "value-4", 0)<<std::endl;
	std::cout<<file.GetDouble("section2", "value-2", 0)<<std::endl;
	std::cout<<file.GetDouble("section1", "value-5", 0)<<std::endl;
	std::cout<<file.GetDouble("section3", "value-1", 0)<<std::endl;

	getchar();
	return 0;
}
