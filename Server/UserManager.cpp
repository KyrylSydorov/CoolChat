// Kyryl Sydorov, 2024

#include <filesystem>
#include <fstream>

#include "Constants.h"
#include "UserManager.h"

using namespace std;

UserInfo UserManager::getUserInfo(int id)
{
    ifstream userData(Paths::usersPath + '\\' + to_string(id));
    if (!userData.is_open())
    {
        return {};
    }

    UserInfo userInfo;
    userData >> userInfo;
    return userInfo;
}

int UserManager::registerUser(const UserInfo& userInfo)
{
    if (userInfo.nickname.length() < Constants::minNameLength ||
        userInfo.firstName.length() < Constants::minNameLength ||
        userInfo.lastName.length() < Constants::minNameLength ||
        userInfo.password.length() < Constants::minPasswordLength)
    {
        return Errors::invalidCredentials;
    }
    
    UserInfo existingUserInfo = getUserInfo(hashString(userInfo.nickname));
    if (existingUserInfo.isValid())
    {
        // Already registered
        return Errors::userAlreadyExists;
    }

    std::filesystem::create_directory(Paths::usersPath);
    
    ofstream userData(Paths::usersPath + '\\' + to_string(userInfo.hashedNickname));
    if (!userData.is_open())
    {
        return Errors::internalError;
    }

    userData << userInfo;

    return Errors::success;
}

int UserManager::getIdByNickname(const std::string& nickname)
{
    return hashString(nickname);
}
