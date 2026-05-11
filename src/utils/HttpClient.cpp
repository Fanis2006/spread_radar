#include "utils/HttpClient.h"

#include <windows.h>
#include <winhttp.h>

#include <stdexcept>
#include <string>
#include <vector>

namespace {
    std::wstring toWideString(const std::string& value) {
        int sizeNeeded = MultiByteToWideChar(
            CP_UTF8,
            0,
            value.c_str(),
            static_cast<int>(value.size()),
            nullptr,
            0
        );

        if (sizeNeeded <= 0) {
            throw std::runtime_error("Failed to convert string to wide string");
        }

        std::wstring result(sizeNeeded, 0);

        MultiByteToWideChar(
            CP_UTF8,
            0,
            value.c_str(),
            static_cast<int>(value.size()),
            result.data(),
            sizeNeeded
        );

        return result;
    }

    void parseHttpsUrl(
        const std::string& url,
        std::wstring& host,
        std::wstring& path
    ) {
        const std::string prefix = "https://";

        if (url.rfind(prefix, 0) != 0) {
            throw std::runtime_error("Only HTTPS URLs are supported");
        }

        std::string withoutProtocol = url.substr(prefix.size());

        size_t slashPos = withoutProtocol.find('/');

        if (slashPos == std::string::npos) {
            host = toWideString(withoutProtocol);
            path = L"/";
            return;
        }

        host = toWideString(withoutProtocol.substr(0, slashPos));
        path = toWideString(withoutProtocol.substr(slashPos));
    }
}

std::string HttpClient::get(const std::string& url) const {
    std::wstring host;
    std::wstring path;

    parseHttpsUrl(url, host, path);

    HINTERNET session = WinHttpOpen(
        L"SpreadRadarBot/1.0",
        WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0
    );

    if (!session) {
        throw std::runtime_error("WinHttpOpen failed");
    }

    HINTERNET connection = WinHttpConnect(
        session,
        host.c_str(),
        INTERNET_DEFAULT_HTTPS_PORT,
        0
    );

    if (!connection) {
        WinHttpCloseHandle(session);
        throw std::runtime_error("WinHttpConnect failed");
    }

    HINTERNET request = WinHttpOpenRequest(
        connection,
        L"GET",
        path.c_str(),
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        WINHTTP_FLAG_SECURE
    );

    if (!request) {
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        throw std::runtime_error("WinHttpOpenRequest failed");
    }

    BOOL sendResult = WinHttpSendRequest(
        request,
        WINHTTP_NO_ADDITIONAL_HEADERS,
        0,
        WINHTTP_NO_REQUEST_DATA,
        0,
        0,
        0
    );

    if (!sendResult) {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        throw std::runtime_error("WinHttpSendRequest failed");
    }

    BOOL receiveResult = WinHttpReceiveResponse(request, nullptr);

    if (!receiveResult) {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        throw std::runtime_error("WinHttpReceiveResponse failed");
    }

    DWORD statusCode = 0;
    DWORD statusCodeSize = sizeof(statusCode);

    WinHttpQueryHeaders(
        request,
        WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER,
        WINHTTP_HEADER_NAME_BY_INDEX,
        &statusCode,
        &statusCodeSize,
        WINHTTP_NO_HEADER_INDEX
    );

    if (statusCode < 200 || statusCode >= 300) {
        WinHttpCloseHandle(request);
        WinHttpCloseHandle(connection);
        WinHttpCloseHandle(session);
        throw std::runtime_error("HTTP error code: " + std::to_string(statusCode));
    }

    std::string response;

    while (true) {
        DWORD bytesAvailable = 0;

        if (!WinHttpQueryDataAvailable(request, &bytesAvailable)) {
            break;
        }

        if (bytesAvailable == 0) {
            break;
        }

        std::vector<char> buffer(bytesAvailable + 1, 0);
        DWORD bytesRead = 0;

        if (!WinHttpReadData(
            request,
            buffer.data(),
            bytesAvailable,
            &bytesRead
        )) {
            break;
        }

        response.append(buffer.data(), bytesRead);
    }

    WinHttpCloseHandle(request);
    WinHttpCloseHandle(connection);
    WinHttpCloseHandle(session);

    return response;
}