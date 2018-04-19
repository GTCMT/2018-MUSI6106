

// standard headers

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"
#include "RingBuffer.h"

#include "ConvFir.h"

CConvFir::CConvFir(  ) :     m_pfImpulseResponse(0),
m_pCRingBuff(0),
m_pfFilterBuff(0),
m_iLengthOfIr(0)
{
    this->reset ();
}

CConvFir::~CConvFir()
{
    this->reset ();
}

Error_t CConvFir::setImpulseResponse( float *pfImpulseResponse, int iLengthOfIr )
{
    if (!pfImpulseResponse || iLengthOfIr <= 0)
        return kFunctionInvalidArgsError;

    this->reset ();

    m_pfImpulseResponse = new float [iLengthOfIr];
    m_iLengthOfIr       = iLengthOfIr;
    CUtil::copyBuff(m_pfImpulseResponse, pfImpulseResponse, iLengthOfIr);

#ifdef WITH_RINGBUFFER
    m_pCRingBuff        = new CRingBuffer<float> (m_iLengthOfIr);
#else
    m_pfFilterBuff      = new float [m_iLengthOfIr];
    CUtil::setZero(m_pfFilterBuff, m_iLengthOfIr);
#endif //WITH_RINGBUFFER

    return kNoError;
}

Error_t CConvFir::reset()
{
#ifdef WITH_RINGBUFFER
    delete m_pCRingBuff;
#else
    delete [] m_pfFilterBuff;
#endif //#ifdef WITH_RINGBUFFER
    delete [] m_pfImpulseResponse;

    m_pCRingBuff        = 0;
    m_pfImpulseResponse = 0;
    m_iLengthOfIr       = 0;

    return kNoError;
}

int CConvFir::getTailLength() const
{
    return m_iLengthOfIr - 1;
}

Error_t CConvFir::process( const float *pfIn, float *pfOut, int iNumSamples )
{
    if (!pfIn || !pfOut || !m_pfImpulseResponse || (!m_pCRingBuff && !m_pfFilterBuff))
        return kFunctionInvalidArgsError;

#ifdef WITH_RINGBUFFER
    for (int i = 0; i < iNumSamples; i++)
    {
        m_pCRingBuff->putPostInc(pfIn[i]);
        
        pfOut[i] = 0;
        for (int j = 0; j < m_iLengthOfIr; j++)
        {
            pfOut[i]    += m_pCRingBuff->get(static_cast<float>(-j)) * m_pfImpulseResponse[j];
        }
        m_pCRingBuff->setReadIdx(m_pCRingBuff->getReadIdx() + 1);
    }
#else //WITH_RINGBUFFER
    int iCount = 0;
    int iEndBufferLoop = m_iLengthOfIr-1;

    CUtil::setZero(pfOut, iNumSamples);

    for (int i = 0; i < iNumSamples; i++)
    {
        for (int j = 0; j < m_iLengthOfIr - iEndBufferLoop; j++)
        {
            pfOut[i]    += pfIn[i-j] * m_pfImpulseResponse[j];
        }
        for (int j = m_iLengthOfIr - iEndBufferLoop, k = 0; j < iEndBufferLoop+1; j++,k++)
        {
            pfOut[i]    += m_pfFilterBuff[m_iLengthOfIr-1-k] * m_pfImpulseResponse[j];
        }
        iEndBufferLoop  = std::max(0, iEndBufferLoop - 1);
    }

    iCount = std::max(m_iLengthOfIr - iNumSamples, 0);
    CUtil::moveBuff(m_pfFilterBuff, 
        0, 
        iNumSamples,
        iCount);
    CUtil::copyBuff(&m_pfFilterBuff[iCount], &pfIn[iNumSamples-m_iLengthOfIr+iCount], m_iLengthOfIr - iCount);
#endif //WITH_RINGBUFFER

    return kNoError;
}

Error_t CConvFir::flush( float *pfOut )
{
    if (!pfOut)
        return kFunctionInvalidArgsError;

    float fZero = 0;

    for (int i = 0; i < m_iLengthOfIr-1; i++)
        process(&fZero, &pfOut[i], 1);

    return kNoError;
}
