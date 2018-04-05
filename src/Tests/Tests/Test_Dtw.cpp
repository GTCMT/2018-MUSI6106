#include "MUSI6106Config.h"

#ifdef WITH_TESTS
#include <cassert>
#include <cstdio>

#include "UnitTest++.h"

#include "Vector.h"

#include "Dtw.h"

SUITE(Dtw)
{
    struct DtwData
    {
        DtwData() 
        {
            m_pCDtw     = new CDtw ();
            m_ppfData   = 0;
            m_ppiPath   = 0;
        }

        ~DtwData() 
        {
            for (int i = 0; i < m_aiMatrixDimension[0]; i++)
            {
                delete [] m_ppfData[i];
            }
            delete [] m_ppfData;
            m_ppfData   = 0;
            delete m_pCDtw;

            if (m_ppiPath)
                for (int k = 0; k < CDtw::kNumMatrixDimensions; k++)
                    delete [] m_ppiPath[k];
            delete [] m_ppiPath;
        }


        CDtw *m_pCDtw;

        float   **m_ppfData;
        int     **m_ppiPath;

        int     m_aiMatrixDimension[2];
    };


    TEST_FIXTURE(DtwData, AcaExample)
    {
        int iPathLength = 0;


        int aiPathResultRow[5] = {0, 1, 2, 3, 4};
        int aiPathResultCol[5] = {0, 0, 1, 2, 3};

        m_aiMatrixDimension[0] = 5;
        m_aiMatrixDimension[1] = 4;

        m_pCDtw->init (m_aiMatrixDimension[0], m_aiMatrixDimension[1]);

        m_ppfData   = new float*[m_aiMatrixDimension[0]];
        for (int i = 0; i < m_aiMatrixDimension[0]; i++)
            m_ppfData[i]    = new float [m_aiMatrixDimension[1]];

        m_ppfData[0][0] = 0; m_ppfData[0][1] = 1; m_ppfData[0][2] = 2; m_ppfData[0][3] = 1; 
        m_ppfData[1][0] = 1; m_ppfData[1][1] = 2; m_ppfData[1][2] = 3; m_ppfData[1][3] = 0; 
        m_ppfData[2][0] = 1; m_ppfData[2][1] = 0; m_ppfData[2][2] = 1; m_ppfData[2][3] = 2; 
        m_ppfData[3][0] = 2; m_ppfData[3][1] = 1; m_ppfData[3][2] = 0; m_ppfData[3][3] = 3; 
        m_ppfData[4][0] = 0; m_ppfData[4][1] = 1; m_ppfData[4][2] = 2; m_ppfData[4][3] = 1; 

        m_pCDtw->process (m_ppfData);

        iPathLength = m_pCDtw->getPathLength();
        CHECK_EQUAL(5, iPathLength);
        m_ppiPath = new int* [CDtw::kNumMatrixDimensions];
        for (int k = 0; k < CDtw::kNumMatrixDimensions; k++)
            m_ppiPath[k] = new int [iPathLength];

        CHECK_EQUAL(2.F, m_pCDtw->getPathCost());

        m_pCDtw->getPath (m_ppiPath);

        CHECK_ARRAY_EQUAL(aiPathResultRow, m_ppiPath[CDtw::kRow], iPathLength);
        CHECK_ARRAY_EQUAL(aiPathResultCol, m_ppiPath[CDtw::kCol], iPathLength);
    }

    TEST_FIXTURE(DtwData, Api)
    {
        m_aiMatrixDimension[0] = 5;
        m_aiMatrixDimension[1] = 4;

        m_ppfData   = new float*[m_aiMatrixDimension[0]];
        CVector::setZero(m_ppfData,m_aiMatrixDimension[0]);
        CHECK_EQUAL(kNotInitializedError, m_pCDtw->process(m_ppfData));

        CHECK_EQUAL(kFunctionInvalidArgsError, m_pCDtw->init(-1,4));
        CHECK_EQUAL(kFunctionInvalidArgsError, m_pCDtw->init(3,0));

        CHECK_EQUAL(kNoError, m_pCDtw->init(m_aiMatrixDimension[0], m_aiMatrixDimension[1]));
        CHECK_EQUAL(kFunctionInvalidArgsError, m_pCDtw->process(0));
        CHECK_EQUAL(0, m_pCDtw->getPathLength());
    }

    TEST_FIXTURE(DtwData, SingleCol)
    {
        int iPathLength         = 0;

        m_aiMatrixDimension[0]  = 3;
        m_aiMatrixDimension[1]  = 1;

        m_ppfData   = new float*[m_aiMatrixDimension[0]];
        for (int i = 0; i < m_aiMatrixDimension[0]; i++)
            m_ppfData[i]    = new float [m_aiMatrixDimension[1]];

        m_ppfData[0][0] = 0.1F; 
        m_ppfData[1][0] = 1; 
        m_ppfData[2][0] = 2; 

        m_pCDtw->init(m_aiMatrixDimension[0], m_aiMatrixDimension[1]);
        m_pCDtw->process (m_ppfData);

        iPathLength = m_pCDtw->getPathLength();
        CHECK_EQUAL(m_aiMatrixDimension[0], iPathLength);

        CHECK_EQUAL(3.1F, m_pCDtw->getPathCost());

        m_ppiPath = new int* [CDtw::kNumMatrixDimensions];
        for (int k = 0; k < CDtw::kNumMatrixDimensions; k++)
            m_ppiPath[k] = new int [iPathLength];

        m_pCDtw->getPath (m_ppiPath);

        for (int i = 0; i < iPathLength; i++)
        {
            CHECK_EQUAL(0, m_ppiPath[CDtw::kCol][i]);
            CHECK_EQUAL(i, m_ppiPath[CDtw::kRow][i]);
        }
    }

    TEST_FIXTURE(DtwData, SingleRow)
    {
        int iPathLength         = 0;

        m_aiMatrixDimension[0]  = 1;
        m_aiMatrixDimension[1]  = 3;

        m_ppfData   = new float*[m_aiMatrixDimension[0]];
        for (int i = 0; i < m_aiMatrixDimension[0]; i++)
            m_ppfData[i]    = new float [m_aiMatrixDimension[1]];

        m_ppfData[0][0] = 0.1F; m_ppfData[0][1] = 1; m_ppfData[0][2] = 2; 

        m_pCDtw->init(m_aiMatrixDimension[0], m_aiMatrixDimension[1]);
        m_pCDtw->process (m_ppfData);

        iPathLength = m_pCDtw->getPathLength();
        CHECK_EQUAL(m_aiMatrixDimension[1], iPathLength);

        CHECK_EQUAL(3.1F, m_pCDtw->getPathCost());

        m_ppiPath = new int* [CDtw::kNumMatrixDimensions];
        for (int k = 0; k < CDtw::kNumMatrixDimensions; k++)
            m_ppiPath[k] = new int [iPathLength];

        m_pCDtw->getPath (m_ppiPath);

        for (int i = 0; i < iPathLength; i++)
        {
            CHECK_EQUAL(i, m_ppiPath[CDtw::kCol][i]);
            CHECK_EQUAL(0, m_ppiPath[CDtw::kRow][i]);
        }
    }
}

#endif //WITH_TESTS