
cmake_minimum_required(VERSION 3.5)
project(WebCrawler CXX)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Para autocompletado del lsp de neovim
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_library(PSL_LIB psl)
find_package(LibXml2 QUIET REQUIRED)
find_package(CURL QUIET REQUIRED)

add_executable(${PROJECT_NAME} ./main.cpp ./src/model/HttpHandler.cpp ./src/model/Crawler.cpp ./src/model/WebpageMetrics.cpp ./src/view/Visualizador.cpp)

# Linking libraries
target_link_libraries(${PROJECT_NAME} LibXml2::LibXml2 CURL::libcurl ${PSL_LIB})
