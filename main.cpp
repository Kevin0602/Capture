//Opencv includes
#include <opencv2/core/utility.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/core/cuda.hpp"
#include "opencv2/cudaimgproc.hpp"


//Stander includes
#include <math.h>
#include <iostream>
#include <ctype.h>
#include <thread>
#include <queue>
#include <vector>

// ZED includes
#include <sl/Camera.hpp>

//Communication includes
//#include "can_send.h"
//#include "linux_sever.h"

using namespace sl;
using namespace cv;
using namespace std;



//Main data struct
typedef struct {

	cv::cuda::GpuMat L;
	cv::cuda::GpuMat R;
	bool finish;
	cv::Mat position;
}IMAGE_PAIR;

vector<IMAGE_PAIR> image_pair_vector;

//Varibales for mouse selected and trackbar
bool backprojMode = false;
bool selectObject = false;
int trackObject = 0;
bool showHist = true;
Point origin;
Rect selection;
int vmin = 10, vmax = 256, smin = 30;

//Image on cpu for showing and drawing result on
cv::Mat image_ocv_L, image_ocv_R;

//Histogram of the object selected
cv::Mat hist;

//Mutex of all threads
Mutex mtx;

// Create a ZED camera object
Camera zed;

// draw a box around object to track. This triggers CAMShift to start tracking
static void onMouse(int event, int x, int y, int, void*)
{
	if (selectObject)
	{
		selection.x = MIN(x, origin.x);
		selection.y = MIN(y, origin.y);
		selection.width = std::abs(x - origin.x);
		selection.height = std::abs(y - origin.y);
		selection &= Rect(0, 0, image_ocv_L.cols, image_ocv_L.rows);
	}
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
		origin = Point(x, y);
		selection = Rect(x, y, 0, 0);
		selectObject = true;
		break;
	case EVENT_LBUTTONUP:
		selectObject = false;
		if (selection.width > 0 && selection.height > 0)
			trackObject = -1;   // Set up CAMShift properties in main() loop
		break;
	}
}

//Covert sl mat to cv mat gpu
sl::Mat cvGpuMat2slGpuMat(cv::cuda::GpuMat& input)
{
	// Mapping between MAT_TYPE and CV_TYPE
	sl::MAT_TYPE type;
	switch (input.type()) {

	case  CV_8UC3: type = MAT_TYPE_8U_C3; break;
	case  CV_8UC4: type = MAT_TYPE_8U_C4; break;
	default: break;
	}

	// Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
	// cv::Mat and sl::Mat will share a single memory structure
	return sl::Mat(size_t(input.rows), size_t(input.cols), type, (sl::uchar1*)input.data, input.step, MEM_GPU);
}


cv::cuda::GpuMat slGpuMat2cvGpuMat(sl::Mat& input)
{
	// Mapping between MAT_TYPE and CV_TYPE
	int cv_type = -1;
	switch (input.getDataType()) {
	case MAT_TYPE_32F_C1: cv_type = CV_32FC1; break;
	case MAT_TYPE_32F_C2: cv_type = CV_32FC2; break;
	case MAT_TYPE_32F_C3: cv_type = CV_32FC3; break;
	case MAT_TYPE_32F_C4: cv_type = CV_32FC4; break;
	case MAT_TYPE_8U_C1: cv_type = CV_8UC1; break;
	case MAT_TYPE_8U_C2: cv_type = CV_8UC2; break;
	case MAT_TYPE_8U_C3: cv_type = CV_8UC3; break;
	case MAT_TYPE_8U_C4: cv_type = CV_8UC4; break;
	default: break;
	}

	// Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
	// cv::Mat and sl::Mat will share a single memory structure
	return cv::cuda::GpuMat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM_GPU), input.getStepBytes(MEM_GPU));
}



void camshift_thread(cuda::GpuMat& img)
{
	cuda::GpuMat hsv;
	cuda::cvt

}


void process_thread()
{


}



int main()
{

	// Set configuration parameters
	InitParameters init_params;
	init_params.camera_resolution = RESOLUTION_VGA;
	init_params.camera_fps = 100;
	init_params.depth_mode = DEPTH_MODE_NONE;
	init_params.coordinate_units = UNIT_METER;
	waitKey(2000);

	// Open the camera
	ERROR_CODE err = zed.open(init_params);
	if (err != SUCCESS) {
		printf("%s\n", errorCode2str(err).c_str());
		zed.close();
		return 1; // Quit if an error occurred
	}

	// Set runtime parameters after opening the camera

	//zed.setCameraSettings(CAMERA_SETTINGS_SATURATION,8);
	//zed.setCameraSettings(CAMERA_SETTINGS_CONTRAST,4);
	//zed.setCameraSettings(CAMERA_SETTINGS_GAIN,60);
	//zed.setCameraSettings(CAMERA_SETTINGS_EXPOSURE,50);
	RuntimeParameters runtime_parameters;
	runtime_parameters.sensing_mode = SENSING_MODE_STANDARD;


	//Define some variables to be use later and allocate memory
	Resolution image_size = zed.getResolution();
	cuda::HostMem MEM_L(image_size.height, image_size.width, CV_8UC3, cuda::HostMem::SHARED);
	cv::cuda::GpuMat image_cvGPU_L = MEM_L.createGpuMatHeader();
	sl::Mat image_zed_L = cvGpuMat2slGpuMat(image_cvGPU_L);
	cv::Mat image_cvCPU_L = MEM_L.createMatHeader();

	cuda::HostMem MEM_R(image_size.height, image_size.width, CV_8UC3, cuda::HostMem::SHARED);
	cv::cuda::GpuMat image_cvGPU_R = MEM_R.createGpuMatHeader();
	sl::Mat image_zed_R = cvGpuMat2slGpuMat(image_cvGPU_R);
	cv::Mat image_cvCPU_R = MEM_L.createMatHeader();

	cv::cuda::Stream stream;
	IMAGE_PAIR image_temp;



	namedWindow("L", 0);
	setMouseCallback("L", onMouse, 0);
	createTrackbar("Vmin", "L", &vmin, 256, 0);
	createTrackbar("Vmax", "L", &vmax, 256, 0);
	createTrackbar("Smin", "L", &smin, 256, 0);

	int hsize = 4;
	float hranges[] = { 0,180 };
	const float* phranges = hranges;

	bool stop = false;
	char c;

	vector<int> a;

	while (!stop)
	{


		if (zed.grab(runtime_parameters) == SUCCESS) {

			// Retrieve the left image, depth image in half-resolution
			zed.retrieveImage(image_zed_L, VIEW_LEFT, MEM_GPU);
			zed.retrieveImage(image_zed_R, VIEW_RIGHT, MEM_GPU);
			image_temp.L = image_cvGPU_L;
			image_temp.R = image_cvGPU_R;
			image_temp.finish = false;
			image_temp.position = cv::Mat::zeros(4, 1, CV_32FC1);
			while (!mtx.trylock()) {}
			image_pair_vector.push_back(image_temp);
			mtx.unlock();
		}
		else
		{
			cout << "failed" << endl;
			break;
		}

		if (trackObject)
		{


			if (trackObject < 0)
			{
				// Object has been selected by user, set up CAMShift search properties once
				cv::Mat hsv, mask;
				cvtColor(image_cvCPU_L, hsv, COLOR_BGR2HSV);
				int _vmin = vmin, _vmax = vmax;
				inRange(hsv, Scalar(0, smin, MIN(_vmin, _vmax)),
					Scalar(180, 256, MAX(_vmin, _vmax)), mask);
				int ch[] = { 0, 0 };
				cv::Mat hue;
				hue.create(hsv.size(), hsv.depth());
				mixChannels(&hsv, 1, &hue, 1, ch, 1);

				cv::Mat roi(hue, selection), maskroi(mask, selection);
				calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
				normalize(hist, hist, 0, 255, NORM_MINMAX);
				trackObject = 1; // Don't set up again, unless user selects new ROI
			}

		}


		imshow("L", image_cvCPU_L);
		char c = (char)waitKey(1);
		if (c == 27)
			break;
		switch (c)
		{
		case 'b':
			backprojMode = !backprojMode;
			break;
		case 'h':
			showHist = !showHist;
			if (!showHist)
				destroyWindow("Histogram");
			else
				namedWindow("Histogram", 1);
			break;
		default:
			;
		}


	}
	zed.close();
}


