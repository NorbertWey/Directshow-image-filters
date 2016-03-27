//------------------------------------------------------------------------------
// File: IA_Colordiff_R-B.cpp
//
// Desc: DirectShow code - IA transformation filter difference between color channels
//
// Copyright (c) 2015 University of Applied Sciences Northwestern Switzerland.
// All rights reserved.
//------------------------------------------------------------------------------



#include <algorithm>
#include <amp.h>
#include <amp_math.h>
#include <concurrent_vector.h>
#include <streams.h>
#include <tchar.h>
#include <vector>

#include <IABaseStructs.h>
#include <IABaseTransformGuids.h>

#include "IA_Colordiff_R-B.h"
#include "IA_Colordiff_R-B_Guids.h"

using namespace std;


/**********************************************
 *
 *  Helper Structures & Functions
 *
 **********************************************/
/// <summary>
/// Helper structure to accessing the channels of an RGB32 byte stream.
/// </summary>
struct RGB32 {
	uint8_t b;
	uint8_t g;
	uint8_t r;
	uint8_t alpha;

	// see also: http://ch.mathworks.com/help/matlab/ref/rgb2gray.html
	uint8_t gray() { return 0.2989 * r + 0.5870 * g + 0.1140 * b; };
};
/// <summary>
/// Helper structure to accessing the channels of an RGB24/BGR24 byte stream.
/// see also: https://msdn.microsoft.com/en-us/library/windows/desktop/dd407253%28v=vs.85%29.aspx
/// </summary>
struct RGB24 {
	uint8_t byte0;	// RGB24: Blue
	uint8_t byte1;	// RGB24: Green
	uint8_t byte2;	// RGB24: Red
};


CIAColordiffRmB::CIAColordiffRmB(LPUNKNOWN pUnk, HRESULT *phr)
           : CIABaseTransform(g_wszFilterName, pUnk, phr, CLSID_IACOLORDIFF_RmB, 2, TRUE)
{
}


CIAColordiffRmB::~CIAColordiffRmB()
{
}


HRESULT CIAColordiffRmB::CheckInputType(const CMediaType *mtIn)
{

	CheckPointer(mtIn,FALSE);

    if (IsEqualGUID(*mtIn->Type(), MEDIATYPE_Video)) 
    {
		if (IsEqualGUID(*mtIn->Subtype(), MEDIASUBTYPE_RGB24))
        {
            VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) mtIn->Format();
            if (pvi->bmiHeader.biBitCount == 24)
				return NOERROR;
        }
    }

    return E_FAIL;
}


HRESULT CIAColordiffRmB::CheckTransform(const CMediaType *mtIn, const CMediaType *mtOut)
{
    CheckPointer(mtIn,E_POINTER);
    CheckPointer(mtOut,E_POINTER);

	VIDEOINFOHEADER *pviIn  = (VIDEOINFOHEADER *) mtIn->Format();
	VIDEOINFOHEADER *pviOut = (VIDEOINFOHEADER *) mtOut->Format();

	if (IsEqualGUID(*mtIn->Type(), MEDIATYPE_Video) && IsEqualGUID(*mtOut->Type(), MEDIATYPE_Video) &&
		IsEqualGUID(*mtIn->Subtype(), MEDIASUBTYPE_RGB24) && IsEqualGUID(*mtOut->Subtype(), MEDIASUBTYPE_RGB24) &&
		pviIn->bmiHeader.biBitCount == 24 && pviOut->bmiHeader.biBitCount == 24 )
    {
		return S_OK;
    }

    return E_FAIL;

}


HRESULT CIAColordiffRmB::GetMediaType(int iPosition, CMediaType *pMediaType) {
	CAutoLock m_Lock();

	CheckPointer(pMediaType, E_POINTER);

	ASSERT(m_pInput->IsConnected());
    if (iPosition < 0)
    {
        return E_INVALIDARG;
    }

	if (iPosition == 0)
	{
	
		// Read input pin
		HRESULT hr = m_pInput->ConnectionMediaType(pMediaType);
		if (FAILED(hr))
		{
			return hr;
		}

		VIDEOINFOHEADER *inPvi = reinterpret_cast<VIDEOINFOHEADER*>(pMediaType->pbFormat);
		ASSERT(inPvi);

		return S_OK;
	}
    return VFW_S_NO_MORE_ITEMS;
}


HRESULT CIAColordiffRmB::Transform(IMediaSample *pMediaSample)
{

	
	AM_MEDIA_TYPE* pType;
	pMediaSample->GetMediaType(&pType);
	VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *)pType->pbFormat;
	ASSERT(pvi);

	CheckPointer(pMediaSample, E_POINTER);
	PBYTE pbData;
	pMediaSample->GetPointer(&pbData);


	RGB24* pixel = (RGB24*)&pbData[0];

	for (int y = 0; y < pvi->bmiHeader.biHeight; y++) {
		for (int x = 0; x < pvi->bmiHeader.biWidth; x++) {

			int pred = pixel->byte2;
			pred = floor(pred / 2);
			int pblue = pixel->byte0;
			pblue = floor(pblue / 2);
			int pdiff = pred - pblue + 128;
			if (pdiff > 254){ pdiff = 254; }
			if (pdiff < 0){ pdiff = 0; }

			pixel->byte2 = pdiff;
			pixel->byte1 = pdiff;
			pixel->byte0 = pdiff;

			pixel++;
		}
	}
	
	return S_OK;
}


CUnknown * WINAPI CIAColordiffRmB::CreateInstance(IUnknown *pUnk, HRESULT *phr)
{
	ASSERT(phr);

	CIAColordiffRmB *pNewObject = new CIAColordiffRmB(pUnk, phr);
    if (pNewObject == NULL) {
        if (phr)
            *phr = E_OUTOFMEMORY;
    }

    return pNewObject;
}
