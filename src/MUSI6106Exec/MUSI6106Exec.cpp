
#include <iostream>
#include <ctime>

#include "MUSI6106Config.h"

#include "AudioFileIf.h"
#include "Convolution.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sInputFilePath,                 //!< file paths
                            sIrFilePath,
                            sOutputFilePath;

    static const int        kBlockSize = 1024;

    clock_t                 time = 0;

    float                   **ppfInputData = 0;
    float                   **ppfIrData = 0;
    float                   **ppfOutputData = 0;

    CAudioFileIf            *phInputFile = 0;
    CAudioFileIf            *phIrFile = 0;
    CAudioFileIf            *phOutputFile = 0;

    CAudioFileIf::FileSpec_t stInputFileSpec;
    CAudioFileIf::FileSpec_t stIrFileSpec;
    CAudioFileIf::FileSpec_t stOutputFileSpec;

    long long iIrLength = 0;

    CConvolution            *phConvolution = 0;

    showClInfo();

    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 2)
    {
        cout << "Missing audio input path!";
        return -1;
    }
    else if (argc < 3)
    {
        cout << "Missing IR file path!";
        return -1;
    }
    else
    {
        sInputFilePath = argv[1];
        sIrFilePath = argv[2];
        sOutputFilePath = sInputFilePath + "_conv.wav";
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input wave files
    CAudioFileIf::create(phInputFile);
    phInputFile->openFile(sInputFilePath, CAudioFileIf::kFileRead);
    if (!phInputFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phInputFile->getFileSpec(stInputFileSpec);

    CAudioFileIf::create(phIrFile);
    phIrFile->openFile(sIrFilePath, CAudioFileIf::kFileRead);
    if (!phIrFile->isOpen())
    {
        cout << "Wave file open error!";
        return -1;
    }
    phIrFile->getFileSpec(stIrFileSpec);


    //////////////////////////////////////////////////////////////////////////////
    // allocate memory for input audio
    ppfInputData = new float*[stInputFileSpec.iNumChannels];
    for (int i = 0; i < stInputFileSpec.iNumChannels; i++)
        ppfInputData[i] = new float[kBlockSize];

    // allocate memory for IR signal
    phIrFile->getLength(iIrLength);
    ppfIrData = new float*[stIrFileSpec.iNumChannels];
    for (int i = 0; i < stIrFileSpec.iNumChannels; i++)
        ppfIrData[i] = new float[iIrLength];

    // allocate memory for output file

    // allocate memory for convolution object
    phConvolution = new CConvolution();

    time = clock();


    //////////////////////////////////////////////////////////////////////////////
    phIrFile->readData(ppfIrData, iIrLength);

    // change these into block-wise multithread operation
    // only use the first channel for both input and IR (0)

    // get audio data and write it to the output file
    while (!phInputFile->isEof())
    {
        long long iNumFrames = kBlockSize;
        phInputFile->readData(ppfInputData, iNumFrames);

        for (int i = 0; i < iNumFrames; i++)
        {
            phConvolution->process(ppfInputData[0], ppfIrData[0], iNumFrames);
        }
    }

    cout << "reading/writing done in: \t" << (clock() - time)*1.F / CLOCKS_PER_SEC << " seconds." << endl;

    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    CAudioFileIf::destroy(phInputFile);
    CAudioFileIf::destroy(phIrFile);
    CAudioFileIf::destroy(phOutputFile);

    for (int i = 0; i < stInputFileSpec.iNumChannels; i++)
        delete[] ppfInputData[i];
    delete[] ppfInputData;
    ppfInputData = 0;

    for (int i = 0; i < stIrFileSpec.iNumChannels; i++)
        delete[] ppfIrData[i];
    delete[] ppfIrData;
    ppfIrData = 0;

    delete phConvolution;
    phConvolution = 0;

    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2018 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

