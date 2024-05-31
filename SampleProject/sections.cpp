#include <ManiTests/ManiTests.h>

MANI_SECTION_BEGIN(Section1, "This is the first section")
{
	MANI_TEST(SomeTestInASection, "should pass")
	{
		MANI_TEST_ASSERT(true, "all g");
	}
}
MANI_SECTION_END(Section1)

MANI_SECTION_BEGIN(Section2, "This is another section, more to come")
{
	MANI_SECTION_BEGIN(Section2_1, "this is a subsection")
	{
		MANI_TEST(SomeTestInASubSection, "should fail")
		{
			MANI_TEST_ASSERT(true, "Aieaieaie, attention philippe le paraPET!");
		}

		MANI_SECTION_BEGIN(Section2_1_1, "yet another subsection")
		{
			MANI_TEST(SomeTestInSection2_1_1, "Should pass")
			{
				MANI_TEST_ASSERT(true, "all g");
			}
		}
		MANI_SECTION_END(Section2_1_1)
	}
	MANI_SECTION_END(Section2_1)
}
MANI_SECTION_END(Section2)

bool bIsIniitialized = false;

MANI_SECTION_BEGIN(SomeFancySection, "This is the fancy section")
{
	MANI_BEFORE_EACH(onFancySectionBeforeEach)
	{
		bIsIniitialized = true;
	}

	MANI_TEST(SomeFancyTest, "Should be initialized")
	{
		MANI_TEST_ASSERT(bIsIniitialized, "Should be initialized");
	}

	MANI_AFTER_EACH(onFancySectionAfterEach)
	{
		bIsIniitialized = false;
	}
}
MANI_SECTION_END(SomeFancySection)

MANI_TEST(SomeNotFancyTest, "Should not be initialized")
{
	MANI_TEST_ASSERT(!bIsIniitialized, "Should not be initialized anymore");
}
