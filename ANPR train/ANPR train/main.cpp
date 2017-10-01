#include "plate.h"

int main(){
	std::vector<int> intValidChars = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T',
		'U', 'V', 'W', 'X', 'Y', 'Z' };

	cv::Mat imgTrainingNumbers, matTrainingImagesAsFlattenedFloats, matClassificationInts;
	vector<platechar> vectorOfChar,allcontours,garbage;
	Mat grayscale,pic = imread("training_chars1.png", CV_LOAD_IMAGE_COLOR);
	//cv::resize(pic, pic, cv::Size(1000, 600));
	Mat dst(pic.size(), CV_8UC3);
	//Mat small(Size(20,30), CV_8UC1);
	dst = Scalar(0,0,0);
	//pic = preprocess(pic, grayscale); 
	vectorOfChar = possibleContours(pic, allcontours);
	drawchars(dst, vectorOfChar, Scalar(255,255,255));
	garbage = removeInnerOverlappingChars(vectorOfChar, allcontours);
	drawchars(dst, garbage, Scalar(0, 0, 0));
	//imshow("pic", pic);
	imshow("black", dst);
	//imshow("small", small);
	
	
	for (auto &chars : vectorOfChar) {                           // for each contour
		// if contour is big enough to consider
		//cv::Rect boundingRect = cv::boundingRect(ptContours[i]);                // get the bounding rect

		cv::rectangle(pic, chars.boundrect, cv::Scalar(0, 0, 255), 2);      // draw red rectangle around each contour as we ask user for input

		cv::Mat matROI = dst(chars.boundrect);           // get ROI image of bounding rect
		cvtColor(matROI, matROI, CV_BGR2GRAY);
		cout << matROI.channels();
		cv::Mat matROIResized;
		cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));     // resize image, this will be more consistent for recognition and storage

		cv::imshow("matROI", matROI);                               // show ROI image for reference
		cv::imshow("matROIResized", matROIResized);                 // show resized ROI image for reference
		cv::imshow("imgTrainingNumbers", pic);       // show training numbers image, this will now have red rectangles drawn on it

		int intChar = cv::waitKey(0);           // get key press

		if (intChar == 27) {        // if esc key was pressed
			return(0);              // exit program
		}
		else if (std::find(intValidChars.begin(), intValidChars.end(), intChar) != intValidChars.end()) {     // else if the char is in the list of chars we are looking for . . .

			matClassificationInts.push_back(intChar);       // append classification char to integer list of chars

			cv::Mat matImageFloat;                          // now add the training image (some conversion is necessary first) . . .
			matROIResized.convertTo(matImageFloat, CV_32FC1);       // convert Mat to float

			cv::Mat matImageFlattenedFloat = matImageFloat.reshape(1, 1);       // flatten
			//cout << "size " << matImageFlattenedFloat.size() << endl;
			matTrainingImagesAsFlattenedFloats.push_back(matImageFlattenedFloat);       // add to Mat as though it was a vector, this is necessary due to the
			// data types that KNearest.train accepts
		}   // end if

	}   // end for

	std::cout << "training complete\n\n";

	// save classifications to file ///////////////////////////////////////////////////////

	cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::WRITE);           // open the classifications file

	if (fsClassifications.isOpened() == false) {                                                        // if the file was not opened successfully
		std::cout << "error, unable to open training classifications file, exiting program\n\n";        // show error message
		return(0);                                                                                      // and exit program
	}

	fsClassifications << "classifications" << matClassificationInts;        // write classifications into classifications section of classifications file
	fsClassifications.release();                                            // close the classifications file

	// save training images to file ///////////////////////////////////////////////////////

	cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::WRITE);         // open the training images file

	if (fsTrainingImages.isOpened() == false) {                                                 // if the file was not opened successfully
		std::cout << "error, unable to open training images file, exiting program\n\n";         // show error message
		return(0);                                                                              // and exit program
	}

	fsTrainingImages << "images" << matTrainingImagesAsFlattenedFloats;         // write training images into images section of images file
	fsTrainingImages.release();

	return(0);

}