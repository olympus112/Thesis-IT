#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

int main() {
    std::string path = "../res/uv.jpg";
    cv::Mat img = cv::imread(path);
    
    imshow("Image", img);
    cv::waitKey(0);
}
