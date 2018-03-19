#include <iostream>
#include <opencv2/opencv.hpp>
#include <caffe/blob.hpp>
using namespace std;
using namespace cv;

int main(int argc, char **argv)
{
    caffe::Blob<float> Blb;
    cout << "Size: " << Blb.shape_string() << endl;
    Blb.Reshape(1, 2, 3, 4);
    cout << Blb.shape_string() << endl;
}