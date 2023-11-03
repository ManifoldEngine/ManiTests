#pragma once

#include <functional>
#include <string>
#include <vector>
#include <map>
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
// Colors
#define RED         "\033[31m"      /* Red */
#define GREEN       "\033[32m"      /* Green */
#define BOLD        "\033[1m"
#define RESET       "\033[0m"

// labels
#define ST_DASHES_STRING "[--------] "
#define ST_PASSED_STRING "[   ok   ] "
#define ST_FAILED_STRING "[ FAILED ] "
#define ST_ASSERT_STRING "[ ASSERT ] "


    // Test container
    struct SimpleTest
    {
        SimpleTest(const std::string& inTitle, const std::string& inDescription,  std::function<void()> inF)
            : title(inTitle), description(inDescription), f(inF) {}

        std::string title;
        std::string description;
        std::function<void()> f;
        bool bPassed = false;

        void addAssertLog(const std::string& assertLog)
        {
            assertLogs.push_back(assertLog);
        }

        std::string getLog(const std::string& indent) const
        {
            std::stringstream testStream;
            // if atleast one assert log was pushed in the s_assertLogs, the test has failed.
            if (!bPassed)
            {
                testStream << BOLD << RED << ST_FAILED_STRING << RED << indent << title << ": " << description << RESET << "\n";
                for (const std::string& assertLog : assertLogs)
                {
                    testStream << RED << ST_ASSERT_STRING << indent << assertLog << RESET;
                }
            }
            else
            {
                testStream << BOLD << GREEN << ST_PASSED_STRING << RESET << indent << title << ": " << description << "\n";
            }

            return testStream.str();
        }
    private:
        std::vector<std::string> assertLogs;
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

        bool bPassed = false;

        std::string getLog(const std::string& indent) const
        {
            // display the section title and description
            std::stringstream sectionTitle;
            sectionTitle << (bPassed ? GREEN : RED) << ST_DASHES_STRING << RESET;
            sectionTitle << indent << BOLD << title << (description.empty() ? "" : ": " + description) << RESET << "\n";
            return sectionTitle.str();
        }
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
        // Executes all tests in s_tests
        static int runTests()
        {
            Section global = SimpleTestsContext::getGlobalSection();

            std::vector<const Section*> sectionStack;
            runSection(global, sectionStack, true);
            logSection(global, sectionStack, true);

            size_t totalTests = 0;
            size_t failedTests = 0;
            countTests(global, totalTests, failedTests);
            // display the final test success count.
            std::cout << "\n\n" << totalTests - failedTests << " ouf of " << totalTests << " test";
            if (totalTests > 1)
            {
                // I'm a gentleman.
                std::cout << "s";
            }
            std::cout <<" passed." << "\n";

            return failedTests > 0 ? 0 : -1;
        }
    
        static bool runSection(Section& section, std::vector<const Section*>& sectionStack, bool bIsInGlobalScope)
        {
            bool bPassed = true;

            // the this section's tests
            bPassed &= runTests(section.tests, sectionStack);

            sectionStack.push_back(&section);
            for (Section& child : section.children)
            {
                // run the children sections
                bPassed &= runSection(child, sectionStack, false);
            }

            //std::cout << ST_DASHES_STRING << "\n";
            sectionStack.pop_back();
            section.bPassed = bPassed;
            return bPassed;
        }

        static bool runTests(std::vector<SimpleTest>& tests, const std::vector<const Section*>& sectionStack)
        {
            bool bPassed = true;
            for (auto& test : tests)
            {
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

                test.bPassed = assertLogs.size() == 0;
                bPassed &= test.bPassed;
                if (assertLogs.size() > 0)
                {
                    // increment the failed test counter for the final display
                    SimpleTestsContext::getFailedTestCounter()++;

                    // dump all assert logs in the test logs queue (after the title)
                    while (assertLogs.size() > 0)
                    {
                        std::stringstream ss;
                        ss << RED << ST_ASSERT_STRING << assertLogs.front() << RESET;
                
                        test.addAssertLog(ss.str());
                        assertLogs.pop();
                    }
                }
            }
            return bPassed;
        }

        static void logSection(const Section& section, std::vector<const Section*>& sectionStack, bool bIsInGlobalScope)
        {
            sectionStack.push_back(&section);

            std::string indent = "";
            if (sectionStack.size() > 1)
            {
                for (size_t i = 2; i < sectionStack.size(); ++i)
                {
                    indent += "|--";
                }
            }

            std::cout << section.getLog(indent);

            if (sectionStack.size() > 0)
            {
                indent += "|--";
            }

            for (const auto& test : section.tests)
            {
                std::cout << test.getLog(indent);
            }

            for (const auto& child : section.children)
            {
                logSection(child, sectionStack, false);
            }

            sectionStack.pop_back();
        }

        static void countTests(const Section& section, size_t& outTotalTestCount, size_t& outFailedTotalTestCount)
        {
            for (const Section& child : section.children)
            {
                countTests(child, outTotalTestCount, outFailedTotalTestCount);
            }

            for (const SimpleTest& test : section.tests)
            {
                if (!test.bPassed)
                {
                    outFailedTotalTestCount++;
                }
            }

            outTotalTestCount += section.tests.size();
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
    static SimpleTests::SectionBeginner sectionBeginner_##SECTIONNAME(#SECTIONNAME, DESCRIPTION);\
    namespace

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
