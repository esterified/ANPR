#include "platechar.h"
#include "SerialClass.h"
#include "stdafx.h"
#include <sapi.h>



bool arduino = false;
char sendi[100];
Serial* SP = new Serial("\\\\.\\COM16");    // adjust as needed"\\\\.\\COM16"
void arduinosend(){

				// don't forget to pre-allocate memory
	char write[100];
	//printf("%s\n",incomingData);
	write[0] = '1';
	write[1] = '\0';
	
	int f = 4;

	while (SP->IsConnected())
	{
		if (arduino){
			SP->WriteData(write, f);
			printf("success\n");
			
		}

		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////

int main(){
	int gom,dom;
	ISpVoice * pVoice = NULL;/////////////speech conversion initialization
	string database1,recstring;
	vector<string> vectdata;
	cv::VideoCapture cap(1);
	std::string strFinalString;
	cv::Mat frame;
	//namedWindow("fayed", 1);
	
	printf("Welcome to the ANPR system!\n\n");

	if (SP->IsConnected())
	{
		printf("We're connected");
	}
	char incomingData[256] = "";
	int dataLength = 255;
	int readResult = 0;
	while (1){

		readResult = SP->ReadData(incomingData, dataLength);
		// printf("Bytes read: (0 means no data available) %i\n",readResult);
		//incomingData[readResult] = 0;

		if (incomingData[0] == '5'){
			break;
		}
		printf("no data found %s\n", incomingData);
		Sleep(1000);
		
	}

	
	int counti = 0;
		
	while (1){
		strFinalString = "";
		cv::Mat matClassificationInts,pic;      // we will read the classification numbers into this variable as though it is a vector
		
		cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::READ);        // open the classifications file

		if (fsClassifications.isOpened() == false) {                                                    // if the file was not opened successfully
			std::cout << "error, unable to open training classifications file, exiting program\n\n";    // show error message
			return(0);                                                                                  // and exit program
		}

		fsClassifications["classifications"] >> matClassificationInts;      // read classifications section into Mat classifications variable
		fsClassifications.release();                                        // close the classifications file

		// read in training images ////////////////////////////////////////////////////////////
		cv::Mat matTrainingImagesAsFlattenedFloats; // we will read multiple images into this single image variable as though it is a vector
		cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::READ);          // open the training images file
		if (fsTrainingImages.isOpened() == false) {                                                 // if the file was not opened successfully
			std::cout << "error, unable to open training images file, exiting program\n\n";         // show error message
			return(0);                                                                              // and exit program
		}
		fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;           // read images section into Mat training images variable
		fsTrainingImages.release();
		cv::Ptr<cv::ml::KNearest>  kNearest(cv::ml::KNearest::create());            // instantiate the KNN object

		// finally we get to the call to train, note that both parameters have to be of type Mat (a single Mat)
		// even though in reality they are multiple images / numbers
		kNearest->train(matTrainingImagesAsFlattenedFloats, cv::ml::ROW_SAMPLE, matClassificationInts);////main row for the tutorial
		vector<platechar> vectorOfChars;
		cv::Mat croppedChar;
		int c = 0;
		//char name[50];
		/*cv::Mat pic = cv::imread("1.png", CV_LOAD_IMAGE_COLOR);
		if (pic.empty()){
			std::cout << "image could not be read";
			return(0);
		}*/
		//resize(pic, pic, Size(900,700));
		cap >> pic;


		cv::Mat picCopy = pic.clone();
		cv::Mat output = filterprocess(picCopy, vectorOfChars);
		//cv::imshow("image", picCopy);
		
		std::sort(vectorOfChars.begin(), vectorOfChars.end(), platechar::sortCharsLeftToRight);
		for (auto &chars : vectorOfChars){
			croppedChar = output(chars.boundrect);
			cvtColor(croppedChar, croppedChar, CV_BGR2GRAY);
			cv::Mat matROIResized;
			cv::resize(croppedChar, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));     // resize image, this will be more consistent for recognition and storage
			if (c == 1)
			{
			   imshow("ROI resized", matROIResized);
			}
			cv::Mat matROIFloat;
			matROIResized.convertTo(matROIFloat, CV_32FC1);             // convert Mat to float, necessary for call to find_nearest

			cv::Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);

			cv::Mat matCurrentChar(0, 0, CV_32F);

			kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);     // finally we can call find_nearest !!!

			float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);

			strFinalString = strFinalString + char(int(fltCurrentChar));
			c++;
		}
		cv::imshow("end", output);
		std::cout << "\n\n" << "numbers read = " << strFinalString << "\n\n" << "length is" << strFinalString.length() << endl;
		
		
		///////////////////////////database
		cv::FileStorage datar("database.xml", cv::FileStorage::READ);
		cv::FileNode n = datar["plate_number"];                         // Read string sequence - Get node
		if (n.type() != cv::FileNode::SEQ)
		{
			cerr << "strings is not a sequence! FAIL" << endl;/////////////i case if there is only one value in database
			return 1;
		}
		cv::FileNodeIterator it = n.begin(), it_end = n.end(); // Go through the node
		cv::FileStorage data("database.xml", cv::FileStorage::READ);
		cv::FileNode an = data["Name"];                         // Read string sequence - Get node
		if (an.type() != cv::FileNode::SEQ)
		{
			cerr << "strings is not a sequence! FAIL" << endl;/////////////i case if there is only one value in database
			return 1;
		}
		cv::FileNodeIterator it1 = an.begin(), it_end1 = an.end(); // Go through the node
		dom = 0;
		for (; it != it_end; ++it)
		{

			database1 = (string)*it;
			if ( strFinalString == database1){
				std::cout << "DOOR UNLOCKED " << endl;
				gom = dom;
				arduino = true;
			}
			dom++;
			std::cout << (string)*it << endl;
		}
		datar.release();
		dom = 0;
		for (; it1 != it_end1; ++it1)
		{

			database1 = (string)*it1;
			if (gom==dom){
				std::cout << "NAME FOUND" << endl;
				recstring = database1;
			
			}
			dom++;
			//std::cout << (string)*it1 << endl;
		}
		data.release();
		dom =0;
		
	
		/*cv::FileStorage data("database.xml", cv::FileStorage::WRITE);           // open the classifications file

if (data.isOpened() == false) {                                                        // if the file was not opened successfully
std::cout << "error, opening data";        // show error message
return(0);                                                                                      // and exit program
}
vectdata.push_back(strFinalString);
vectdata.push_back(strFinalString);
data << "plate_number" << vectdata;         // write training images into images section of images file

data.release();
*/
		/////////////////////
		
		
		arduinosend();
		
		strcpy(sendi, strFinalString.c_str());
		while (SP->IsConnected())
		{
			if (arduino){

				SP->WriteData(sendi, 100);
				printf("plate sent\n");
			}
			break;

		}
		
		
		
		if (FAILED(::CoInitialize(NULL)))
			return FALSE;
		wstring jam=L"<rate speed='-3'>wrong plate.</rate>";

		wstringstream wss;
		string str,temp,a1;
		temp= " <silence msec = '400'/>";
		string a = "<rate speed = '-3'><emph>hello<silence msec = '50' /> ";
		a = a + recstring;
		a = a + "< /emph><silence msec = '300' / >The number plate is :</rate><silence msec = '600' / ><rate speed = '-3'>";
		for (int i = 0; i < strFinalString.size(); i++){
			str.push_back(strFinalString[i]);
			str = str + temp;
		}
		
		a = a + str;
		a = a + "</rate>";
		a1 = "<rate speed='-4'><speech>" + strFinalString + "</speech></rate>";
		//std::cout << a<<endl;
		std::wstring widestr = std::wstring(a.begin(), a.end());
		std::wstring widestr1 = std::wstring(a1.begin(), a1.end());
		HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
		if (SUCCEEDED(hr))
		{
			
			if (!arduino){
				hr = pVoice->Speak(jam.c_str(), SPF_IS_XML, NULL);

			}
			
			
			//////////////////arduino send numberplate
			
			
			//hr = pVoice->Speak(L"<rate speed='-3'>", SPF_IS_XML, NULL);
			hr = pVoice->Speak(widestr.c_str(), SPF_IS_XML, NULL);
			//hr = pVoice->Speak(widestr1.c_str(), SPF_IS_XML, NULL);
			
			pVoice->Release();
			pVoice = NULL;
		}
		
		
		
        ::CoUninitialize();
		counti++;
		////////////////////////////////////////
		

		if (counti == 3 || arduino==true){
			break;
		}
		
		cout << counti << endl;
		cv::waitKey(2000);/////////////////////////////////////end of code


	}

	cout << "program exited";
	
	
	cv::waitKey(0);
		return TRUE;


}