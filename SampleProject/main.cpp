#include "simpleTests.h"
#include <iostream>

ST_TEST(Test1, "should fail") 
{
	ST_ASSERT(false, "assert false");
}

ST_TEST(Test2, "should pass") 
{
	ST_ASSERT(true, "assert true");
}

int main(int argc, char** argv)
{
	return SimpleTests::SimpleTestsRunner::runTests();
}
