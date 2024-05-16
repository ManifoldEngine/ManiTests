# ManiTests
Simple single header C++ test library

ManiTests is a simple single header testing library that I made because I'm too stupid to figure out Catch2, googletest, 
or the other cool testing frameworks.

# Installation instructions
include `include/ManiTests/ManiTests.h`

# How - to:
## Declare a test
```c+ +
#include <ManiTests/ManiTests.h>

MANI_TEST(ATestName, "Some description on what the test does")
{
    MANI_ASSERT(true, "this test cannot fail.")
}
```

## Declare a section
```c+ +
#include <ManiTests/ManiTests.h>

MANI_SECTION_BEGIN(MySection, "My section's description")
{
    MANI_BEFORE_EACH(MyBeforeEachFunctor)
    {
        // Initialize my tests' context
    }

    MANI_TEST(MyTest, "My test's description")
    {
        MANI_ASSERT(true, "all is good.");
    }

    MANI_AFTER_EACH(MyAfterEachFunctor)
    {
        // clean up my tests' context
    }
}
MANI_SECTION_END(MySection)
```
## Run a single Test or Section
Use `MANI_TEMANI_ONLY` and /or `MANI_SECTION_BEGIN_ONLY` to isolate tests.This is useful when debugging a single test or section.There's no need to run all the tests everytime if you're iterating.
```c+ +
#include <ManiTests/ManiTests.h>

MANI_SECTION_BEGIN(OnlySection, "Test only flow")
{
    // this test will run
    MANI_TEST_ONLY(OnlyTest, "OnlyTest, should pass")
    {
        MANI_ASSERT(true, "assert true");
    }

    // this test will not run
    MANI_TEST(NotOnlyTest, "Should not run, if it does, fails.")
    {
        MANI_ASSERT(false, "assert false");
    }
}
MANI_SECTION_END(OnlySection)

MANI_SECTION_BEGIN(OnlySubSection, "Test only flow with subsections")
{
    // this whole section will run
    MANI_SECTION_BEGIN_ONLY(OnlySubSubSection, "Test only flow with subsections")
    {
        MANI_TEST(TestInOnlySubsection, "Only subsection Test, should pass")
        {
            MANI_ASSERT(true, "assert true");
        }
    }
    MANI_SECTION_END(OnlySubSubSection)
}
MANI_SECTION_END(OnlySubSection)
```

## Run all tests
```c+ +
int main()
{
    return ManiTests::ManiTestsRunner::runTests();
}
```

## Exemple output :
```
[--------] Global
[ FAILED ] |--Test1: should fail
[ ASSERT ] |--[ASSERT] Failed : assert false (C:\Repository\ManiTests\SampleProject\main.cpp:5)
[   ok   ] |--Test2 : should pass
[   ok   ] |--SomeNotFancyTest : Should not be initialized
[   ok   ] |--SomeTestInAnotherFile2 : should pass
[--------] Section1 : This is the first section
[   ok   ] |--SomeTestInASection : should pass
[--------] Section2 : This is another section, more to come
[--------] |--Section2_1 : this is a subsection
[   ok   ] |-- |--SomeTestInASubSection : should fail
[--------] |-- |--Section2_1_1 : yet another subsection
[   ok   ] |-- |-- |--SomeTestInSection2_1_1 : Should pass
[--------] SomeFancySection : This is the fancy section
[   ok   ] |--SomeFancyTest : Should be initialized


7 ouf of 8 tests passed.
```