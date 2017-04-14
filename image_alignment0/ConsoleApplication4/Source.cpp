/**
* OpenCV Image Alignment  Example
*
* Copyright 2015 by Satya Mallick <spmallick@learnopencv.com>
*
*/

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

Mat GetGradient(Mat src_gray)
{
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	int scale = 1;
	int delta = 0;
	int ddepth = CV_32FC1; ;

	// Calculate the x and y gradients using Sobel operator

	Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_x, abs_grad_x);

	Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT);
	convertScaleAbs(grad_y, abs_grad_y);

	// Combine the two gradients
	Mat grad;
	addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);

	return grad;

}


int main(int argc, char** argv)
{

	// Read 8-bit color image.
	// This is an image in which the three channels are
	// concatenated vertically.
	Mat im = imread("girls.jpg", IMREAD_GRAYSCALE);

	// Find the width and height of the color image
	Size sz = im.size();
	int height = sz.height / 3;
	int width = sz.width;

	// Extract the three channels from the gray scale image
	vector<Mat>channels1;

	channels1.push_back(im(Rect(0, 0, width, height)));
	channels1.push_back(im(Rect(0, height, width, height)));
	channels1.push_back(im(Rect(0, 2 * height, width, height)));
	Mat im_color;
	merge(channels1, im_color);
	//imshow("rwre", im_color);

	// Set space for aligned image.
	vector<Mat> aligned_channels1;
	aligned_channels1.push_back(Mat(height, width, CV_8UC1));
	aligned_channels1.push_back(Mat(height, width, CV_8UC1));

	// The blue and green channels will be aligned to the red channel.
	// So copy the red channel
	aligned_channels1.push_back(channels1[2].clone());


	// Define motion model
	const int warp_mode = MOTION_AFFINE;

	// Set space for warp matrix.
	Mat warp_matrix;

	// Set the warp matrix to identity.
	if (warp_mode == MOTION_HOMOGRAPHY)
		warp_matrix = Mat::eye(3, 3, CV_32F);
	else
		warp_matrix = Mat::eye(2, 3, CV_32F);

	// Set the stopping criteria for the algorithm.
	int number_of_iterations = 10;
	double termination_eps = 1e-10;

	TermCriteria criteria(TermCriteria::COUNT + TermCriteria::EPS,
		number_of_iterations, termination_eps);

	imshow("gradient", GetGradient(channels1[2]));

	// Warp the blue and green channels to the red channel
	for (int i = 0; i < 2; i++)
	{

		double cc = findTransformECC(
			GetGradient(channels1[2]),
			GetGradient(channels1[i]),
			warp_matrix,
			warp_mode,
			criteria
		);

		cout << "warp_matrix : " << warp_matrix << endl;
		cout << "CC " << cc << endl;
		if (cc == -1)
		{
			cerr << "The execution was interrupted. The correlation value is going to be minimized." << endl;
			cerr << "Check the warp initialization and/or the size of images." << endl << flush;
		}


		if (warp_mode == MOTION_HOMOGRAPHY)
			// Use Perspective warp when the transformation is a Homography
			warpPerspective(channels1[i], aligned_channels1[i], warp_matrix, aligned_channels1[0].size(), INTER_LINEAR + WARP_INVERSE_MAP);
		else
			// Use Affine warp when the transformation is not a Homography
			warpAffine(channels1[i], aligned_channels1[i], warp_matrix, aligned_channels1[0].size(), INTER_LINEAR + WARP_INVERSE_MAP);

	}

	// Merge the three channels
	Mat im_aligned;
	merge(aligned_channels1, im_aligned);
	cout << " channel10.depth =  " << aligned_channels1[0].depth() << endl;
	cout << " channel11.depth =  " << aligned_channels1[1].depth() << endl;
	cout << " channel12.depth =  " << aligned_channels1[2].depth() << endl;

	cout << " channel10.chhaane =  " << aligned_channels1[0].channels() << endl;
	cout << " channel11.chhaane =  " << aligned_channels1[1].channels() << endl;
	cout << " channel12.chhaane =  " << aligned_channels1[2].channels() << endl;

	cout << " channel10.chhaane =  " << aligned_channels1[0].size() << endl;
	cout << " channel11.chhaane =  " << aligned_channels1[1].size() << endl;
	cout << " channel12.chhaane =  " << aligned_channels1[2].size() << endl;
	// Show final output
	imshow("Color Image", im_color);
	imshow("Aligned Image", im_aligned);
	waitKey(0);


}
