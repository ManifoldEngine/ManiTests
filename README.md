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

ST_SETION_END(MySection)
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
Test1: should fail X FAILED
Failed: assert false (C:\Repository\SimpleTests\SampleProject\main.cpp:5)
Test2: should pass +
SomeTestInAnotherFile1: should fail X FAILED
Failed: another assert false (C:\Repository\SimpleTests\SampleProject\tests.cpp:4)
SomeTestInAnotherFile2: should pass +
2 ouf of 4 tests passed.
```
