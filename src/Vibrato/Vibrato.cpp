
// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"

#include "RingBuffer.h"

#include "Lfo.h"
#include "Vibrato.h"

static const char*  kCVibratoBuildDate             = __DATE__;


CVibrato::CVibrato () :
    m_bIsInitialized(false),
    m_pCLfo(0),
    m_ppCRingBuff(0),
    m_fSampleRate(44100),
    m_iNumChannels(0)
{

    // this never hurts
    this->resetInstance ();
}


CVibrato::~CVibrato ()
{
    this->resetInstance ();
}

const int  CVibrato::getVersion (const Version_t eVersionIdx)
{
    int iVersion = 0;

    switch (eVersionIdx)
    {
    case kMajor:
        iVersion    = MUSI6106_VERSION_MAJOR; 
        break;
    case kMinor:
        iVersion    = MUSI6106_VERSION_MINOR; 
        break;
    case kPatch:
        iVersion    = MUSI6106_VERSION_PATCH; 
        break;
    case kNumVersionInts:
        iVersion    = -1;
        break;
    }

    return iVersion;
}
const char*  CVibrato::getBuildDate ()
{
    return kCVibratoBuildDate;
}

Error_t CVibrato::createInstance (CVibrato*& pCVibrato)
{
    pCVibrato = new CVibrato ();

    if (!pCVibrato)
        return kUnknownError;


    return kNoError;
}

Error_t CVibrato::destroyInstance (CVibrato*& pCVibrato)
{
    if (!pCVibrato)
        return kUnknownError;
    
    pCVibrato->resetInstance ();
    
    delete pCVibrato;
    pCVibrato = 0;

    return kNoError;

}

Error_t CVibrato::initInstance( float fMaxModWidthInS, float fSampleRateInHz, int iNumChannels )
{
    // set parameters
    m_fSampleRate       = fSampleRateInHz;
    m_iNumChannels      = iNumChannels;

    // set parameter ranges
    m_aafParamRange[kParamModFreqInHz][0] = 0;
    m_aafParamRange[kParamModFreqInHz][1] = fSampleRateInHz*.5F;
    m_aafParamRange[kParamModWidthInS][0] = 0;
    m_aafParamRange[kParamModWidthInS][1] = fMaxModWidthInS;


    // create instances
    m_ppCRingBuff       = new CRingBuffer<float>*[m_iNumChannels];
    for (int c= 0; c < m_iNumChannels; c++)
    {
        m_ppCRingBuff[c]= new CRingBuffer<float>(CUtil::float2int<int>(fMaxModWidthInS*m_fSampleRate*2+1));
        m_ppCRingBuff[c]->setWriteIdx(CUtil::float2int<int>(fMaxModWidthInS*m_fSampleRate+1));
    }

    m_pCLfo             = new CLfo(m_fSampleRate);

    m_bIsInitialized    = true;

    return kNoError;
}

Error_t CVibrato::resetInstance ()
{
    for (int i = 0; i < kNumVibratoParams; i++)
        setParam((VibratoParam_t)i, 0);

    for (int c= 0; c < m_iNumChannels; c++)
        delete m_ppCRingBuff[c];
    delete [] m_ppCRingBuff;
    m_ppCRingBuff       = 0;
    delete m_pCLfo;
    m_pCLfo             = 0;
    
    m_iNumChannels      = 0;
    m_bIsInitialized    = false;

    return kNoError;
}

Error_t CVibrato::setParam( VibratoParam_t eParam, float fParamValue )
{
    if (!m_bIsInitialized)
        return kNotInitializedError;
    if (!isInParamRange(eParam, fParamValue))
        return kFunctionInvalidArgsError;

    switch (eParam)
    {
    case kParamModFreqInHz:
        return m_pCLfo->setParam(CLfo::kLfoParamFrequency, fParamValue);
    case kParamModWidthInS:
        return m_pCLfo->setParam(CLfo::kLfoParamAmplitude, fParamValue * m_fSampleRate);
    case kNumVibratoParams:
        return kFunctionInvalidArgsError;
    }

    return kNoError;
}

float CVibrato::getParam( VibratoParam_t eParam ) const
{
    switch (eParam)
    {
    case kParamModFreqInHz:
        return m_pCLfo->getParam(CLfo::kLfoParamFrequency);
    case kParamModWidthInS:
        return m_pCLfo->getParam(CLfo::kLfoParamAmplitude) / m_fSampleRate;
    }
    return kNoError;
}


bool CVibrato::isInParamRange( VibratoParam_t eParam, float fValue )
{
    if (fValue < m_aafParamRange[eParam][0] || fValue > m_aafParamRange[eParam][1])
    {
        return false;
    }
    else
    {
        return true;
    }
}

Error_t CVibrato::process( float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames )
{
    if (!ppfInputBuffer || !ppfOutputBuffer || iNumberOfFrames < 0)
        return kFunctionInvalidArgsError;

    for (int i = 0; i < iNumberOfFrames; i++)
    {
        float fOffset = m_pCLfo->getNext();
        for (int c = 0; c < m_iNumChannels; c++)
        {
            m_ppCRingBuff[c]->putPostInc(ppfInputBuffer[c][i]);

            ppfOutputBuffer[c][i]   = m_ppCRingBuff[c]->get(fOffset);
            m_ppCRingBuff[c]->getPostInc(); // dummy call to keep write and read idx in sync
        }
    }

    return kNoError;
}
