#include "simpleTests.h"

ST_SECTION_BEGIN(Section1, "This is the first section")
{
	ST_TEST(SomeTestInASection, "should pass")
	{
		ST_ASSERT(true, "all g");
	}
}
ST_SECTION_END(Section1)

ST_SECTION_BEGIN(Section2, "This is another section, more to come")
{
	ST_SECTION_BEGIN(Section2_1, "this is a subsection")
	{
		ST_TEST(SomeTestInASubSection, "should fail")
		{
			ST_ASSERT(false, "Aieaieaie, attention philippe le paraPET!");
		}

		ST_SECTION_BEGIN(Section2_1_1, "yet another subsection")
		{
			ST_TEST(SomeTestInSection2_1_1, "Should pass")
			{
				ST_ASSERT(true, "all g");
			}
		}
		ST_SECTION_END(Section2_1_1)
	}
	ST_SECTION_END(Section2_1)
}
ST_SECTION_END(Section2)

bool bIsIniitialized = false;

ST_SECTION_BEGIN(SomeFancySection, "This is the fancy section")
{
	ST_BEFORE_EACH(onFancySectionBeforeEach)
	{
		bIsIniitialized = true;
	}

	ST_TEST(SomeFancyTest, "Should be initialized")
	{
		ST_ASSERT(bIsIniitialized, "Should be initialized");
	}

	ST_AFTER_EACH(onFancySectionAfterEach)
	{
		bIsIniitialized = false;
	}
}
ST_SECTION_END(SomeFancySection)

ST_TEST(SomeNotFancyTest, "Should not be initialized")
{
	ST_ASSERT(!bIsIniitialized, "Should not be initialized anymore");
}
