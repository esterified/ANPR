

#include<opencv2/core/core.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/ml/ml.hpp>
#include<iostream>
#include<sstream>
#include<string>
using namespace std;
using namespace cv;
////preprocessing parameters
const cv::Size GAUSSIAN_SMOOTH_FILTER_SIZE = cv::Size(5, 5);

const int ADAPTIVE_THRESH_BLOCK_SIZE = 51;
const int ADAPTIVE_THRESH_WEIGHT = 9;
////////constants for comparing adjacent chars
const double MIN_DIAG_SIZE_MULTIPLE_AWAY = 0.3;
const double MAX_DIAG_SIZE_MULTIPLE_AWAY = 5.0;

const double MAX_CHANGE_IN_AREA = 4.5;

const double MAX_CHANGE_IN_WIDTH = 1.7;
const double MAX_CHANGE_IN_HEIGHT = 0.2;

const double MAX_ANGLE_BETWEEN_CHARS = 12.0;
////////////////////
const int MIN_NUMBER_OF_MATCHING_CHARS = 3;


/////////////////////////////////////////////////

const double PLATE_WIDTH_PADDING_FACTOR = 1.3;
const double PLATE_HEIGHT_PADDING_FACTOR = 1.5;
///////////////////////////////////////////////////////////////////////
const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 40;


class platechar{
public:
	vector<cv::Point> contour;
	Rect boundrect;
	double rectarea;

	int c_x;
	int c_y;

	double dblDiagonalSize;
	double dblAspectRatio;
	static bool sortCharsLeftToRight(const platechar &pcLeft, const platechar & pcRight) {
		return(pcLeft.c_x < pcRight.c_x);
	}


	bool operator == (const platechar& otherChar) const {
		if (this->contour == otherChar.contour) return true;
		else return false;
	}


	bool operator != (const platechar& otherChar) const {
		if (this->contour != otherChar.contour) return true;
		else return false;
	}

	// function prototypes ////////////////////////////////////////////////////////////////////////
	platechar(vector<Point> _contour){
		contour = _contour;

		boundrect = boundingRect(contour);
		rectarea = boundrect.area();
		c_x = (boundrect.x + boundrect.x + boundrect.width) / 2;
		c_y = (boundrect.y + boundrect.y + boundrect.height) / 2;

		dblDiagonalSize = sqrt(pow(boundrect.width, 2) + pow(boundrect.height, 2));

		dblAspectRatio = (float)boundrect.width / (float)boundrect.height;


	}

};
Mat filterprocess(Mat &inputImage, vector<platechar> &charsInPlate);
std::vector<std::vector<platechar> > findVectorOfVectorsOfMatchingChars(const std::vector<platechar> &vectorOfPossibleChars);

std::vector<platechar> findVectorOfMatchingChars(const platechar &possibleChar, const std::vector<platechar> &allchars);

double distanceBetweenChars(const platechar &firstChar, const platechar &secondChar);

double angleBetweenChars(const platechar &firstChar, const platechar &secondChar);

std::vector<platechar> removeInnerOverlappingChars(std::vector<platechar> &vectorOfMatchingChars, std::vector<platechar> &vectorOfallMatchingChars);

Mat plates(cv::Mat &imgOriginal, Mat &imgdrawon, std::vector<platechar> &vectorOfMatchingChars, Mat &imageCrop);



Mat preprocess(cv::Mat &imgOriginal, cv::Mat &imgGrayscale);

cv::Mat extractValue(cv::Mat &imgOriginal);

cv::Mat maximizeContrast(cv::Mat &imgGrayscale);

void drawchars(Mat &pic, vector<platechar> &vectorOfChars, Scalar &color);

vector<platechar> possibleContours(Mat &image, vector<platechar> &allcontours);
vector<platechar> VectorOfallpossibleContours(Mat &image);
