//------------------------------------------------------------------------------
// File: IA_Colordiff_G-R.h
//
// Desc: DirectShow code - IA transformation filter difference between color channels
//
// Copyright (c) University of Applied Sciences Northwestern Switzerland.
// All rights reserved.
//------------------------------------------------------------------------------

#include <IABaseTransform.h>

// Filter name strings
#define g_wszFilterName     L"IA Colordiff R-B"

/**********************************************
 *
 *  Class declarations
 *
 **********************************************/

class CIAColordiffRmB : public CIABaseTransform
{

private:

    // Constructor is private because you have to use CreateInstance
    CIAColordiffRmB(LPUNKNOWN pUnk, HRESULT* phr);
    ~CIAColordiffRmB();

public:

	DECLARE_IUNKNOWN;

	//HRESULT Transform(IMediaSample *pIn, IMediaSample *pOut) override;
	HRESULT CheckInputType(const CMediaType *mtIn) override;
	HRESULT CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut) override;
	HRESULT GetMediaType(int iPosition, CMediaType *pMediaType) override;

	HRESULT Transform(IMediaSample *pMediaSample) override;

    static CUnknown * WINAPI CreateInstance(IUnknown *pUnk, HRESULT *phr);

};
