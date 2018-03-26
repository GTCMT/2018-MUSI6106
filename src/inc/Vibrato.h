#if !defined(__Vibrato_hdr__)
#define __Vibrato_hdr__

#include "ErrorDef.h"

class CLfo;
template <class T>
class CRingBuffer;

#include "ErrorDef.h"

class CVibrato
{
public:
    /*! version number */
    enum Version_t
    {
        kMajor, //!< major version number
        kMinor, //!< minor version number
        kPatch, //!< patch version number

        kNumVersionInts
    };

    enum VibratoParam_t
    {
        kParamModWidthInS,
        kParamModFreqInHz,

        kNumVibratoParams
    };
    static const int getVersion (const Version_t eVersionIdx);
    static const char* getBuildDate ();

    static Error_t createInstance (CVibrato*& pCVibrato);
    static Error_t destroyInstance (CVibrato*& pCVibrato);

    Error_t initInstance (float fMaxModWidthInS, float fSampleRateInHz, int iNumChannels);
    Error_t resetInstance ();

    Error_t setParam (VibratoParam_t eParam, float fParamValue);
    float getParam (VibratoParam_t eParam) const;

    Error_t process (float **ppfInputBuffer, float **ppfOutputBuffer, int iNumberOfFrames);

protected:
    CVibrato ();
    virtual ~CVibrato ();

private:
    bool isInParamRange (VibratoParam_t eParam, float fValue);

    bool m_bIsInitialized;

    CLfo *m_pCLfo;
    CRingBuffer<float> **m_ppCRingBuff;

    float   m_fSampleRate;
    int     m_iNumChannels;
    float m_aafParamRange[kNumVibratoParams][2];
};

#endif // #if !defined(__Vibrato_hdr__)