#include <fstream>
#include <iostream>
#include <string>

#include <opencv2/core/core.hpp>

#include "userprofilecontroller.h"

constexpr std::string_view userCSVpath = "/home/pi/facesQT/faces/users.csv";

UserProfileController::UserProfileController()
{
    deserialiseUsers();
}

void UserProfileController::addUserProfile(UserProfile user)
{
    users.emplace(user.getID(), user);
    serialiseUsers();
}

void UserProfileController::serialiseUsers()
{
    std::ofstream file(userCSVpath.data(), std::ofstream::out | std::ofstream::trunc);

    if (!file.is_open())
    {
        std::string error_message = "No valid input file was given, please check the given filename.";
        std::cerr << error_message << std::endl;
        return;
    }

    for(auto& pair : users)
    {
        for(auto& path : pair.second.getPicPaths())
            file << path << ';' << pair.first << std::endl;
    }
}

void UserProfileController::deserialiseUsers()
{
    std::ifstream file(userCSVpath.data(), std::ifstream::in);
    users.clear();

    if (!file.is_open())
    {
        std::string error_message = "No valid input file was given, please check the given filename.";
        std::cerr << error_message << std::endl;
        return;
    }

    std::string line, path, id;
    while (getline(file, line))
    {
        std::stringstream lines(line);
        getline(lines, path, ';');
        getline(lines, id);
        if(!path.empty() && !id.empty())
        {
            int userID = atoi(id.c_str());

            if(users.find(userID) != users.end())
            {
                users.at(userID).addPicPath(path);
            }
            else
            {
                UserProfile prof(userID, path);
                users.emplace(userID, prof);
            }
        }
    }
}
