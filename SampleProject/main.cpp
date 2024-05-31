#include <ManiTests/ManiTests.h>
#include <iostream>

MANI_TEST(Test1, "should fail") 
{
	MANI_TEST_ASSERT(false, "assert false");
}

MANI_TEST(Test2, "should pass") 
{
	MANI_TEST_ASSERT(true, "assert true");
}

int main(int argc, char** argv)
{
	return ManiTests::ManiTestsRunner::runTests();
}
