#pragma once

#include <functional>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>


// # SimpleTests
// Simple single header C++ test framework
// 
// Simple Tests is a simple single header testing library that I made because I'm too stupid to use Catch2, googletest, 
// or the other cool testing frameworks.
// 
// # Installation instructions
// Download `SimpleTests/simpleTests.h` and include it in your project and build system
// 
// # How - to:
// ## Declare a test
// ```c++
// #include "simpleTest.h"
// 
// ST_TEST(ATestName, "Some description on what the test does")
// {
//     ST_ASSERT(true, "this test cannot fail.")
// }
// ```
// 
// ## Run all tests
// ```c++
// int main()
// {
//     return SimpleTests::SimpleTestsRunner::runTests();
// }
// ```
// 
// ## Exemple output :
// ```
// Test1 : should fail X FAILED
// Failed : assert false (C : \Repository\SimpleTests\SampleProject\main.cpp:5)
// Test2 : should pass +
// SomeTestInAnotherFile1 : should fail X FAILED
// Failed : another assert false (C : \Repository\SimpleTests\SampleProject\tests.cpp:4)
// SomeTestInAnotherFile2 : should pass +
// 2 ouf of 4 tests passed.
// ```


/*
 * ###############################################################
 * Simple Tests Core
 * ###############################################################
 */
namespace SimpleTests
{
    // Test container
    struct SimpleTest
    {
        SimpleTest(const std::string& inTitle, const std::string& inDescription,  std::function<void()> inF)
            : title(inTitle), description(inDescription), f(inF) {}

        std::string title;
        std::string description;
        std::function<void()> f;
    };

    // Simple test's runner 
    struct SimpleTestsContext
    {
        // registers a new test case
        static void registerTest(const std::string& title, const std::string& description, std::function<void()> func)
        {
            getTests().push_back({title, description, func});
        }

        // pushes an assert message in the assert logs queue. it will be consumed once the current test is completed
        static void notifyAssertFailed(const std::string& message)
        {
            getAssertLogs().push(message);
        }

        static std::vector<SimpleTest>& getTests() 
        {
            static std::vector<SimpleTest> s_tests;
            return s_tests;
        }
        
        static std::queue<std::string>& getTestLogs() 
        {
            static std::queue<std::string> s_testLogs;
            return s_testLogs;
        }

        static std::queue<std::string>& getAssertLogs() 
        {
            static std::queue<std::string> s_assertLogs;
            return s_assertLogs;
        }
    };

    struct SimpleTestsRunner
    {
// Colors
#define RED         "\033[31m"      /* Red */
#define GREEN       "\033[32m"      /* Green */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define RESET       "\033[0m"

        // Executes all tests in s_tests
        static int runTests()
        {
            auto& tests = SimpleTestsContext::getTests();
            auto& testLogs = SimpleTestsContext::getTestLogs();
            auto& assertLogs = SimpleTestsContext::getAssertLogs();

            int failedTestCounter = 0;
            for (auto& test : tests)
            {
                // format the test title
                std::stringstream testTitleStream;
                testTitleStream << test.title;
                if (!test.description.empty())
                {
                    // append the description if has one
                    testTitleStream << ": " << test.description;
                }

                // run the test
                test.f();
                                
                // if atleast one assert log was pushed in the s_assertLogs, the test has failed.
                if (assertLogs.size() > 0)
                {
                    testTitleStream << BOLDRED << " X" << RED << " FAILED" << RESET;
                }
                else
                {
                    testTitleStream << BOLDGREEN << " +" << RESET;
                }
                testTitleStream << "\n";

                // push the test's title in the test logs queue
               testLogs.push(testTitleStream.str());

                if (assertLogs.size() > 0)
                {
                    // increment the failed test counter for the final display
                    failedTestCounter++;

                    // dump all assert logs in the test logs queue (after the title)
                    while (assertLogs.size() > 0)
                    {
                        std::stringstream ss;
                        ss << RED << assertLogs.front() << RESET;
                       testLogs.push(ss.str());
                        assertLogs.pop();
                    }
                }
            }

            // dump the whole test logs in std::cout
            while (testLogs.size() > 0)
            {
                std::cout <<testLogs.front();
               testLogs.pop();
            }

            // display the final test success count.
            std::cout << tests.size() - failedTestCounter << " ouf of " << tests.size() << " test";
            if (tests.size() > 1)
            {
                // I'm a gentleman.
                std::cout << "s";
            }
            std::cout <<" passed." << "\n";

            return failedTestCounter > 0 ? 0 : -1;
        }
    };

    struct AutoRegister
    {
        AutoRegister(const std::string& title, const std::string& description, std::function<void()> func)
        {
            SimpleTestsContext::registerTest(title, description, func);
        };
    };
}

/*
 * ###############################################################
 * Simple Tests macros
 * ###############################################################
 */
#define ST_TEST(TESTNAME, DESCRIPTION) \
    static void TESTNAME(); \
    static SimpleTests::AutoRegister autoRegister_##TESTNAME(#TESTNAME, DESCRIPTION, TESTNAME);\
    static void TESTNAME()

#define ST_ASSERT(EXPRESSION, DESCRIPTION) \
    if (!(EXPRESSION)) \
    {\
        std::stringstream ss;\
        ss << "Failed: " << DESCRIPTION << " (" << __FILE__ << ":" << __LINE__ << ")\n";\
        SimpleTests::SimpleTestsContext::notifyAssertFailed(ss.str());\
    }