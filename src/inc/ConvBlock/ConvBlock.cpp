
// standard headers
#include <cassert>

// project headers
#include "MUSI6106Config.h"

#include "ErrorDef.h"

#include "AudioFileIf.h"
#include "ConvFir.h"
#include "ConvBlock.h"

static const int kBlockLength = 1024;

CConvBlock::CConvBlock (std::string cFilePath, int iStartIdx, int iLengthOfBlock, std::string cIdentifier /*= "ConvBlock Instance"*/) :
    m_ppfOutputData(0),
    m_ppfProcessData(0),
    m_ppFir(0),
    m_pCAudioFile(0),
    m_iStartIdx(iStartIdx), 
    m_iLengthOfBlock(iLengthOfBlock),
    m_iLengthOfIr(0),
    m_iNumChannels(0),
    m_cIdentifier(cIdentifier)
{
    CAudioFileIf::FileSpec_t stFileSpec;

    CAudioFileIf::createInstance(m_pCAudioFile);
    m_pCAudioFile->openFile(cFilePath, CAudioFileIf::kFileRead);
    m_pCAudioFile->setPosition(static_cast<long long>(m_iStartIdx));

    m_pCAudioFile->getFileSpec(stFileSpec);
    m_iNumChannels  = stFileSpec.iNumChannels;

    m_ppFir = new CConvFir* [m_iNumChannels];

    m_ppfOutputData = new float* [m_iNumChannels];
    memset (m_ppfOutputData, 0, sizeof(float*)*m_iNumChannels);
    m_ppfProcessData = new float* [m_iNumChannels];
    for (int i = 0; i < m_iNumChannels; i++)
    {
        m_ppFir[i]          = new CConvFir();
        m_ppfProcessData[i] = new float [kBlockLength];
    }
}


CConvBlock::~CConvBlock ()
{
    m_pCAudioFile->closeFile();
    CAudioFileIf::destroyInstance(m_pCAudioFile);

    for (int i=0; i < m_iNumChannels; i++)
    {
        delete m_ppFir[i];
        delete [] m_ppfOutputData[i];
        delete [] m_ppfProcessData[i];
    }

    delete [] m_ppFir;
    delete [] m_ppfOutputData;
    delete [] m_ppfProcessData;
}

Error_t CConvBlock::setIr( float **ppfImpulseResponse, int iLengthOfIr )
{
    if (!ppfImpulseResponse || iLengthOfIr < 0)
        return kFunctionInvalidArgsError;

    m_iLengthOfIr   = iLengthOfIr;

    // copy IR data
    for (int i = 0; i < m_iNumChannels; i++)
    {
        assert(ppfImpulseResponse[i]);
        m_ppFir[i]->setImpulseResponse(ppfImpulseResponse[i], m_iLengthOfIr);

        // allocate output data
        m_ppfOutputData[i]  = new float [m_iLengthOfIr-1 + m_iLengthOfBlock];
    }


    return kNoError;
}

Error_t CConvBlock::process()
{
    int iSampleCount = 0;

    while(iSampleCount < m_iLengthOfBlock)
    {
        int iNumSamplesRead = std::min(kBlockLength, m_iLengthOfBlock - iSampleCount);

        m_pCAudioFile->readData(m_ppfProcessData, iNumSamplesRead);

        if (!iNumSamplesRead)
            break;

        for (int i = 0; i < m_iNumChannels; i++)
            m_ppFir[i]->process(m_ppfProcessData[i], 
            &m_ppfOutputData[i][iSampleCount], 
            iNumSamplesRead);
        
        iSampleCount += iNumSamplesRead;
    }

    assert (iSampleCount == m_iLengthOfBlock);
    for (int i = 0; i < m_iNumChannels; i++)     
        m_ppFir[i]->flush(&m_ppfOutputData[i][iSampleCount]);

    return kNoError;
}

Error_t CConvBlock::getResult( float **&ppfOutputData, int &iNumChannels, int &iLength ) const
{
    ppfOutputData   = m_ppfOutputData;
    iNumChannels    = m_iNumChannels;
    iLength         = m_iLengthOfIr-1 + m_iLengthOfBlock;

    return kNoError;
}

std::string CConvBlock::getIdentifier() const
{
    return m_cIdentifier;
}

float CConvBlock::getAbsMax() const
{
    float fMax = CUtil::getMax<float>(m_ppfOutputData[0], m_iLengthOfIr-1 + m_iLengthOfBlock,true);
    for (int c = 1; c < m_iNumChannels; c++)
        fMax = std::max(fMax, CUtil::getMax<float>(m_ppfOutputData[c], m_iLengthOfIr-1 + m_iLengthOfBlock,true));
    return fMax;
}
