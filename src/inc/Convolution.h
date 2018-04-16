#if !defined(__Convolution_hdr__)
#define __Convolution_hdr__

#include "ErrorDef.h"

class CConvolution
{
public:
    CConvolution();
    ~CConvolution();

    Error_t reset();

    Error_t storeIr(float *pfIrSig, int iIrLength);

    Error_t process(float *pfInput, float *pfOutput, int iInputLength);

    int getOutputLengthForInput(int iInputLength);

private:
    int m_iOutputLength;
    float *m_pfIrSig;
    float *m_pfTail;
};

#endif // #if !defined(__Convolution_hdr__)