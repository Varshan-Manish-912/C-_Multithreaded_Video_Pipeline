# Performance Benchmarks: Concurrent Video Pipeline

This document details the performance evaluation of the Multithreaded Video Processing Engine. The benchmarks compare a standard sequential processing pipeline against our decoupled, producer-consumer concurrent architecture.

## Overview of Metrics
*   **Sequential FPS:** Frames processed per second utilizing a single thread for both I/O (decoding) and processing (filtering).
*   **Concurrent FPS:** Frames processed per second utilizing the multithreaded architecture.
*   **FPS Improvement (%):** The percentage increase in throughput.
*   **Time Reduction (%):** The percentage decrease in total execution time.

---

## Executive Summary

| Sample | Frame Count | Seq FPS | Conc FPS | FPS Improvement | Time Reduction | Bottleneck Profile |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **Sample 1** | 902 | 26.34 | 28.35 | **+7.64%** | -7.10% | I/O Bound (Heavy Decode) |
| **Sample 2** | 3,880 | 35.17 | 36.12 | **+2.69%** | -2.62% | I/O Bound (Heavy Decode) |
| **Sample 3** | 10,920 | 44.81 | 48.93 | **+9.19%** | -8.42% | I/O Bound (Heavy Decode) |
| **Sample 4** | 5,078 | 55.72 | 91.61 | **+64.38%** | -39.17% | CPU Bound (Heavy Filter) |
| **Sample 5** | 4,628 | 58.93 | 85.83 | **+45.65%** | -31.34% | CPU Bound (Heavy Filter) |

---

## Detailed Results

### Sample 1
*   **Total Frames:** 902
*   **Sequential Time:** 34.24 seconds (26.34 FPS)
*   **Concurrent Time:** 31.81 seconds (28.35 FPS)
*   **Throughput Delta:** +7.64% FPS

### Sample 2
*   **Total Frames:** 3,880
*   **Sequential Time:** 110.31 seconds (35.17 FPS)
*   **Concurrent Time:** 107.42 seconds (36.12 FPS)
*   **Throughput Delta:** +2.69% FPS

### Sample 3
*   **Total Frames:** 10,920
*   **Sequential Time:** 243.68 seconds (44.81 FPS)
*   **Concurrent Time:** 223.17 seconds (48.93 FPS)
*   **Throughput Delta:** +9.19% FPS

### Sample 4
*   **Total Frames:** 5,078
*   **Sequential Time:** 91.11 seconds (55.72 FPS)
*   **Concurrent Time:** 55.43 seconds (91.61 FPS)
*   **Throughput Delta:** +64.38% FPS

### Sample 5
*   **Total Frames:** 4,628
*   **Sequential Time:** 78.52 seconds (58.93 FPS)
*   **Concurrent Time:** 53.91 seconds (85.83 FPS)
*   **Throughput Delta:** +45.65% FPS

---

## Systems Engineering Analysis

The benchmark results demonstrate the fundamental constraints of a concurrent producer-consumer pipeline. The architecture successfully yields massive performance gains (up to **64.38%**), but the efficacy is strictly dictated by the workload balance between the threads.

### 1. The CPU-Bound Workload (Samples 4 & 5)
In Samples 4 and 5, the multithreaded architecture shows dramatic improvements (+64% and +45% respectively). In these scenarios, the video codec is relatively lightweight to decode, meaning the `cv::VideoCapture` (Producer) can read frames faster than the image filter (Consumer) can process them sequentially.

By decoupling these tasks, the Capture Thread can continuously pre-fetch and decode frames into the thread-safe queue while the Processing Thread computes the filters, resulting in maximum hardware utilization and near-optimal pipeline efficiency.

### 2. The I/O-Bound Workload (Samples 1, 2, & 3)
In Samples 1, 2, and 3, the throughput improvements are marginal (~2% to 9%). This indicates an **I/O bottleneck** caused by heavy video decoding (e.g., highly compressed 4K H.264/HEVC footage).

Because the time required by the OS and OpenCV to read and decode a single frame exceeds the time required to apply the image filter, the Processing Thread is frequently starved. The concurrent pipeline cannot process data faster than the Capture Thread can decode it, demonstrating that overall pipeline throughput is always limited by its slowest discrete component: `Max(Decode_Time, Process_Time)`.