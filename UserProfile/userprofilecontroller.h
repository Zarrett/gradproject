#pragma once

#include <QObject>
#include <QMap>

#include "userprofile.h"

class UserProfileController : public QObject
{
    Q_OBJECT
public:
    UserProfileController();
    void deleteUserProfile(int id);
    void addUserProfile(UserProfile user);

    int getUserDBSize() {return users.size();}
    const UserProfile& getUserProfile(int id) {return users.at(id);}

private:
    void serialiseUsers();
    void deserialiseUsers();

private:

    std::map<int, UserProfile> users;
};
