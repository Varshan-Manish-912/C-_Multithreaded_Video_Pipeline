#include <opencv2/opencv.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <atomic>
#include <chrono>
#include "Filter.hpp"
#include "EdgeFilter.hpp"
#include "BlurFilter.hpp"
#include "ThreadSafeQueue.hpp"

std::atomic<bool> running = true;
void captureFrames(
    cv::VideoCapture& cap,
    ThreadSafeQueue<cv::Mat>& rawFrameQueue
) {
    cv::Mat frame;
    while (running) {
        cap >> frame;
        if (frame.empty()) {
            break;
        }
        rawFrameQueue.push(frame.clone());
        while (rawFrameQueue.size() > 100 && running) {
            std::this_thread::yield();
        }
    }
    rawFrameQueue.push(cv::Mat());
}
void processFrames(
    ThreadSafeQueue<cv::Mat>& rawFrameQueue,
    ThreadSafeQueue<cv::Mat>& processedFrameQueue,
    std::unique_ptr<Filter>& filter
) {
    while (running) {
        cv::Mat frame = rawFrameQueue.pop();
        if (frame.empty()) {
            break;
        }
        cv::Mat processedFrame = filter->apply(frame);
        processedFrameQueue.push(processedFrame);
        while (processedFrameQueue.size() > 10 && running) {
            std::this_thread::yield();
        }
    }
    processedFrameQueue.push(cv::Mat());
}
int main() {
    cv::VideoCapture cap("../videos/sample2.mp4");
    if (!cap.isOpened()) {
        std::cout << "Failed to open video\n";
        return -1;
    }
    double videoFPS = cap.get(cv::CAP_PROP_FPS);
    int frameDelay = static_cast<int>(1000.0 / videoFPS);
    ThreadSafeQueue<cv::Mat> rawFrameQueue;
    ThreadSafeQueue<cv::Mat> processedFrameQueue;
    std::unique_ptr<Filter> filter = std::make_unique<EdgeFilter>();
    std::string currentFilter = "Edge Detection";
    std::thread captureThread(
        captureFrames,
        std::ref(cap),
        std::ref(rawFrameQueue)
    );
    std::thread processingThread(
        processFrames,
        std::ref(rawFrameQueue),
        std::ref(processedFrameQueue),
        std::ref(filter)
    );
    auto previousTime = std::chrono::high_resolution_clock::now();
    double fps = 0.0;
    while (running) {
        cv::Mat processedFrame = processedFrameQueue.pop();
        if (processedFrame.empty()) {
            running = false;
            break;
        }
        auto currentTime = std::chrono::high_resolution_clock::now();
        double deltaTime = std::chrono::duration<double>(currentTime - previousTime).count();
        previousTime = currentTime;
        fps = 1.0 / deltaTime;
        std::string rawQueueText = "Raw Queue: " + std::to_string(rawFrameQueue.size());
        cv::putText(processedFrame, rawQueueText, cv::Point(20, 40), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
        std::string processedQueueText = "Processed Queue: " + std::to_string(processedFrameQueue.size());
        cv::putText(processedFrame, processedQueueText, cv::Point(20, 80), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(fps));
        cv::putText(processedFrame, fpsText, cv::Point(20, 120), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
        std::string filterText = "Filter: " + currentFilter;
        cv::putText(processedFrame, filterText, cv::Point(20, 160), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
        cv::imshow("Processed Video", processedFrame);
        int key = cv::waitKey(1);
        if (key == '1') {
            filter = std::make_unique<EdgeFilter>();
            currentFilter = "Edge Detection";
        } else if (key == '2') {
            filter = std::make_unique<BlurFilter>();
            currentFilter = "Gaussian Blur";
        } else if (key == 27) {
            running = false;
        }
    }
    captureThread.join();
    processingThread.join();
    return 0;
}