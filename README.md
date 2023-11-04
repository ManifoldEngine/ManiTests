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
