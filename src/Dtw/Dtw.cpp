
#include "Vector.h"
#include "Util.h"

#include "Dtw.h"

CDtw::CDtw( void )
{

}

CDtw::~CDtw( void )
{

}

Error_t CDtw::init( int iNumRows, int iNumCols )
{
    return kNoError;
}

Error_t CDtw::reset()
{
    return kNoError;
}

Error_t CDtw::process(float **ppfDistanceMatrix)
{
    return kNoError;
}

int CDtw::getPathLength()
{    
    return 0;
}

float CDtw::getPathCost() const
{
    return 0.f;
}

Error_t CDtw::getPath( int **ppiPathResult ) const
{
    return kNoError;
}

