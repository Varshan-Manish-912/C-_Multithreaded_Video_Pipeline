#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

int main() {
    cv::VideoCapture cap("../videos/sample5.mp4");
    if (!cap.isOpened()) {
        std::cout << "Failed to open video\n";
        return -1;
    }
    cv::Mat frame;
    cv::Mat gray;
    cv::Mat blurred;
    cv::Mat edges;
    int frameCount = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            break;
        }
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        cv::GaussianBlur(gray, blurred, cv::Size(5, 5), 0);
        cv::Canny(blurred, edges, 50, 150);
        frameCount++;
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    double averageFPS = frameCount / totalTime;
    std::cout << "\n===== Sequential Benchmark =====\n";
    std::cout << "Frames Processed: " << frameCount << "\n";
    std::cout << "Total Time: " << totalTime << " seconds\n";
    std::cout << "Average FPS: " << averageFPS << "\n";
    return 0;
}