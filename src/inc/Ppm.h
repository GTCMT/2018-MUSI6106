#if !defined(__Ppm_hdr__)
#define __Ppm_hdr__

#include "ErrorDef.h"

class CPpm
{
public:
    static Error_t createInstance (CPpm*& pCPpm);
    static Error_t destroyInstance (CPpm*& pCPpm);

    Error_t process (float **ppfInputBuffer, int iNumberOfFrames);

protected:
    CPpm ();
    virtual ~CPpm ();

private:

};

#endif // #if !defined(__Ppm_hdr__)