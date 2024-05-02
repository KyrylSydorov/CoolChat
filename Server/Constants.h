// Kyryl Sydorov, 2024

#pragma once

#include <string>

namespace Paths
{
    const extern std::string rootPath;
    const extern std::string usersPath;
    const extern std::string messagesPath;
}

namespace Errors
{
    constexpr int success = 0;
    constexpr int internalError = -1;
    constexpr int invalidCredentials = -2;
    constexpr int userAlreadyExists = -3;
    constexpr int noSuchMessage = -4;
    constexpr int unauthorized = -5;
}

namespace Constants
{
    constexpr int minNameLength = 3;
    constexpr int minPasswordLength = 6;
}

enum class RequestType : int
{
    Invalid = -1,
    Login,
    Register,
    GetUserInfo,
    SendMessageTo,
    GetMessageTo,
    GetMessages,
    FetchDialogStates,
    GetLastMessages,
    UpdateDialogState,
};