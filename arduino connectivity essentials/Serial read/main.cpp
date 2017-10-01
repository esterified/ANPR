#include <stdio.h>
#include<iostream>
#include <tchar.h>
#include "SerialClass.h"	// Library described above
#include <string>
using namespace std;

// application reads from the specified serial port and reports the collected data
int _tmain(int argc, _TCHAR* argv[])
{

	printf("Welcome to the serial test app!\n\n");

	Serial* SP = new Serial("\\\\.\\COM16");    // adjust as needed

	if (SP->IsConnected())
		printf("We're connected");

	char incomingData[256] = "";			// don't forget to pre-allocate memory
	char write[100];
	//printf("%s\n",incomingData);
	int dataLength = 255;
	int readResult = 0;
	cin >> write;
	int f = 1;
	while (SP->IsConnected())
	{
		if (SP->WriteData(write, f))
			printf("success");


		break;
	}



	//while (SP->IsConnected())
	readResult = SP->ReadData(incomingData, dataLength);
	// printf("Bytes read: (0 means no data available) %i\n",readResult);
	incomingData[readResult] = 0;

	printf("%s\n", incomingData);


	Sleep(100000);

	return 0;

}
