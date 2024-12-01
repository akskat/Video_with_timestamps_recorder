#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <vector>

int main(int argc, char** argv)
{
    // Åpne videokamera med v4l2 backend
    cv::VideoCapture cap(0, cv::CAP_V4L2);
    if (!cap.isOpened()) {
        std::cerr << "Error: Kunne ikke åpne kameraet." << std::endl;
        return -1;
    }

    // Sett til MJPG for høy FPS ved høy oppløsning
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

    // Sett ønsket oppløsning og FPS
    int width = 1280;  // Oppløsning bredde for 30 FPS
    int height = 720;  // Oppløsning høyde for 30 FPS
    cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    cap.set(cv::CAP_PROP_FPS, 30);

    // Hent faktisk FPS fra kameraet
    double fps = cap.get(cv::CAP_PROP_FPS);
    if (fps < 1.0) {
        std::cerr << "Warning: Kunne ikke hente FPS fra kameraet. Setter til 30." << std::endl;
        fps = 30.0;
    }
    std::cout << "FPS: " << fps << std::endl;

    // Opprett VideoWriter for å lagre video
    cv::VideoWriter video("raw_output.mp4", cv::VideoWriter::fourcc('a', 'v', 'c', '1'), fps, cv::Size(width, height));
    if (!video.isOpened()) {
        std::cerr << "Error: Kunne ikke lagre videoen." << std::endl;
        return -1;
    }

    // Åpne fil for å lagre tidsstempler
    std::ofstream timestampFile("timestamps.txt");
    if (!timestampFile.is_open()) {
        std::cerr << "Error: Kunne ikke åpne tidsstempelfilen." << std::endl;
        return -1;
    }

    // Vektor for tidsstempler
    std::vector<std::chrono::system_clock::time_point> timestamps;

    // Hovedopptaksløkke
    while (true) {
        cv::Mat frame;
        cap >> frame; // Hent et nytt bilde fra kameraet

        if (frame.empty()) {
            std::cerr << "Warning: Tom ramme mottatt." << std::endl;
            continue; // Hopp over tomme rammer
        }

        // Hent systemtid for denne framen
        timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        auto now = std::chrono::system_clock::time_point(std::chrono::seconds(ts.tv_sec) + std::chrono::nanoseconds(ts.tv_nsec));
        timestamps.push_back(now);

        // Lagre bildet til video
        video.write(frame);

        // Vis bildet i et vindu
        cv::imshow("Video", frame);

        // Avslutt når 'q' trykkes
        if (cv::waitKey(1) == 'q') {
            std::cout << "Avslutter opptak..." << std::endl;
            break;
        }
    }

    // Lukk ressurser etter opptak
    cap.release();
    video.release();
    cv::destroyAllWindows();
    timestampFile.close();

    // Etterbehandling: Legg til tidsstempler på videoen
    std::cout << "Legger til tidsstempler på videoen..." << std::endl;

    // Åpne original video og lage ny video med tidsstempler
    cv::VideoCapture inputVideo("raw_output.mp4");
    cv::VideoWriter outputVideo("output_with_timestamps.mp4", cv::VideoWriter::fourcc('a', 'v', 'c', '1'), fps, cv::Size(width, height));

    if (!inputVideo.isOpened() || !outputVideo.isOpened()) {
        std::cerr << "Error: Kunne ikke åpne videofilene for etterbehandling." << std::endl;
        return -1;
    }

    int newFrameCount = 0;
    cv::Mat frame;

    while (inputVideo.read(frame)) {
        if (newFrameCount >= timestamps.size()) {
            break; // Sikre at vi ikke går ut av tidsstempel-liste
        }

        // Hent nøyaktig systemtid for denne framen
        auto actual_time = timestamps[newFrameCount];

        // Konverter tid til streng (HH:MM:SS.sss)
        auto time_t_now = std::chrono::system_clock::to_time_t(actual_time);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(actual_time.time_since_epoch()) % 1000;
        std::tm local_tm = *std::localtime(&time_t_now);
        std::stringstream timeStream;
        timeStream << std::put_time(&local_tm, "%H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count();

        // Legg til tidsstempel på bildet
        std::string timeText = "Time: " + timeStream.str();
        cv::putText(frame, timeText, cv::Point(10, height - 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 1);

        // Lagre bildet til ny video med tidsstempler
        outputVideo.write(frame);
        newFrameCount++;
    }

    // Lukk videofiler etter etterbehandling
    inputVideo.release();
    outputVideo.release();

    std::cout << "Etterbehandling ferdig. Video lagret som 'output_with_timestamps.mp4'." << std::endl;

    return 0;
}
