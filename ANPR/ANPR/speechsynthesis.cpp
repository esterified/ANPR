#include "stdafx.h"
#include <sapi.h>
#include <sstream>
#include <iostream>
using namespace std;

int main()
{
	ISpVoice * pVoice = NULL;

	if (FAILED(::CoInitialize(NULL)))
		return FALSE;
	wstringstream wss;
	string a;
	
	wstring text=L"M C F L R N";//wcin for read and wcout for print
	
	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
	if (SUCCEEDED(hr))
	{
		
		hr = pVoice->Speak(L"<rate speed='-3'><emph>hello</emph>!<silence msec='500'/> the number plate is </rate>", SPF_IS_XML, NULL);
		hr = pVoice->Speak(text.c_str(), SPF_IS_XML, NULL);
		
		pVoice->Release();
		pVoice = NULL;
	}

	::CoUninitialize();
	return TRUE;
}
