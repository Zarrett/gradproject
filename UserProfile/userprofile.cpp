#include "userprofile.h"

UserProfile::UserProfile(int userID, std::string& path)
{
     id = userID;
     picturesPaths.push_back(path);
}

UserProfile::UserProfile(int userID, std::vector<std::string>& paths)
{
     id = userID;
     picturesPaths = paths;
}

void UserProfile::addPicPath(std::string& path)
{
    picturesPaths.push_back(path);
}

void UserProfile::addPicPaths(std::vector<std::string>& paths)
{
    for(auto& path : paths)
        picturesPaths.push_back(path);
}

std::vector<std::string>& UserProfile::getPicPaths()
{
    return picturesPaths;
}

int UserProfile::getID()
{
    return id;
}
