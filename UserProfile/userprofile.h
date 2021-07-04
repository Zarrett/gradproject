#pragma once

#include <QObject>
#include <QDataStream>
#include <string>

class UserProfile
{

public:
    UserProfile(int userID, std::string& path);
    UserProfile(int userID, std::vector<std::string>& path);

    void addPicPath(std::string& path);
    void addPicPaths(std::vector<std::string>& paths);

    std::vector<std::string>& getPicPaths();
    int getID();

private:
    int id;
    std::vector<std::string> picturesPaths;
};

