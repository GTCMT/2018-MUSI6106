#if !defined(__ConvFir_hdr__)
#define __ConvFir_hdr__

#include "ErrorDef.h"

template<class T> class CRingBuffer;

class CConvFir
{
public:
//#define WITH_RINGBUFFER 1

    CConvFir();
    virtual ~CConvFir();

    Error_t setImpulseResponse(float *pfImpulseResponse, int iLengthOfIr);
    Error_t reset ();

    int     getTailLength () const;

    Error_t process (const float *pfIn, float *pfOut, int iNumSamples);

    Error_t flush (float *pfOut);

private:
    float *m_pfImpulseResponse;
    CRingBuffer<float> *m_pCRingBuff;

    float *m_pfFilterBuff;

    int m_iLengthOfIr;
};
#endif // __ConvFir_hdr__