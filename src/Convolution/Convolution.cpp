
// standard headers

// project headers
#include "MUSI6106Config.h"
#include "ErrorDef.h"

#include "Convolution.h"

CConvolution::CConvolution() : m_iOutputLength(0),
                               m_pfIrSig(0)
{

}

CConvolution::~CConvolution() {

}

Error_t CConvolution::storeIr(float *pfIrSig, int iIrLength) {

    return kNoError;
}

Error_t CConvolution::process(float *pfInput, float *pfOutput, int iInputLength) {
    return kNoError;
}

int CConvolution::getOutputLengthForInput(int iInputLength) {
    return 0;
}

