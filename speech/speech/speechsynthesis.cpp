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
	
	wstring a = L"<rate speed = '5'>This text should be spoken at rate five.< / rate><rate speed = '-5'>This text should be spoken at rate zero. </ rate><rate speed='-5'>Door is unlocked.</rate>";

	
	HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
	if (SUCCEEDED(hr))
	{
		hr = pVoice->Speak(a.c_str(),SPF_IS_XML, NULL);
		hr = pVoice->Speak(L"The number plate is  <spell>NICESKY</spell>", SPF_IS_XML, NULL);
		pVoice->Release();
		pVoice = NULL;
	}

	::CoUninitialize();
	main();
}
