cmake_minimum_required(VERSION 3.16)

project(SpreadRadar)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
)

FetchContent_MakeAvailable(json)

add_executable(SpreadRadar
    src/main.cpp

    src/utils/HttpClient.cpp

    src/exchange/BinanceClient.cpp
    src/exchange/CoinbaseClient.cpp
    src/exchange/KrakenClient.cpp

    src/core/SpreadCalculator.cpp
)

target_include_directories(SpreadRadar PRIVATE src)

target_link_libraries(SpreadRadar PRIVATE
    nlohmann_json::nlohmann_json
    winhttp
)

if (MSVC)
    target_compile_options(SpreadRadar PRIVATE /utf-8)
endif()