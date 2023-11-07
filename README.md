# SimpleTests
Simple single header C++ test framework

Simple Tests is a simple single header testing library that I made because I'm too stupid to use Catch2, googletest, 
or the other cool testing frameworks.

# Installation instructions
Download `SimpleTests/simpleTests.h` and include it in your project and build system

# How-to:
## Declare a test
```c++
#include "simpleTest.h"

ST_TEST(ATestName, "Some description on what the test does")
{
    ST_ASSERT(true, "this test cannot fail.")
}
```

## Declare a section
```c++
#include "simpleTest.h"

ST_SECTION_BEGIN(MySection, "My section's description")
{
     ST_BEFORE_EACH(MyBeforeEachFunctor)
     {
         // Initialize my tests' context
     }

     ST_TEST(MyTest, "My test's description")
     {
         ST_ASSERT(true, "all is good.");
     }

     ST_AFTER_EACH(MyAfterEachFunctor)
     {
         // clean up my tests' context
     }
}
ST_SETION_END(MySection)
```
## Run a single Test or Section
Use `ST_TEST_ONLY` and/or `ST_SECTION_BEGIN_ONLY` to isolate tests. This is useful when debugging a single test or section. There's no need to run all the tests everytime if you're iterating.
```c++
#include "simpleTests.h"

ST_SECTION_BEGIN(OnlySection, "Test only flow")
{
	// this test will run
	ST_TEST_ONLY(OnlyTest, "OnlyTest, should pass")
	{
		ST_ASSERT(true, "assert true");
	}

	// this test will not run
	ST_TEST(NotOnlyTest, "Should not run, if it does, fails.")
	{
		ST_ASSERT(false, "assert false");
	}
}
ST_SECTION_END(OnlySection)

ST_SECTION_BEGIN(OnlySubSection, "Test only flow with subsections")
{
	// this whole section will run
	ST_SECTION_BEGIN_ONLY(OnlySubSubSection, "Test only flow with subsections")
	{
		ST_TEST(TestInOnlySubsection, "Only subsection Test, should pass")
		{
			ST_ASSERT(true, "assert true");
		}
	}
	ST_SECTION_END(OnlySubSubSection)
}
ST_SECTION_END(OnlySubSection)
```

## Run all tests
```c++
int main()
{
    return SimpleTests::SimpleTestsRunner::runTests();
}
```

## Exemple output:
```
[--------] Global
[ FAILED ] |--Test1: should fail
[ ASSERT ] |--[ ASSERT ] Failed: assert false (C:\Repository\SimpleTests\SampleProject\main.cpp:5)
[   ok   ] |--Test2: should pass
[   ok   ] |--SomeNotFancyTest: Should not be initialized
[   ok   ] |--SomeTestInAnotherFile2: should pass
[--------] Section1: This is the first section
[   ok   ] |--SomeTestInASection: should pass
[--------] Section2: This is another section, more to come
[--------] |--Section2_1: this is a subsection
[   ok   ] |--|--SomeTestInASubSection: should fail
[--------] |--|--Section2_1_1: yet another subsection
[   ok   ] |--|--|--SomeTestInSection2_1_1: Should pass
[--------] SomeFancySection: This is the fancy section
[   ok   ] |--SomeFancyTest: Should be initialized


7 ouf of 8 tests passed.
```
