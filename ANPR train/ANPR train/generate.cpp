
#include "plate.h"


using namespace cv;
using namespace std;

///////////////////////////////////////////////////////////////////////////////////////////////////
void drawchars(Mat &pic, vector<platechar> &vectorOfChars, Scalar &color){
	vector<vector<cv::Point>> contra;
	vector<Vec4i> hierarchy;
	for (auto &Vect : vectorOfChars){
		contra.push_back(Vect.contour);

	}
	for (int i = 0; i < vectorOfChars.size(); i++){
		drawContours(pic, contra, i, color, CV_FILLED, 8, hierarchy, 0, Point());
		//rectangle(pic, vectorOfChars[i].boundrect, Scalar(0, 0, 255), 1, 8, 0);

	}

}
///////////////////////////////////////////////////////////////////////////////////////
vector<platechar> possibleContours(Mat &image, vector<platechar> &allcontours){
	vector<vector<Point> > contours;
	vector<platechar> test;
	Mat grayscale;
	Mat origin = preprocess(image, grayscale);
	Mat imagecopy = origin.clone();
	findContours(imagecopy, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);


	for (int i = 0; i< contours.size(); i++)
	{
		platechar posschar(contours[i]);
		test.push_back(posschar);
		if (posschar.rectarea < 2200){
			allcontours.push_back(posschar);
		}

	}
	/*Mat dst(1000, 750, CV_8UC3);
	dst = Scalar(255, 255, 255);
	drawchars(dst, test, Scalar(0, 0, 0));
	imshow("asdas", dst);*/
	return test;

}
//////////////////////////////////////////AVERAGE CONTOUR AREA/////////////////////////////////////////
double averageContourArea(vector<platechar> &vectorOfChars){
	double totalArea = 0;
	for (auto &chars : vectorOfChars){
		totalArea = totalArea + chars.rectarea;

	}
	double average = totalArea / (double)vectorOfChars.size();
	return average;
}

////////////////////////////////////////////////////////////////////////////
Mat filterprocess(Mat &pic, vector<platechar> &charsInPlate){
	vector<platechar> vectorofallmatch;
	vector<platechar> garbage, allchar;
	vector<platechar> test = possibleContours(pic, allchar);
	Mat crop, grayscale;
	Mat pic1 = pic.clone();
	Mat dst = Mat::zeros(pic.size(), CV_8UC3);
	Mat origin = preprocess(pic1, grayscale);
	vector<vector<platechar>> VectorOfVects = findVectorOfVectorsOfMatchingChars(test);
	Rect charRect;
	vector<vector<platechar>> VectorOfVectsgarbage;
	int i = 0, vectCount = 0;
	size_t size = 0;
	double average = 0;
	for (auto &vectorofmatch : VectorOfVects){
		/*for (int i = 0; i < vectorofmatch.size(); i++){
		contra = vectorofmatch[i].contour;
		}*/
		/*for (auto &convec : vectorofmatch){
		contra.push_back(convec.contour);
		}
		for (int i = 0; i < contra.size(); i++){
		drawContours(dst, contra, i, Scalar(255,255,255), CV_FILLED, 8, hierarchy, 0, Point());
		}
		*/

		garbage = removeInnerOverlappingChars(vectorofmatch, allchar);
		VectorOfVectsgarbage.push_back(garbage);
		if (vectorofmatch.size() > size && averageContourArea(vectorofmatch)>average){
			size = vectorofmatch.size();

			vectCount = i;

		}
		i++;

	}
	charsInPlate = VectorOfVects[vectCount];
	Mat rotationMatrix = plates(origin, pic, VectorOfVects[vectCount], crop);
	warpAffine(dst, dst, rotationMatrix, dst.size());///rotate the image before cropping;
	drawchars(dst, VectorOfVects[vectCount], Scalar(255, 255, 255));
	drawchars(dst, VectorOfVectsgarbage[vectCount], Scalar(0, 0, 0));
	//garbage = removeInnerOverlappingChars(vectorofmatch, allchar);
	cout << size << "characters in the plate" << endl;
	return(dst);
}
///////////////////////////////////////////////////////////////////////
std::vector<platechar> removeInnerOverlappingChars(std::vector<platechar> &vectorOfMatchingChars, std::vector<platechar> &vectorOfallMatchingChars) {
	std::vector<platechar> vectorOfInnerChar;
	std::vector<platechar> vectorOfMatchingChars1(vectorOfMatchingChars);


	for (auto &otherChar : vectorOfallMatchingChars){

		for (auto &currentChar : vectorOfMatchingChars1){

			if (currentChar != otherChar){

				if (otherChar.boundrect.x > currentChar.boundrect.x && (currentChar.boundrect.x + currentChar.boundrect.width) > (otherChar.boundrect.x + otherChar.boundrect.width)
					&& otherChar.boundrect.y > currentChar.boundrect.y && (currentChar.boundrect.y + currentChar.boundrect.height) > (otherChar.boundrect.y + otherChar.boundrect.height))
				{
					vectorOfInnerChar.push_back(otherChar);
					//vectorOfMatchingChars.erase(otherChar);//////////need to erase
					vector<platechar>::iterator currentCharIterator = std::find(vectorOfMatchingChars.begin(), vectorOfMatchingChars.end(), otherChar);
					if (currentCharIterator != vectorOfMatchingChars.end()){
						vectorOfMatchingChars.erase(currentCharIterator);
					}


				}

			}

		}
	}


	return(vectorOfInnerChar);
}

std::vector<std::vector<platechar> > findVectorOfVectorsOfMatchingChars(const std::vector<platechar> &vectorOfPossibleChars) {
	// with this function, we start off with all the possible chars in one big vector
	// the purpose of this function is to re-arrange the one big vector of chars into a vector of vectors of matching chars,
	// note that chars that are not found to be in a group of matches do not need to be considered further
	std::vector<std::vector<platechar> > vectorOfVectorsOfMatchingChars;             // this will be the return value



	for (auto &possibleChar : vectorOfPossibleChars) {                  // for each possible char in the one big vector of chars

		// find all chars in the big vector that match the current char

		std::vector<platechar> vectorOfMatchingChars = findVectorOfMatchingChars(possibleChar, vectorOfPossibleChars);

		vectorOfMatchingChars.push_back(possibleChar);          // also add the current char to current possible vector of matching chars

		// if current possible vector of matching chars is not long enough to constitute a possible plate
		if (vectorOfMatchingChars.size() < MIN_NUMBER_OF_MATCHING_CHARS) {
			continue;                       // jump back to the top of the for loop and try again with next char, note that it's not necessary
			// to save the vector in any way since it did not have enough chars to be a possible plate
		}
		// if we get here, the current vector passed test as a "group" or "cluster" of matching chars
		vectorOfVectorsOfMatchingChars.push_back(vectorOfMatchingChars);            // so add to our vector of vectors of matching chars

		std::vector<platechar> vectorOfPossibleCharsWithCurrentMatchesRemoved;

		for (auto &possChar : vectorOfPossibleChars) {
			if (std::find(vectorOfMatchingChars.begin(), vectorOfMatchingChars.end(), possChar) == vectorOfMatchingChars.end()) {
				vectorOfPossibleCharsWithCurrentMatchesRemoved.push_back(possChar);
			}
		}
		// declare new vector of vectors of chars to get result from recursive call
		std::vector<std::vector<platechar> > recursiveVectorOfVectorsOfMatchingChars;

		// recursive call
		recursiveVectorOfVectorsOfMatchingChars = findVectorOfVectorsOfMatchingChars(vectorOfPossibleCharsWithCurrentMatchesRemoved);	// recursive call !!

		for (auto &recursiveVectorOfMatchingChars : recursiveVectorOfVectorsOfMatchingChars) {      // for each vector of matching chars found by recursive call
			vectorOfVectorsOfMatchingChars.push_back(recursiveVectorOfMatchingChars);               // add to our original vector of vectors of matching chars



		}

		break;		// exit for loop
	}

	return(vectorOfVectorsOfMatchingChars);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::vector<platechar> findVectorOfMatchingChars(const platechar &possibleChar, const std::vector<platechar> &allchars) {

	std::vector<platechar> vectorOfMatchingChars;                // this will be the return value

	for (auto &possibleMatchingChar : allchars) {              // for each char in big vector
		// if the char we attempting to find matches for is the exact same char as the char in the big vector we are currently checking
		if (possibleMatchingChar == possibleChar) {
			// then we should not include it in the vector of matches b/c that would end up double including the current char
			continue;           // so do not add to vector of matches and jump back to top of for loop
		}
		// compute stuff to see if chars are a match
		double dblDistanceBetweenChars = distanceBetweenChars(possibleChar, possibleMatchingChar);
		double dblAngleBetweenChars = angleBetweenChars(possibleChar, possibleMatchingChar);
		double dblChangeInArea = (double)abs(possibleMatchingChar.rectarea - possibleChar.rectarea) / (double)possibleChar.rectarea;
		double dblChangeInWidth = (double)abs(possibleMatchingChar.boundrect.width - possibleChar.boundrect.width) / (double)possibleChar.boundrect.width;
		double dblChangeInHeight = (double)abs(possibleMatchingChar.boundrect.height - possibleChar.boundrect.height) / (double)possibleChar.boundrect.height;

		// check if chars match
		if (dblDistanceBetweenChars < (possibleChar.dblDiagonalSize * MAX_DIAG_SIZE_MULTIPLE_AWAY) &&
			dblAngleBetweenChars < MAX_ANGLE_BETWEEN_CHARS &&
			dblChangeInArea < MAX_CHANGE_IN_AREA &&
			dblChangeInWidth < MAX_CHANGE_IN_WIDTH &&
			dblChangeInHeight < MAX_CHANGE_IN_HEIGHT) {
			vectorOfMatchingChars.push_back(possibleMatchingChar);      // if the chars are a match, add the current char to vector of matching chars
		}
	}

	return(vectorOfMatchingChars);          // return result
}
///////////////////////////////////////////////////////////////////////////////////////////////////
// use Pythagorean theorem to calculate distance between two chars
double distanceBetweenChars(const platechar &firstChar, const platechar &secondChar) {
	int intX = abs(firstChar.c_x - secondChar.c_x);
	int intY = abs(firstChar.c_y - secondChar.c_y);

	return(sqrt(pow(intX, 2) + pow(intY, 2)));
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// use basic trigonometry(SOH CAH TOA) to calculate angle between chars
double angleBetweenChars(const platechar &firstChar, const platechar &secondChar) {
	double dblAdj = abs(firstChar.c_x - secondChar.c_x);
	double dblOpp = abs(firstChar.c_y - secondChar.c_y);

	double dblAngleInRad = atan(dblOpp / dblAdj);

	double dblAngleInDeg = dblAngleInRad * (180.0 / CV_PI);

	return(dblAngleInDeg);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////
Mat plates(cv::Mat &imgOriginal, Mat &imgdrawon, std::vector<platechar> &vectorOfMatchingChars, Mat &imageCrop) {
	// this will be the return value

	// sort chars from left to right based on x position
	std::sort(vectorOfMatchingChars.begin(), vectorOfMatchingChars.end(), platechar::sortCharsLeftToRight);

	// calculate the center point of the plate
	double dblPlateCenterX = (double)(vectorOfMatchingChars[0].c_x + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].c_x) / 2.0;
	double dblPlateCenterY = (double)(vectorOfMatchingChars[0].c_y + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].c_y) / 2.0;
	cv::Point2d p2dPlateCenter(dblPlateCenterX, dblPlateCenterY);

	// calculate plate width and height
	int intPlateWidth = (int)((vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].boundrect.x + vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].boundrect.width - vectorOfMatchingChars[0].boundrect.x) * PLATE_WIDTH_PADDING_FACTOR);

	double intTotalOfCharHeights = 0;

	for (auto &matchingChar : vectorOfMatchingChars) {
		intTotalOfCharHeights = intTotalOfCharHeights + matchingChar.boundrect.height;
	}

	double dblAverageCharHeight = (double)intTotalOfCharHeights / vectorOfMatchingChars.size();

	int intPlateHeight = (int)(dblAverageCharHeight * PLATE_HEIGHT_PADDING_FACTOR);

	// calculate correction angle of plate region
	double dblOpposite = vectorOfMatchingChars[vectorOfMatchingChars.size() - 1].c_y - vectorOfMatchingChars[0].c_y;
	double dblHypotenuse = distanceBetweenChars(vectorOfMatchingChars[0], vectorOfMatchingChars[vectorOfMatchingChars.size() - 1]);
	double dblCorrectionAngleInRad = asin(dblOpposite / dblHypotenuse);//sine inverse
	double dblCorrectionAngleInDeg = dblCorrectionAngleInRad * (180.0 / CV_PI);

	// assign rotated rect member variable of possible plate
	cv::RotatedRect rrLocationOfPlateInScene = cv::RotatedRect(p2dPlateCenter, cv::Size2f((float)intPlateWidth, (float)intPlateHeight), (float)dblCorrectionAngleInDeg);

	cv::Mat rotationMatrix;             // final steps are to perform the actual rotation
	cv::Mat imgRotated;
	cv::Mat imgCropped;
	cv::Point2f p2fRectPoints[4];
	rotationMatrix = cv::getRotationMatrix2D(p2dPlateCenter, dblCorrectionAngleInDeg, 1.0);         // get the rotation matrix for our calculated correction angle

	cv::warpAffine(imgOriginal, imgRotated, rotationMatrix, imgOriginal.size());            // rotate the entire image

	//crop out the actual plate portion of the rotated image
	cv::getRectSubPix(imgRotated, rrLocationOfPlateInScene.size, rrLocationOfPlateInScene.center, imgCropped);
	imshow("rotated", imgOriginal);
	// copy the cropped plate image into the applicable member variable of the possible plate
	char Name[50];
	sprintf(Name, "win %d", rand());

	imshow(Name, imgCropped);


	rrLocationOfPlateInScene.points(p2fRectPoints);
	imageCrop = imgCropped.clone();

	for (int j = 0; j < 4; j++) {
		cv::line(imgdrawon, p2fRectPoints[j], p2fRectPoints[(j + 1) % 4], Scalar(0, 120, 255), 1);
	}
	return(rotationMatrix);

}

//////////////////////////////////////////////////PREPROCESSING SECTION//////////////////////////////
Mat preprocess(cv::Mat &imgOriginal, cv::Mat &imgGrayscale) {
	imgGrayscale = extractValue(imgOriginal);                           // extract value channel only from original image to get imgGrayscale
	Mat imgThresh;
	cv::Mat imgMaxContrastGrayscale = maximizeContrast(imgGrayscale);       // maximize contrast with top hat and black hat
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(1, 1));
	cv::Mat imgBlurred;

	cv::GaussianBlur(imgMaxContrastGrayscale, imgBlurred, GAUSSIAN_SMOOTH_FILTER_SIZE, 0);          // gaussian blur
	erode(imgBlurred, imgBlurred, dilateElement);

	//Canny(imgBlurred, imgThresh, 50, 200, 3,true);
	// call adaptive threshold to get imgThresh
	cv::adaptiveThreshold(imgBlurred, imgThresh, 255.0, CV_ADAPTIVE_THRESH_GAUSSIAN_C, CV_THRESH_BINARY_INV, ADAPTIVE_THRESH_BLOCK_SIZE, ADAPTIVE_THRESH_WEIGHT);

	return (imgThresh);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
cv::Mat extractValue(cv::Mat &imgOriginal) {
	cv::Mat imgHSV;
	std::vector<cv::Mat> vectorOfHSVImages;
	cv::Mat imgValue;

	cv::cvtColor(imgOriginal, imgHSV, CV_BGR2HSV);

	cv::split(imgHSV, vectorOfHSVImages);

	imgValue = vectorOfHSVImages[2];

	return(imgValue);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

cv::Mat maximizeContrast(cv::Mat &imgGrayscale) {
	cv::Mat imgTopHat;
	cv::Mat imgBlackHat;
	cv::Mat imgGrayscalePlusTopHat;
	cv::Mat imgGrayscalePlusTopHatMinusBlackHat;

	cv::Mat structuringElement = cv::getStructuringElement(CV_SHAPE_RECT, cv::Size(3, 3));

	cv::morphologyEx(imgGrayscale, imgTopHat, CV_MOP_TOPHAT, structuringElement);
	cv::morphologyEx(imgGrayscale, imgBlackHat, CV_MOP_BLACKHAT, structuringElement);

	imgGrayscalePlusTopHat = imgGrayscale + imgTopHat;
	imgGrayscalePlusTopHatMinusBlackHat = imgGrayscalePlusTopHat - imgBlackHat;

	return(imgGrayscalePlusTopHatMinusBlackHat);
}

/*std::string recognizeCharsInPlate(cv::Mat &imgThresh, std::vector<platechar> &vectorOfMatchingChars) {
std::string strChars;               // this will be the return value, the chars in the lic plate

cv::Mat imgThreshColor;

// sort chars from left to right
std::sort(vectorOfMatchingChars.begin(), vectorOfMatchingChars.end(), platechar::sortCharsLeftToRight);

cv::cvtColor(imgThresh, imgThreshColor, CV_GRAY2BGR);       // make color version of threshold image so we can draw contours in color on it

for (auto &currentChar : vectorOfMatchingChars) {           // for each char in plate
cv::rectangle(imgThreshColor, currentChar.boundrect,, 2);       // draw green box around the char

cv::Mat imgROItoBeCloned = imgThresh(currentChar.boundrect);                 // get ROI image of bounding rect

cv::Mat imgROI = imgROItoBeCloned.clone();      // clone ROI image so we don't change original when we resize

cv::Mat imgROIResized;
// resize image, this is necessary for char recognition
cv::resize(imgROI, imgROIResized, cv::Size(RESIZED_CHAR_IMAGE_WIDTH, RESIZED_CHAR_IMAGE_HEIGHT));

cv::Mat matROIFloat;

imgROIResized.convertTo(matROIFloat, CV_32FC1);         // convert Mat to float, necessary for call to findNearest

cv::Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);       // flatten Matrix into one row

cv::Mat matCurrentChar(0, 0, CV_32F);                   // declare Mat to read current char into, this is necessary b/c findNearest requires a Mat

kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);     // finally we can call find_nearest !!!

float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);       // convert current char from Mat to float

strChars = strChars + char(int(fltCurrentChar));        // append current char to full string
}



return(strChars);               // return result
}
*/