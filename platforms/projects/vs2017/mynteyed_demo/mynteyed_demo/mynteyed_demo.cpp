// mynteyed_demo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <iostream>

#include <opencv2/highgui/highgui.hpp>

#include <mynteye/camera.h>
#include <mynteye/utils.h>

int main()
{
	mynteye::Camera cam;
	mynteye::DeviceInfo dev_info;
	if (!mynteye::util::select(cam, &dev_info)) {
		return 1;
	}
	mynteye::util::print_stream_infos(cam, dev_info.index);

	std::cout << "Open device: " << dev_info.index << ", "
		<< dev_info.name << std::endl << std::endl;

	// Warning: Color stream format MJPG doesn't work.
	mynteye::InitParams params(dev_info.index);
	params.depth_mode = mynteye::DepthMode::DEPTH_COLORFUL;
	params.stream_mode = mynteye::StreamMode::STREAM_2560x720;
	params.ir_intensity = 4;
	params.framerate = 30;

	cam.EnableImageType(mynteye::ImageType::ALL);
	cam.Open(params);

	std::cout << std::endl;
	if (!cam.IsOpened()) {
		std::cerr << "Error: Open camera failed" << std::endl;
		return 1;
	}
	std::cout << "Open device success" << std::endl << std::endl;

	std::cout << "Press ESC/Q on Windows to terminate" << std::endl;

	cv::namedWindow("color");
	cv::namedWindow("depth");

	for (;;) {
		auto left_color = cam.RetrieveImage(mynteye::ImageType::IMAGE_LEFT_COLOR);
		auto right_color = cam.RetrieveImage(mynteye::ImageType::IMAGE_RIGHT_COLOR);
		if (left_color.img && right_color.img) {
			cv::Mat left = left_color.img->To(mynteye::ImageFormat::COLOR_BGR)->ToMat();
			cv::Mat right = right_color.img->To(mynteye::ImageFormat::COLOR_BGR)->ToMat();
			cv::Mat img;
			cv::hconcat(left, right, img);
			cv::imshow("color", img);
		}

		auto image_depth = cam.RetrieveImage(mynteye::ImageType::IMAGE_DEPTH);
		if (image_depth.img) {
			cv::Mat depth = image_depth.img->To(mynteye::ImageFormat::DEPTH_BGR)->ToMat();
			cv::imshow("depth", depth);
		}

		char key = static_cast<char>(cv::waitKey(1));
		if (key == 27 || key == 'q' || key == 'Q') {  // ESC/Q
			break;
		}
		cam.Wait();  // keep frequency
	}

	cam.Close();
	cv::destroyAllWindows();
    return 0;
}
