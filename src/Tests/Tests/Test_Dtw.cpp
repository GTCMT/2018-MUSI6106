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

        void prepareAcaData()
        {
            m_aiMatrixDimension[0] = 5;
            m_aiMatrixDimension[1] = 4;

            m_ppfData   = new float*[m_aiMatrixDimension[0]];
            
            for (int i = 0; i < m_aiMatrixDimension[0]; i++)
                m_ppfData[i]    = new float [m_aiMatrixDimension[1]];

            m_ppfData[0][0] = 0; m_ppfData[0][1] = 1; m_ppfData[0][2] = 2; m_ppfData[0][3] = 1; 
            m_ppfData[1][0] = 1; m_ppfData[1][1] = 2; m_ppfData[1][2] = 3; m_ppfData[1][3] = 0; 
            m_ppfData[2][0] = 1; m_ppfData[2][1] = 0; m_ppfData[2][2] = 1; m_ppfData[2][3] = 2; 
            m_ppfData[3][0] = 2; m_ppfData[3][1] = 1; m_ppfData[3][2] = 0; m_ppfData[3][3] = 3; 
            m_ppfData[4][0] = 0; m_ppfData[4][1] = 1; m_ppfData[4][2] = 2; m_ppfData[4][3] = 1; 
        }

        CDtw *m_pCDtw;

        float   **m_ppfData;
        int     **m_ppiPath;

        int     m_aiMatrixDimension[2];
    };

    TEST_FIXTURE(DtwData, Exercise)
    {
        prepareAcaData(); // fill m_ppfData with a 5x4 distance matrix
    }
}

#endif //WITH_TESTS