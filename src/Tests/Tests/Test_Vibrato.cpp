#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>

#include "UnitTest++.h"

#include "Synthesis.h"
#include "Vector.h"
#include "ErrorDef.h"

#include "Vibrato.h"

SUITE(Vibrato)
{
    struct VibratoData
    {
        VibratoData()
        {
            // setup
            // e.g., allocate a vibrato object and test signal (newly created for each test case)
        }

        ~VibratoData()
        {
            // teardown
            // e.g., deallocate the vibrato object and test signal
        }

        // e.g., a reusable process() function

        // e.g., a member vibrato object to be reused in each test
    };

    TEST(MyTestWithNoFixture)
    {
        // e.g., allocate & deallocate local data for testing
    }

    TEST_FIXTURE(VibratoData, MyTestWithFixture)
    {
        // e.g., you can use the "VibratoData" contents
    }
}

#endif //WITH_TESTS