
#include "stdafx.h"

//---------------------------------------------------------------------------
//  DllMain
//---------------------------------------------------------------------------
BOOL APIENTRY DllMain( HINSTANCE hInstance, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		HRESULT hRes = ::CoInitialize(NULL);

		return SUCCEEDED(hRes);
	}

	if (ul_reason_for_call == DLL_PROCESS_DETACH)
	{
		::CoUninitialize();
	}

    return TRUE;
}

