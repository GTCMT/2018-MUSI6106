#include "ErrorDef.h"
#include "Ppm.h"

Error_t CPpm::createInstance(CPpm *&pCPpm)
{
    pCPpm = new CPpm ();

    if (!pCPpm)
        return kUnknownError;

    return kNoError;
}

Error_t CPpm::destroyInstance(CPpm *&pCPpm)
{
    if (!pCPpm)
        return kUnknownError;

    delete pCPpm;
    pCPpm = 0;

    return kNoError;
}

CPpm::CPpm()
{

}

CPpm::~CPpm()
{

}

Error_t CPpm::process(float **ppfInputBuffer, int iNumberOfFrames)
{
    return kNoError;
}
