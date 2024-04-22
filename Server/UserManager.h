// Kyryl Sydorov, 2024

#pragma once

#include <iostream>
#include <string>

inline int hashString(const std::string& str)
{
    // This is a very simple hash function.
    int hash = 0;
    for (const char c : str)
    {
        hash = hash * 31 + c;
    }
    return hash;
}

struct UserInfo
{
    std::string nickname;
    std::string firstName;
    std::string lastName;
    std::string biography;
    std::string password;
    
    int hashedNickname = -1;

    inline bool isValid() const { return hashedNickname != -1; }
};

inline std::istream& operator>>(std::istream& in, UserInfo& userInfo)
{
    in  >> userInfo.nickname
        >> userInfo.firstName
        >> userInfo.lastName
        >> userInfo.biography
        >> userInfo.password;

    userInfo.hashedNickname = hashString(userInfo.nickname);

    return in;
}

inline std::ostream& operator<<(std::ostream& out, const UserInfo& userInfo)
{
    return out
        << userInfo.nickname << ' '
        << userInfo.firstName << ' '
        << userInfo.lastName << ' '
        << userInfo.biography << ' '
        << userInfo.password;
}

class UserManager
{
public:
    static UserInfo getUserInfo(int id);
    static int registerUser(const UserInfo& userInfo);
    static int getIdByNickname(const std::string& nickname);
};
