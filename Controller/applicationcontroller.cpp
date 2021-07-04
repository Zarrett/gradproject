#include "applicationcontroller.h"

#include <fstream>
#include <QFileSystemModel>

constexpr size_t dummyFaceCount = 40;
constexpr std::string_view fn_haar = "/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
constexpr std::string_view fn_csv = "/home/pi/facesQT/faces/dummyFaces.csv";
constexpr std::string_view fn_csv_usr = "/home/pi/facesQT/faces/users.csv";

static const QString FingerprintSerial = "/dev/serial0";
static const QString FingerPrinttBaudrate = "57600";

ApplicationController::ApplicationController(QLabel* videoOutput, QLabel* logLabel) : userProfcontroller(new UserProfileController())
{
    vidOut = videoOutput;
    logOut = logLabel;
    model = cv::face::LBPHFaceRecognizer::create();
    TrainFaceRecogniserOnDummy();
    UpdateFaceRecogniser(fn_csv_usr.data());
}

void ApplicationController::retrainRecogniser()
{
    model->clear();
    TrainFaceRecogniserOnDummy();
    UpdateFaceRecogniser(fn_csv_usr.data());
}

void ApplicationController::ProcessVideoOutput(EVidOutMode mode, std::vector<std::string>& paths, int userID, bool& auth)
{
    if(is_displaying)
        return;

        auto deviceId = 0;

        cv::VideoCapture cap(deviceId);
        cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));

        if(!cap.isOpened())
            std::cerr << "Capture Device ID " << deviceId << "cannot be opened." << std::endl;

        haarCascade.load(fn_haar.data());

        cv::Mat frame;


        Timer timer;
        int labell=-1;
        double accur=0.0;
        int countRecognised = 0;

        timer.start();
        is_displaying = true;
        while(is_displaying)
        {
            cap.read(frame);
            // Clone the cursolutionrent frame:
            cv::Mat original = frame.clone();
            // Convert the current frame to grayscale:
            cv::Mat gray;
            cvtColor(original, gray, cv::COLOR_BGRA2GRAY);
            // Find the faces in the frame:
            std::vector< cv::Rect_<int> > faces;
            haarCascade.detectMultiScale(gray, faces);

            for(size_t i = 0; i < faces.size(); i++)
            {
                cv::Rect face_i = faces[i];
                cv::Mat face = gray(face_i);
                cv::Mat face_resized;
                cv::resize(face, face_resized, cv::Size(100, 100), 1.0, 1.0, cv::INTER_CUBIC);
                rectangle(original, face_i, CV_RGB(0, 255,0), 1);

                if(mode == EVidOutMode::REGISTER_USER && toMakePhoto)
                    paths.push_back(savePhoto(face_resized, userID));

                if(mode == EVidOutMode::RECOGNISE_USER)
                {
                    int pos_x = std::max(face_i.tl().x - 10, 0);
                    int pos_y = std::max(face_i.tl().y - 10, 0);
                    std::string box_text = cv::format("Prediction = %d Confidence=%f", labell, accur);
                    putText(original, box_text, cv::Point(pos_x, pos_y), cv::FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,0,0), 2.0);
                    model->predict(face_resized, labell, accur);

                    if(labell == userID && accur > 60)
                        countRecognised++;

                    if(countRecognised > 20)
                    {
                        auth = true;
                        is_displaying = false;
                        break;
                    }
                    else if(timer.elapsedSeconds() > 20)
                    {
                        auth = false;
                        is_displaying = false;
                        break;
                    }
                }
            }

            if(is_displaying)
            {
                cvtColor(original, original, cv::COLOR_BGR2RGB);
                QImage image1= QImage((uchar*) original.data, original.cols, original.rows, original.step, QImage::Format_RGB888);

                vidOut->setPixmap(QPixmap::fromImage(image1));
                vidOut->show();
            }
        }
}

void ApplicationController::StopDisplayingImage()
{
    if(is_displaying)
    {
        is_displaying = false;
        controllerThread->join();
    }
}

void ApplicationController::UpdateFaceRecogniser(const std::string& path)
{
    std::vector<cv::Mat> images;
    std::vector<int> labels;
    try {
        readCsv(path, images, labels);
    } catch (cv::Exception& e) {
        std::cerr << "Error opening file \"" << path << "\". Reason: " << e.msg << std::endl;
        exit(1);
    }

    if(images.size() == 0)
        return;

    imWidth = images[0].cols;
    imHeight = images[0].rows;

    model->update(images, labels);
}

void ApplicationController::TrainFaceRecogniserOnDummy()
{
    std::vector<cv::Mat> images;
    std::vector<int> labels;
    try {
        readCsv(fn_csv.data(), images, labels);
    } catch (cv::Exception& e) {
        std::cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << std::endl;
        exit(1);
    }
    imWidth = images[0].cols;
    imHeight = images[0].rows;

    model->train(images, labels);
}

void ApplicationController::readCsv(const std::string& filename
                                    , std::vector<cv::Mat>& images
                                    , std::vector<int>& labels
                                    , char separator)
{
    std::ifstream file(filename.c_str(), std::ifstream::in);

    if (!file)
    {
        std::string error_message = "No valid input file was given, please check the given filename.";
        std::cerr << error_message << std::endl;
        return;
    }

    std::string line, path, classlabel;
    while (getline(file, line))
    {
        std::stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty())
        {
            const auto& im = cv::imread(path, 0);
            cv::Mat resized;
            if(!im.empty())
            {
                resize(im, resized, cv::Size(92, 112), 1.0, 1.0, cv::INTER_CUBIC);
                images.push_back(im);
                labels.push_back(atoi(classlabel.c_str()));
            }
        }
    }
}

void ApplicationController::addUser()
{

    controllerThread = std::make_unique<std::thread>([this]()
    {
        auto userID = userProfcontroller->getUserDBSize() + dummyFaceCount + 1;

        logOut->setText(logOut->text() + QString("Adding user under ID %d \n").arg(userID));

        logOut->setText(logOut->text() + QString("Adding fingerprint\n"));

        finger.reset(new Adafruit_Fingerprint{FingerprintSerial, FingerPrinttBaudrate});
        finger->begin(FingerPrinttBaudrate);
        EnrollExample::runEnroll(*finger.get(), userID);
        finger->close();

        std::vector<std::string> paths;

        bool dummy;
        ProcessVideoOutput(EVidOutMode::REGISTER_USER, paths, userID, dummy);

        UserProfile prof (userID, paths);

        userProfcontroller->addUserProfile(prof);
        vidOut->hide();
        retrainRecogniser();
    });

}

bool ApplicationController::authenticate()
{

    bool faceRecognised;

    controllerThread = std::make_unique<std::thread>([this, &faceRecognised]()
    {
        finger.reset(new Adafruit_Fingerprint{FingerprintSerial, FingerPrinttBaudrate});
        finger->begin(FingerPrinttBaudrate);
        int userID = FingerprintExample::runFind(*finger.get());
        finger->close();

        std::vector<std::string> dummy;
        ProcessVideoOutput(EVidOutMode::RECOGNISE_USER, dummy, userID, faceRecognised);
    });

    while(!controllerThread->joinable()){}

    controllerThread->join();
    vidOut->hide();

    return faceRecognised;
}

void ApplicationController::makePhoto()
{
    if(is_displaying)
        toMakePhoto = true;
}

std::string ApplicationController::savePhoto(cv::Mat& pic, int userID)
{
    assert(is_displaying);
    assert(toMakePhoto);

    toMakePhoto = false;

    auto saveDirPath = "/home/pi/facesQT/faces/g" + QVariant(userID).toString();

    if(!QDir(saveDirPath).exists())
        QDir().mkdir(saveDirPath);

    int fileIndex = 0;

    QString saveFilePath = saveDirPath + "/p" + QVariant(fileIndex).toString() + ".bmp";

    while(QFile(saveFilePath).exists())
    {
        fileIndex++;
        saveFilePath = saveDirPath + "/p" + QVariant(fileIndex).toString() + ".bmp";
    }

    std::cout << saveFilePath.toStdString() << std::endl;

    cv::imwrite(saveFilePath.toStdString(), pic);

    return saveFilePath.toStdString();

}

/*
    string fn_haar = "/usr/local/share/opencv4/haarcascades/haarcascade_frontalface_default.xml";
    string fn_csv = "/home/zarret/faces.csv";//string(argv[2]);
    int deviceId = 0;//atoi(argv[3]);

    CascadeClassifier haar_cascade;
    haar_cascade.load(fn_haar);
    VideoCapture cap(deviceId);
    cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'));
    // Check if we can use this device at all:
    if(!cap.isOpened()) {
        cerr << "Capture Device ID " << deviceId << "cannot be opened." << endl;
        return -1;
    }
    // Holds the current frame from the Video device:
    Mat frame;

    int labell=-1;
    double accur=0.0;
    for(;;)
    {
        cap.read(frame);
        // Clone the current frame:
        Mat original = frame.clone();
        // Convert the current frame to grayscale:
        Mat gray;
        cvtColor(original, gray, COLOR_BGRA2GRAY);

        // Find the faces in the frame:
        vector< Rect_<int> > faces;
        haar_cascade.detectMultiScale(gray, faces);

        for(int i = 0; i < faces.size(); i++)
        {
            Rect face_i = faces[i];
            Mat face = gray(face_i);
            Mat face_resized;
            cv::resize(face, face_resized, Size(im_width, im_height), 1.0, 1.0, INTER_CUBIC);
            model->predict(face_resized, labell, accur);
            rectangle(original, face_i, CV_RGB(0, 255,0), 1);
            string box_text = format("Prediction = %d Confidence=%f", labell, accur);
            int pos_x = std::max(face_i.tl().x - 10, 0);
            int pos_y = std::max(face_i.tl().y - 10, 0);
            // And now put it into the image:
            putText(original, box_text, Point(pos_x, pos_y), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(255,0,0), 2.0);
        }

        imshow("face_recognizer", original);
        char key = (char) waitKey(20);
        // Exit this loop on escape:
        if(key == 27)
            break;
    }*/

    /*#include "opencv2/core/core.hpp"
#include "opencv2/face.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"

#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>

using namespace cv;
using namespace std;*/
