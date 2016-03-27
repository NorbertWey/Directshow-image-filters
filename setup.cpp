//------------------------------------------------------------------------------
// File: setup.cpp
//
// Desc: DirectShow code
//
// Note: It is better to register no media types than to register a partial 
// media type (subtype == GUID_NULL) because that can slow down intelligent connect 
// for everyone else.

// For a specialized source filter like this, it is best to leave out the 
// AMOVIESETUP_FILTER altogether, so that the filter is not available for 
// intelligent connect. Instead, use the CLSID to create the filter or just 
// use 'new' in your application.

// More information:
// How to Register DirectShow Filters: https://msdn.microsoft.com/en-us/library/windows/desktop/dd389099%28v=vs.85%29.aspx
// Intelligent Connect https://msdn.microsoft.com/en-us/library/windows/desktop/dd390342(v=vs.85).aspx
// Declaring Filter Information https://msdn.microsoft.com/en-us/library/windows/desktop/dd388397(v=vs.85).aspx
// Guidelines for Registering Filters https://msdn.microsoft.com/en-us/library/windows/desktop/dd388793(v=vs.85).aspx
// Filter Categories https://msdn.microsoft.com/en-us/library/windows/desktop/dd375655(v=vs.85).aspx
//
// Copyright (c) 2015 University of Applied Sciences Northwestern Switzerland.
// All rights reserved.
//------------------------------------------------------------------------------

#include <streams.h>
#include <initguid.h>

#include <IABaseTransformGuids.h>

#include "IA_Colordiff_R-B_Guids.h"
#include "IA_Colordiff_R-B.h"


/**********************************************
 *
 *  Filter setup
 *
 **********************************************/

// Filter setup data
const AMOVIESETUP_MEDIATYPE sudPinTypes =
{
    &MEDIATYPE_Video,             // Major type
	&MEDIASUBTYPE_RGB24     // Minor type
};

const AMOVIESETUP_PIN sudpPins[] =
{
    { L"Input",             // Pins string name
      FALSE,                // Is it rendered
      FALSE,                // Is it an output
      FALSE,                // Are we allowed none
      FALSE,                // And allowed many
      &CLSID_NULL,          // Connects to filter
      NULL,                 // Connects to pin
      1,                    // Number of types
	  &sudPinTypes        // Pin information
    },
    { L"Output",            // Pins string name
      FALSE,                // Is it rendered
      TRUE,                 // Is it an output
      FALSE,                // Are we allowed none
      FALSE,                // And allowed many
      &CLSID_NULL,          // Connects to filter
      NULL,                 // Connects to pin
      1,                    // Number of types
	  &sudPinTypes       // Pin information
    }
};

const AMOVIESETUP_FILTER sudIACOLORDIFF_RmB =
{
	&CLSID_IACOLORDIFF_RmB, // Filter CLSID
    g_wszFilterName,        // String name
    MERIT_DO_NOT_USE,       // Filter merit
    2,                      // Number of pins
    sudpPins                // Pin information
};


// List of class IDs and creator functions for the class factory. This
// provides the link between the OLE entry point in the DLL and an object
// being created. The class factory will call the static CreateInstance.
// We provide a set of filters in this one DLL.
CFactoryTemplate g_Templates[1] = 
{
    { 
      g_wszFilterName,                   // Name
      &CLSID_IACOLORDIFF_RmB,            // CLSID
	  CIAColordiffRmB::CreateInstance,  // Method to create an instance of MyComponent
      NULL,                              // Initialization function
      NULL                               // Set-up information; if set, registers the filter to the "DirectShow Filters" category
    },
};

int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);


const REGFILTER2 rf2FilterReg = {
	1,				    // Version 1 (no pin mediums or pin category).
	MERIT_DO_NOT_USE,	// Merit.
	2,				    // Number of pins.
	sudpPins		    // Pointer to pin information.
};


/**********************************************
 *
 *  Exported entry points for registration and unregistration
 *
 **********************************************/

STDAPI DllRegisterServer()
{
	HRESULT hr;
	IFilterMapper2 *pFM2 = NULL;
	
	hr = AMovieDllRegisterServer2(TRUE);
	if (FAILED(hr))
		return hr;
	
	hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper2, (void **)&pFM2);
	if (FAILED(hr))
		return hr;
	
	hr = pFM2->CreateCategory(CLSID_ImageAnalysisCategory, MERIT_NORMAL, L"Image Analysis");
	if (FAILED(hr))
		return hr;

	hr = pFM2->RegisterFilter(
		CLSID_IACOLORDIFF_RmB,        // Filter CLSID.
		g_wszFilterName,              // Filter name.
		NULL,                         // Device moniker.
		&CLSID_ImageAnalysisCategory, // Video compressor category.
		g_wszFilterName,              // Instance data.
		&rf2FilterReg                 // Pointer to filter information.
	);

	pFM2->Release();
	return hr;
}

STDAPI DllUnregisterServer()
{
	HRESULT hr;
	IFilterMapper2 *pFM2 = NULL;

	hr = AMovieDllRegisterServer2(FALSE);
	if (FAILED(hr))
		return hr;
	
	hr = CoCreateInstance(CLSID_FilterMapper2, NULL, CLSCTX_INPROC_SERVER, IID_IFilterMapper2, (void **)&pFM2);
	if (FAILED(hr))
		return hr;

	hr = pFM2->UnregisterFilter(&CLSID_ImageAnalysisCategory, g_wszFilterName, CLSID_IACOLORDIFF_RmB);
	
	pFM2->Release();
	return hr;
}

//
// DllEntryPoint
//
extern "C" BOOL WINAPI DllEntryPoint(HINSTANCE, ULONG, LPVOID);

BOOL APIENTRY DllMain(HANDLE hModule, 
                      DWORD  dwReason, 
                      LPVOID lpReserved)
{
	return DllEntryPoint((HINSTANCE)(hModule), dwReason, lpReserved);
}
