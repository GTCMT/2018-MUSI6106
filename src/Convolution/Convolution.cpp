
// standard headers

// project headers
#include "MUSI6106Config.h"
#include "ErrorDef.h"

#include "Convolution.h"

CConvolution::CConvolution() : m_iOutputLength(0),
                               m_pfIrSig(nullptr)
{

}

CConvolution::~CConvolution() {

}

Error_t CConvolution::storeIr(float *fIrSig, int iIrLength) {

    return kNoError;
}

Error_t CConvolution::process(float *fInput, float *fOutput, int iInputLength) {
    return kNoError;
}

Error_t CConvolution::getOutputLengthForInput(int iInputLength) {
    return kFunctionIllegalCallError;
}

