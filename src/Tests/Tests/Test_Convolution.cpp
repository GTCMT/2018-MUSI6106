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

    TEST_FIXTURE(ConvData, ConvNoiseImpulse) {
        int iInputLen = 100;
        float *pfNoiseInput = new float[iInputLen];
        CSynthesis::generateNoise(pfNoiseInput, iInputLen, 1.f);

        int iIrLen = 1;
        float *pfIrSig = new float[iIrLen];
        std::fill_n(pfIrSig, iIrLen, 0.f);
        pfIrSig[0] = 1.f;

        m_phConv->storeIr(pfIrSig, iIrLen);

        int iOutputLen = m_phConv->getOutputLengthForInput(iInputLen);
        float *pfOutput = new float[iOutputLen];
        std::fill_n(pfOutput, iOutputLen, 0.f);

        m_phConv->process(pfNoiseInput, pfOutput, iInputLen);
        
        for (int i = 0; i < iOutputLen; i++) {
            CHECK_EQUAL(pfNoiseInput[i], pfOutput[i]);
        }

        delete [] pfNoiseInput; pfNoiseInput = nullptr;
        delete [] pfIrSig; pfIrSig = nullptr;
        delete [] pfOutput; pfOutput = nullptr;
    }
}

#endif //WITH_TESTS