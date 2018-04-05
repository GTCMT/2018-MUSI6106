
#if !defined(__Dtw_HEADER_INCLUDED__)
#define __Dtw_HEADER_INCLUDED__

#pragma once

#include "ErrorDef.h"

/*! \brief basic dynamic time warping (http://www.audiocontentanalysis.org/code/helper-functions/dtw/)
*/
class CDtw
{
public:
    enum MatrixDimension_t
    {
        kRow,
        kCol,

        kNumMatrixDimensions
    };

    CDtw(void);
    virtual ~CDtw(void);

    /*! initializes the class with the size of the distance matrix
    \param iNumRows
    \param iNumCols
    \return Error_t
    */
    Error_t init (int iNumRows, int iNumCols);
    
    /*! resets all internal class members
    \return Error_t
    */
    Error_t reset ();

    /*! computes cost and path w/o back-tracking
    \param ppfDistanceMatrix (dimensions [rows][columns])
    \return Error_t
    */
    Error_t process (float **ppfDistanceMatrix);
 
    /*! returns the length of the path
    \return int
    */
    int getPathLength ();
    
    /*! returns the overall cost
    \return float
    */
    float getPathCost () const;
    
    /*! returns the path \sa getPathLength
    \param ppiPathResult pointer to memory the result is written to (dimensions [2][length_of_path])
    \return Error_t
    */
    Error_t getPath (int **ppiPathResult) const;

private:
    enum Directions_t
    {
        kHoriz,
        kVert,
        kDiag,

        kNumDirections
    };
    enum InternalMemory_t
    {
        kRowCurr,
        kRowNext,

        kNumVectors
    };

    static inline Directions_t findMinimum (float fHorizCost, float fVertCost, float fDiagCost, float &fResultCost)
    {
        Directions_t eDirection = kDiag;
        fResultCost             = fDiagCost;

        if (fHorizCost < fDiagCost)
        {
            eDirection  = kHoriz;
            fResultCost = fHorizCost;
        }
        if (fVertCost < fHorizCost && fVertCost < fDiagCost)
        {
            eDirection  = kVert;
            fResultCost = fVertCost;
        }

        return eDirection;
    }


    bool m_bIsInitialized;
    bool m_bWasProcessed;

    float *m_apfCost[kNumVectors];
    float m_fOverallCost;
    unsigned char  **m_ppePathIdx;
    int   m_iLengthOfPath;
    int   m_aiMatrixDimensions[kNumMatrixDimensions];

    static const int aiDecrement[kNumDirections][kNumMatrixDimensions];
};


#endif
