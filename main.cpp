
//Opencv includes
#include <opencv2/core/utility.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/video/tracking.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
//#include "cuda.h"
#include "opencv2/core/cuda.hpp"
//#include "opencv2/gpu/gpu.hpp"

//Stander includes
#include <math.h>
#include <iostream>
#include <ctype.h>
#include <thread>
#include <queue>
// ZED includes
#include <sl/Camera.hpp>

//Communication includes
//#include "can_send.h"
//#include "linux_sever.h"

using namespace sl;
using namespace cv;
using namespace std;

// draw a box around object to track. This triggers CAMShift to start tracking
static void onMouse( int event, int x, int y, int, void* )
{
    if( selectObject )
    {
        selection.x = MIN(x, origin.x);
        selection.y = MIN(y, origin.y);
        selection.width = std::abs(x - origin.x);
        selection.height = std::abs(y - origin.y);
        selection &= Rect(0, 0, image_ocv.cols, image_ocv.rows);
    }
    switch( event )
    {
    case EVENT_LBUTTONDOWN:
        origin = Point(x,y);
        selection = Rect(x,y,0,0);
        selectObject = true;
        break;
    case EVENT_LBUTTONUP:
        selectObject = false;
        if( selection.width > 0 && selection.height > 0 )
            trackObject = -1;   // Set up CAMShift properties in main() loop
        break;
    }
}

//Covert sl mat to cv mat gpu
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
	return cv::cuda::GpuMat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM_GPU),input.getStepBytes(MEM_GPU));
}

typedef struct{

  cv::cuda::GpuMat L;
  cv::cuda::GpuMat R;

}IMAGE_PAIR


int main()
{
	// Create a ZED camera object
	Camera zed;

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
    // zed.setCameraSettings(CAMERA_SETTINGS_CONTRAST,4);
    //zed.setCameraSettings(CAMERA_SETTINGS_GAIN,60);
    //zed.setCameraSettings(CAMERA_SETTINGS_EXPOSURE,50);
	RuntimeParameters runtime_parameters;
	runtime_parameters.sensing_mode = SENSING_MODE_STANDARD;


	//Define some variables to be use later and allocate memory
	Resolution image_size = zed.getResolution();
	sl::Mat image_zed_L(image_size, MAT_TYPE_8U_C4,MEM_GPU);
	cv::cuda::GpuMat image_cvGPU_L = slGpuMat2cvGpuMat(image_zed);
	cv::Mat image_cvCPU_L(image_size.height, image_size.width,image_cvGPU.type());

	sl::Mat image_zed_R(image_size, MAT_TYPE_8U_C4,MEM_GPU);
	cv::cuda::GpuMat image_cvGPU_R = slGpuMat2cvGpuMat(image_zed);
	cv::Mat image_cvCPU_R(image_size.height, image_size.width,image_cvGPU.type());

    cv::cuda::Stream stream;
    IMAGE_PAIR image_temp;
    queue<IMAGE_PAIR> image_queue;


	bool stop = false;
	char c;

	while (!stop)
	{


		if (zed.grab(runtime_parameters) == SUCCESS) {

			// Retrieve the left image, depth image in half-resolution
			zed.retrieveImage(image_zed_L, VIEW_LEFT, MEM_GPU);
            zed.retrieveImage(image_zed_R, VIEW_RIGHT, MEM_GPU);
            image_temp.L = image_cvG


		}
		else
		{
			cout << "failed" << endl;
			break;
		}


        image_cvCPU = cv::Mat(image_size.height, image_size.width,image_cvGPU.type());
		//cv::cuda::GpuMat Image_ocv_gpu = image_cvGPU.clone();
		image_cvGPU.download(image_cvCPU,stream);

		imshow("123", image_cvCPU);

		c = waitKey(0);
		if (c == 'q') stop = true;







	}
    zed.close();
}
