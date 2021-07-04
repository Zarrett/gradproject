#pragma once

#include "userprofilecontroller.h"
#include "Adafruit_Fingerprint.h"
#include "fingerprintExample.hpp"
#include "enrollExample.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/face.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <QLabel>
#include <thread>
#include <iostream>

enum class EVidOutMode : int
{
    OUTPUT_VIDEO_ONLY,
    REGISTER_USER,
    RECOGNISE_USER
};

class ApplicationController
{
public:
    ApplicationController(QLabel* videoOutpu, QLabel* logLabel);
    void StopDisplayingImage();
    void addUser();
    void makePhoto();

    bool authenticate();

private:
    void TrainFaceRecogniserOnDummy();
    void UpdateFaceRecogniser(const std::string& path);
    void retrainRecogniser();

    void ProcessVideoOutput(EVidOutMode mode, std::vector<std::string>& paths, int userID, bool& auth);

    std::string savePhoto(cv::Mat& pic, int userID);
    void readCsv(const std::string& filename
                  , std::vector<cv::Mat>& images
                  , std::vector<int>& labels
                  , char separator = ';');



private:
    bool is_displaying = false;
    bool toMakePhoto = false;
    std::unique_ptr<std::thread> controllerThread;
    std::unique_ptr<UserProfileController> userProfcontroller;
    cv::Ptr<cv::face::FaceRecognizer> model;
    cv::CascadeClassifier haarCascade;
    std::unique_ptr<Adafruit_Fingerprint> finger;
    QLabel* vidOut; //not responsible
    QLabel* logOut; //not responsible
    size_t imWidth;
    size_t imHeight;

};

class Timer
{
public:
    void start()
    {
        m_StartTime = std::chrono::system_clock::now();
        m_bRunning = true;
    }

    void stop()
    {
        m_EndTime = std::chrono::system_clock::now();
        m_bRunning = false;
    }

    double elapsedMilliseconds()
    {
        std::chrono::time_point<std::chrono::system_clock> endTime;

        if(m_bRunning)
        {
            endTime = std::chrono::system_clock::now();
        }
        else
        {
            endTime = m_EndTime;
        }

        return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
    }

    double elapsedSeconds()
    {
        return elapsedMilliseconds() / 1000.0;
    }

private:
    std::chrono::time_point<std::chrono::system_clock> m_StartTime;
    std::chrono::time_point<std::chrono::system_clock> m_EndTime;
    bool                                               m_bRunning = false;
};

