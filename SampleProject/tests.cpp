#include "simpleTests.h"

ST_TEST(SomeTestInAnotherFile1, "should fail") {
	ST_ASSERT(false, "another assert false");
}

ST_TEST(SomeTestInAnotherFile2, "should pass") {
	ST_ASSERT(true, "all g");
}
