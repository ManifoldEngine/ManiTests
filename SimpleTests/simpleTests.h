#pragma once

#include <functional>
#include <string>
#include <vector>
#include <queue>
#include <iostream>
#include <sstream>
#include <map>


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
// ## Declare a section
// ```c++
// #include "simpleTest.h"
// 
// ST_SECTION_BEGIN(MySection, "My section's description")
//
//      ST_BEFORE_EACH(MyBeforeEachFunctor)
//      {
//          // Initialize my tests' context
//      }
// 
//      ST_TEST(MyTest, "My test's description")
//      {
//          ST_ASSERT(true, "all is good.");
//      }
// 
//      ST_AFTER_EACH(MyBeforeEachFunctor)
//      {
//          // clean up my tests' context
//      }
// 
// ST_SETION_END(MySection)
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

    // section container
    struct Section
    {
        Section(const std::string& inTitle, const std::string& inDescription)
          : title(inTitle), description(inDescription)
        {

        };

        std::string title;
        std::string description;
        std::function<void()> onBeforeEach;
        std::function<void()> onAfterEach;

        std::vector<SimpleTest> tests;
        std::vector<Section> children;

    private:
        int id = -1;
    };

    // Simple's test context. this struct exposes all the Simple tests' state as static function. This way they're initialized when we call
    // the getter for the first time, avoiding initialization order bugs. 
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

        static void beginSection(const std::string& title, const std::string& description)
        {
            Section s { title, description };
            Section* pCurrent = getSectionStack().back();
            pCurrent->children.push_back(std::move(s));
            
            // this is safe because the parent cannot reallocate its children vector unless it's the current scope.
            getSectionStack().push_back(&pCurrent->children.back());
        }

        static void registerBeforeEach(std::function<void()> f)
        {
            getSectionStack().back()->onBeforeEach = f;
        }

        static void registerAfterEach(std::function<void()> f)
        {
            getSectionStack().back()->onAfterEach = f;
        }

        static void endSection()
        {
            SimpleTestsContext::getSectionStack().pop_back();
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

        static std::vector<Section*>& getSectionStack()
        {
            static std::vector<Section*> s_sectionStack = { &getGlobalSection() };
            return s_sectionStack;
        }

        static Section& getGlobalSection()
        {
            static Section s_global{ "Global", "" };
            return s_global;
        }

        static uint32_t& getFailedTestCounter()
        {
            static uint32_t s_failedTestCounter;
            return s_failedTestCounter;
        }

    private:
        static std::vector<SimpleTest>& getTests()
        {
            return getSectionStack().back()->tests;
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
            Section global = SimpleTestsContext::getGlobalSection();
            auto& testLogs = SimpleTestsContext::getTestLogs();
            auto& assertLogs = SimpleTestsContext::getAssertLogs();
            int failedTestCounter = 0;

            std::vector<const Section*> sectionStack = { &global };
            runSection(global, sectionStack, true);
            
            dumpTestLogs();

            const size_t totalTests = countTotalTest(global);

            // display the final test success count.
            std::cout << totalTests - failedTestCounter << " ouf of " << totalTests << " test";
            if (totalTests > 1)
            {
                // I'm a gentleman.
                std::cout << "s";
            }
            std::cout <<" passed." << "\n";

            return failedTestCounter > 0 ? 0 : -1;
        }
    
        static void runSection(const Section& section, std::vector<const Section*>& sectionStack, bool bIsInGlobalScope)
        {
            // run a single section
            std::string indent = "";
            if (!bIsInGlobalScope)
            {
                // if we're not in the global scope, we indent once per section
                for (int i = 1; i < sectionStack.size() - 1; ++i)
                {
                    indent += "\t";
                }

                // display the section title and description
                std::cout << indent << section.title << (section.description.empty() ? "" : ": " + section.description) << "\n";
            }

            if (!bIsInGlobalScope)
            {
                indent += "\t";
            }

            if (section.tests.size() > 0)
            {
                // the this section's tests
                runTests(section.tests, sectionStack, indent);

                std::cout << "\n";
            }

            sectionStack.push_back(&section);
            for (const Section& child : section.children)
            {
                // run the children sections
                runSection(child, sectionStack, false);
            }
        
            sectionStack.pop_back();
        }

        static void runTests(const std::vector<SimpleTest>& tests, const std::vector<const Section*>& sectionStack, const std::string& indent)
        {
            for (auto& test : tests)
            {
                // format the test title
                std::stringstream testTitleStream;
                testTitleStream << indent << test.title;
                if (!test.description.empty())
                {
                    // append the description if has one
                    testTitleStream << ": " << test.description;
                }

                // run the test
                for (const Section* section : sectionStack)
                {
                    if (section->onBeforeEach)
                    {
                        section->onBeforeEach();
                    }
                }

                test.f();

                for (const Section* section : sectionStack)
                {
                    if (section->onAfterEach)
                    {
                        section->onAfterEach();
                    }
                }

                auto& assertLogs = SimpleTestsContext::getAssertLogs();
                auto& testLogs = SimpleTestsContext::getTestLogs();
                // if atleast one assert log was pushed in the s_assertLogs, the test has failed.
                if (SimpleTestsContext::getAssertLogs().size() > 0)
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
                    SimpleTestsContext::getFailedTestCounter()++;

                    // dump all assert logs in the test logs queue (after the title)
                    while (assertLogs.size() > 0)
                    {
                        std::stringstream ss;
                        ss << RED << indent << assertLogs.front() << RESET;
                        testLogs.push(ss.str());
                        assertLogs.pop();
                    }
                }

                dumpTestLogs();
            }
        }
    
        static void dumpTestLogs()
        {
            auto& testLogs = SimpleTestsContext::getTestLogs();
            // dump the whole test logs in std::cout
            while (testLogs.size() > 0)
            {
                std::cout << testLogs.front();
                testLogs.pop();
            }
        }

        static size_t countTotalTest(const Section& section)
        {
            size_t result = 0;
            for (const Section& child : section.children)
            {
                result += countTotalTest(child);
            }

            result += section.tests.size();
            return result;
        }
    };

    struct AutoRegister
    {
        AutoRegister(const std::string& title, const std::string& description, std::function<void()> func)
        {
            SimpleTestsContext::registerTest(title, description, func);
        };
    };

    struct SectionBeginner
    {
        SectionBeginner(const std::string& title, const std::string& description)
        {
            SimpleTestsContext::beginSection(title, description);
        }
    };

    struct SectionBeforeEachRegister
    {
        SectionBeforeEachRegister(std::function<void()> f)
        {
            SimpleTestsContext::registerBeforeEach(f);
        }
    };

    struct SectionAfterEachRegister
    {
        SectionAfterEachRegister(std::function<void()> f)
        {
            SimpleTestsContext::registerAfterEach(f);
        }
    };

    struct SectionEnder
    {
        SectionEnder()
        {
            SimpleTestsContext::endSection();
        }
    };
}

/*
 * ###############################################################
 * Simple Tests macros
 * ###############################################################
 */

// One level of macro indirection is required in order to resolve __COUNTER__,
// and get varname1 instead of varname__COUNTER__.
#define ST_LINE() __LINE__
#define ST_UNIQUE_NAME(base) base ## ST_LINE()

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

#define ST_SECTION_BEGIN(SECTIONNAME, DESCRIPTION) \
    static SimpleTests::SectionBeginner sectionBeginner_##SECTIONNAME(#SECTIONNAME, DESCRIPTION);

#define ST_SECTION_END(SECTIONNAME) \
    static SimpleTests::SectionEnder sectionEnder_##SECTIONNAME;
    
#define ST_BEFORE_EACH(FUNCTORNAME) \
    static void FUNCTORNAME(); \
    static SimpleTests::SectionBeforeEachRegister beforeEach_##FUNCTORNAME(FUNCTORNAME); \
    static void FUNCTORNAME()

#define ST_AFTER_EACH(FUNCTORNAME) \
    static void FUNCTORNAME(); \
    static SimpleTests::SectionAfterEachRegister afterEach_##FUNCTORNAME(FUNCTORNAME); \
    static void FUNCTORNAME()
