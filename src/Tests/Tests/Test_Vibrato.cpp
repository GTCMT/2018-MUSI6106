#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>
#include <algorithm>

#include "UnitTest++.h"

#include "Synthesis.h"
#include "Vector.h"

#include "Vibrato.h"

SUITE(Vibrato)
{
    struct VibratoData
    {
        VibratoData()  :
            m_pVibrato(0),
            m_ppfInputData(0),
            m_ppfOutputData(0),
            m_kiDataLength(35131),
            m_fMaxModWidth(.5F),
            m_iBlockLength(171),
            m_iNumChannels(3),
            m_fSampleRate(31271),
            m_fModWidth(.1F),
            m_fModFreq(1.F)
        {
            CVibrato::createInstance(m_pVibrato);
            m_pVibrato->initInstance(m_fMaxModWidth,m_fSampleRate,m_iNumChannels);

            m_ppfInputData = new float*[m_iNumChannels];
            m_ppfOutputData = new float*[m_iNumChannels];
            m_ppfInputTmp = new float*[m_iNumChannels];
            m_ppfOutputTmp = new float*[m_iNumChannels];
            for (int i = 0; i < m_iNumChannels; i++)
            {
                m_ppfInputData[i] = new float [m_kiDataLength];
                CSynthesis::generateSine(m_ppfInputData[i], 800, m_fSampleRate, m_kiDataLength, .6F, 0);
                m_ppfOutputData[i] = new float [m_kiDataLength];
                CVector::setZero(m_ppfOutputData[i], m_kiDataLength); 
            }

        }

        ~VibratoData() 
        {
            for (int i = 0; i < m_iNumChannels; i++)
            {
                delete [] m_ppfOutputData[i];
                delete [] m_ppfInputData[i];
            }
            delete [] m_ppfOutputTmp;
            delete [] m_ppfInputTmp;
            delete [] m_ppfOutputData;
            delete [] m_ppfInputData;

            CVibrato::destroyInstance(m_pVibrato);
        }

        void process ()
        {
            int iNumFramesRemaining = m_kiDataLength;
            while (iNumFramesRemaining > 0)
            {
                int iNumFrames = std::min(iNumFramesRemaining, m_iBlockLength);

                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c] = &m_ppfInputData[c][m_kiDataLength - iNumFramesRemaining];
                    m_ppfOutputTmp[c] = &m_ppfOutputData[c][m_kiDataLength - iNumFramesRemaining];
                }
                m_pVibrato->process(m_ppfInputTmp, m_ppfOutputTmp, iNumFrames);

                iNumFramesRemaining -= iNumFrames;
            }
        }

        CVibrato *m_pVibrato;
        float **m_ppfInputData,
            **m_ppfOutputData,
            **m_ppfInputTmp,
            **m_ppfOutputTmp;
        int m_kiDataLength;
        float m_fMaxModWidth;
        int m_iBlockLength;
        int m_iNumChannels;
        float m_fSampleRate;
        float m_fModWidth,
            m_fModFreq;

    };

    TEST_FIXTURE(VibratoData, VibModWidthZero)
    {
        m_pVibrato->setParam(CVibrato::kParamModFreqInHz, 20);
        m_pVibrato->setParam(CVibrato::kParamModWidthInS, 0);

        process();

        int iDelay = CUtil::float2int<int>(m_fMaxModWidth*m_fSampleRate+1);
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_kiDataLength-iDelay, 1e-3F);
    }

    TEST_FIXTURE(VibratoData, VibDc)
    {
        m_pVibrato->setParam(CVibrato::kParamModFreqInHz, 2);
        m_pVibrato->setParam(CVibrato::kParamModWidthInS, .1F);
        for (int c = 0; c < m_iNumChannels; c++)
        {
            CSynthesis::generateDc(m_ppfInputData[c], m_kiDataLength, (c+1)*.1F);
        }

        process();

        int iDelay = CUtil::float2int<int>(m_fMaxModWidth*m_fSampleRate+1);
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_kiDataLength-iDelay, 1e-3F);
    }

    TEST_FIXTURE(VibratoData, VibVaryingBlocksize)
    {
        m_pVibrato->setParam(CVibrato::kParamModFreqInHz, 2);
        m_pVibrato->setParam(CVibrato::kParamModWidthInS, .1F);

        process();

        m_pVibrato->resetInstance();
        m_pVibrato->initInstance(m_fMaxModWidth,m_fSampleRate,m_iNumChannels);
        m_pVibrato->setParam(CVibrato::kParamModFreqInHz, 2);
        m_pVibrato->setParam(CVibrato::kParamModWidthInS, .1F);
        {
            int iNumFramesRemaining = m_kiDataLength;
            while (iNumFramesRemaining > 0)
            {

                int iNumFrames = std::min(iNumFramesRemaining, rand()/RAND_MAX*17000);

                for (int c = 0; c < m_iNumChannels; c++)
                {
                    m_ppfInputTmp[c] = &m_ppfInputData[c][m_kiDataLength - iNumFramesRemaining];
                }
                m_pVibrato->process(m_ppfInputTmp, m_ppfInputTmp, iNumFrames);

                iNumFramesRemaining -= iNumFrames;
            }
        }

        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], m_ppfOutputData[c], m_kiDataLength, 1e-3);
    }

    TEST_FIXTURE(VibratoData, VibParamRange)
    {
        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pVibrato->setParam(CVibrato::kParamModFreqInHz, -1));
        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pVibrato->setParam(CVibrato::kParamModWidthInS, -0.001F));

        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pVibrato->setParam(CVibrato::kParamModFreqInHz, m_fSampleRate));
        CHECK_EQUAL(Error_t::kFunctionInvalidArgsError, m_pVibrato->setParam(CVibrato::kParamModWidthInS, m_fMaxModWidth+.1F));
    }

    TEST_FIXTURE(VibratoData, VibZeroInput)
    {
        m_pVibrato->setParam(CVibrato::kParamModFreqInHz, 20);
        m_pVibrato->setParam(CVibrato::kParamModWidthInS, 0);
        
        for (int c = 0; c < m_iNumChannels; c++)
            CVector::setZero(m_ppfInputData[c], m_kiDataLength);

        process();

        int iDelay = CUtil::float2int<int>(m_fMaxModWidth*m_fSampleRate+1);
        for (int c = 0; c < m_iNumChannels; c++)
            CHECK_ARRAY_CLOSE(m_ppfInputData[c], &m_ppfOutputData[c][iDelay], m_kiDataLength-iDelay, 1e-3F);
    }

}

#endif //WITH_TESTS