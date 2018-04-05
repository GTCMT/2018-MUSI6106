
#include <ctime>
#include <iostream>
#include <fstream>

#include "MUSI6106Config.h"

#include "Dtw.h"

using std::cout;
using std::endl;

// local function declarations
void    showClInfo ();
void    clLoadMatrixFromFile(float **ppfMatrix, std::ifstream &FFile, int iNumRows, int iNumCols);
void    clLoadIntMatrixFromFile(int **ppiMatrix, std::ifstream &FFile, int iNumRows, int iNumCols);
int     clGetNumCols(std::ifstream &FFile);
int     clGetNumRows(std::ifstream &FFile);

static const int kNumRuns = 100;

/////////////////////////////////////////////////////////////////////////////////
// main function
int main(int argc, char* argv[])
{
    std::string             sMatrixFilePath,                 //!< file paths
        sAlignmentFilePath;

    clock_t                 TimeInTicks = 0;

    CDtw                    DtwInstance ;

    std::ifstream           FMatrixFile,
        FPathFile;

    int                     aiDistanceDim[2],
        aiTargetPathDim[2];

    float   **ppfDistanceMatrix = 0;
    int     **ppiTargetPath     = 0;
    int     **ppiResultPath     = 0;


    showClInfo();


    //////////////////////////////////////////////////////////////////////////////
    // parse command line arguments
    if (argc < 3)
    {
        cout << "Missing target alignment path!";
        return -1;
    }
    else if (argc < 2)
    {
        cout << "Missing distance matrix path!";
        return -1;
    }
    else
    {
        sMatrixFilePath = argv[1];
        sAlignmentFilePath = argv[2];
    }

    //////////////////////////////////////////////////////////////////////////////
    // open the input files
    FMatrixFile.open(sMatrixFilePath, std::ios::out);
    if (!FMatrixFile.is_open())
    {
        cout << endl << "input file (matrix) could not be opened!" << endl << endl;
        exit(-1);
    }
    FPathFile.open(sAlignmentFilePath, std::ios::out);
    if (!FPathFile.is_open())
    {
        cout << endl << "input file (path) could not be opened!" << endl << endl;
        exit(-1);
    }

    //////////////////////////////////////////////////////////////////////////////
    // get the input file dimensions
    aiDistanceDim[0]    = clGetNumRows(FMatrixFile);
    aiDistanceDim[1]    = clGetNumCols(FMatrixFile);
    aiTargetPathDim[0]  = clGetNumRows(FPathFile);
    aiTargetPathDim[1]  = clGetNumCols(FPathFile);

    //////////////////////////////////////////////////////////////////////////////
    // allocate memory
    ppfDistanceMatrix = new float*[aiDistanceDim[0]];
    for (int i = 0; i < aiDistanceDim[0]; i++)
        ppfDistanceMatrix[i] = new float[aiDistanceDim[1]];
    ppiTargetPath = new int*[aiTargetPathDim[0]];
    for (int i = 0; i < aiTargetPathDim[0]; i++)
    {
        ppiTargetPath[i] = new int[aiTargetPathDim[1]];
    }
    ppiResultPath = new int*[aiTargetPathDim[1]];
    for (int i = 0; i < aiTargetPathDim[1]; i++)
    {
        ppiResultPath[i] = new int[aiTargetPathDim[0]];
    }

    //////////////////////////////////////////////////////////////////////////////
    // read data
    clLoadMatrixFromFile(ppfDistanceMatrix, FMatrixFile, aiDistanceDim[0], aiDistanceDim[1]);
    clLoadIntMatrixFromFile(ppiTargetPath, FPathFile, aiTargetPathDim[0], aiTargetPathDim[1]);

    //////////////////////////////////////////////////////////////////////////////
    // init instance
    DtwInstance.init(aiDistanceDim[0], aiDistanceDim[1]);

    TimeInTicks = clock();

    for (int i = 0; i < kNumRuns; i++)
    {
        int     iLength     = 0;

        DtwInstance.process(ppfDistanceMatrix);
        iLength = DtwInstance.getPathLength();

        if (iLength != aiTargetPathDim[0])
        {
            cout << "Invalid Result!" << endl;
            break;
        }

        DtwInstance.getPath(ppiResultPath);

    }
    cout << "Time elapsed: " << (clock() - TimeInTicks)*1.F / CLOCKS_PER_SEC <<endl;

    for (int i = 0; i < aiTargetPathDim[0]; i++)
    {
        for(int j = 0; j < aiTargetPathDim[1]; j++)
            if (ppiResultPath[j][i] != ppiTargetPath[i][j])
            {
                cout << "Incorrect Path!" << endl;
                break;
            }
    }
 
    //////////////////////////////////////////////////////////////////////////////
    // clean-up
    for (int i = 0; i < aiTargetPathDim[0]; i++)
    {
        delete[] ppiTargetPath[i];
    }
    delete[] ppiTargetPath;
    for (int i = 0; i < aiTargetPathDim[1]; i++)
    {
        delete[] ppiResultPath[i];
    }
    delete[] ppiResultPath;
    for (int i = 0; i < aiDistanceDim[0]; i++)
    {
        delete[] ppfDistanceMatrix[i];
    }
    FMatrixFile.close();
    FPathFile.close();

    return 0;

}


void     showClInfo()
{
    cout << "GTCMT MUSI6106 Executable" << endl;
    cout << "(c) 2014-2018 by Alexander Lerch" << endl;
    cout  << endl;

    return;
}

int clGetNumRows(std::ifstream &FFile)
{
    int iNumOfRows = 0;

    // check num of input file elements
    while (!FFile.eof())
    {
        FFile.ignore(INT_MAX, '\n');
        iNumOfRows++;
    }
    iNumOfRows--;
    FFile.clear();
    FFile.seekg(0, std::ios::beg);

    return iNumOfRows;
}

int clGetNumCols(std::ifstream &FFile)
{
    int iNumOfRows = 0,
        iNumOfCols = 0;
    float fDummy;

    // check num of input file elements
    while (!FFile.eof())
    {
        FFile.ignore(INT_MAX, '\n');
        iNumOfRows++;
    }
    iNumOfRows--;
    FFile.clear();
    FFile.seekg(0, std::ios::beg);

    while (!FFile.eof())
    {
        FFile >> fDummy;
        iNumOfCols++;
    }
    iNumOfCols--;
    FFile.clear();
    FFile.seekg(0, std::ios::beg);

    return iNumOfCols / iNumOfRows;
}

void clLoadMatrixFromFile(float **ppfMatrix, std::ifstream &FFile, int iNumRows, int iNumCols)
{
    int i, j;

    // read file
    for (i = 0; i < iNumRows; i++)
    {
        for (j = 0; j < iNumCols; j++)
            FFile >> ppfMatrix[i][j];
    }
    FFile.clear();
    FFile.seekg(0, std::ios::beg);

    return;
}

void clLoadIntMatrixFromFile(int **ppiMatrix, std::ifstream &FFile, int iNumRows, int iNumCols)
{
    int i, j;

    // read file
    for (i = 0; i < iNumRows; i++)
    {
        for (j = 0; j < iNumCols; j++)
        {
            float fTmp = 0;
            FFile >> fTmp;
            ppiMatrix[i][j] = static_cast<int>(fTmp-.5F);
        }
    }
    FFile.clear();
    FFile.seekg(0, std::ios::beg);

    return;
}