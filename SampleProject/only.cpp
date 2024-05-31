#include <ManiTests/ManiTests.h>

// Uncomment me to try the _ONLY flow
//MANI_SECTION_BEGIN(OnlySection, "Test only flow")
//{
//	MANI_TEST_ONLY(OnlyTest, "OnlyTest, should pass")
//	{
//		MANI_TEST_ASSERT(true, "assert true");
//	}
//
//	MANI_TEST(NotOnlyTest, "Should not run, if it does, fails.")
//	{
//		MANI_TEST_ASSERT(false, "assert false");
//	}
//}
//MANI_SECTION_END(OnlySection)
//
//MANI_SECTION_BEGIN(OnlySubSection, "Test only flow with subsections")
//{
//	MANI_SECTION_BEGIN_ONLY(OnlySubSubSection, "Test only flow with subsections")
//	{
//		MANI_TEST(TestInOnlySubsection, "Only subsection Test, should pass")
//		{
//			MANI_TEST_ASSERT(true, "assert true");
//		}
//	}
//	MANI_SECTION_END(OnlySubSubSection)
//}
//MANI_SECTION_END(OnlySubSection)
