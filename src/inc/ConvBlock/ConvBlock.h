#if !defined(__ConvBlock_hdr__)
#define __ConvBlock_hdr__

#include <string>
#include "ErrorDef.h"

class CConvFir;
class CAudioFileIf;

class CConvBlock
{
public:
    CConvBlock(std::string cFilePath, int iStartIdx, int iLengthOfBlock, std::string cIdentifier = "ConvBlock Instance");
    virtual ~CConvBlock();

    std::string getIdentifier () const;

    Error_t setIr(float **ppfImpulseResponse, int iLengthOfIr);

    Error_t getResult(float **&ppfOutputData, int &iNumChannels, int &iLength) const;
    float   getAbsMax() const;

    Error_t process ();

private:
    CConvBlock() {};
    float **m_ppfOutputData;
    float **m_ppfProcessData;

    CConvFir **m_ppFir;
    CAudioFileIf *m_pCAudioFile;

    int m_iStartIdx, 
        m_iLengthOfBlock,
        m_iLengthOfIr,
        m_iNumChannels;

    std::string m_cIdentifier;
};
#endif // __ConvBlock_hdr__