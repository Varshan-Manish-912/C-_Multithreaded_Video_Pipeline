#include <opencv2/opencv.hpp>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include "../include/Filter.hpp"
#include "../include/EdgeFilter.hpp"
#include "../include/ThreadSafeQueue.hpp"

void captureFrames(
    cv::VideoCapture& cap,
    ThreadSafeQueue<cv::Mat>& rawFrameQueue
) {
    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) {
            rawFrameQueue.push(cv::Mat());
            break;
        }
        rawFrameQueue.push(frame.clone());
        while (rawFrameQueue.size() > 100) {
            std::this_thread::yield();
        }
    }
}
void processFrames(
    ThreadSafeQueue<cv::Mat>& rawFrameQueue,
    ThreadSafeQueue<cv::Mat>& processedFrameQueue,
    Filter& filter
) {
    while (true) {
        cv::Mat frame = rawFrameQueue.pop();
        if (frame.empty()) {
            processedFrameQueue.push(cv::Mat());
            break;
        }
        cv::Mat processedFrame = filter.apply(frame);
        processedFrameQueue.push(processedFrame);
        while (processedFrameQueue.size() > 10) {
            std::this_thread::yield();
        }
    }
}
int main() {
    cv::VideoCapture cap("../videos/sample5.mp4");
    if (!cap.isOpened()) {
        std::cout << "Failed to open video\n";
        return -1;
    }
    ThreadSafeQueue<cv::Mat> rawFrameQueue;
    ThreadSafeQueue<cv::Mat> processedFrameQueue;
    std::unique_ptr<Filter> filter = std::make_unique<EdgeFilter>();
    int frameCount = 0;
    auto startTime = std::chrono::high_resolution_clock::now();
    std::thread captureThread(
        captureFrames,
        std::ref(cap),
        std::ref(rawFrameQueue)
    );
    std::thread processingThread(
        processFrames,
        std::ref(rawFrameQueue),
        std::ref(processedFrameQueue),
        std::ref(*filter)
    );
    while (true) {
        cv::Mat processedFrame = processedFrameQueue.pop();
        if (processedFrame.empty()) {
            break;
        }
        frameCount++;
    }
    captureThread.join();
    processingThread.join();
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    double averageFPS = frameCount / totalTime;
    std::cout << "\n===== Concurrent Benchmark =====\n";
    std::cout << "Frames Processed: " << frameCount << "\n";
    std::cout << "Total Time: " << totalTime << " seconds\n";
    std::cout << "Average FPS: " << averageFPS << "\n";
    return 0;
}