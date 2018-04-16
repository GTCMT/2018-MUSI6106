#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>

#include "UnitTest++.h"

#include "Synthesis.h"
#include "Vector.h"

#include "Convolution.h"

SUITE(Convolution)
{
    struct ConvData {
        ConvData() {
            m_phConv = new CConvolution();
        }

        ~ConvData() {
            delete m_phConv; m_phConv = nullptr;
        }

        CConvolution *m_phConv;
    };

    TEST_FIXTURE(ConvData, getLength) {

    }

    TEST_FIXTURE(ConvData, process) {
        
    }
}

#endif //WITH_TESTS