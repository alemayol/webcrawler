#pragma once

#include "WebpageMetrics.h"
#include <unordered_set>
#define HTTPHANDLER_H
#include <deque>
#include <queue>

#include <curl/curl.h>
#include <string>

typedef struct {
  char *str;
  size_t size;
} Response;

class HttpHandler {
public:
  HttpHandler() noexcept(false);
  HttpHandler(std::string uri) noexcept(false);
  HttpHandler(std::string uri, std::string keyword) noexcept(false);
  ~HttpHandler();
  // Metodos
  std::string getRequest(std::string uri);
  static size_t write_chunk(void *data, size_t size, size_t nmemb,
                            void *userData);
  std::vector<std::string> findLinks(std::string html, std::string parentUrl,
                                     WebpageMetrics &metricas);
  bool compararDominios(std::string url1, std::string url2);
  std::string obtenerDominio(std::string foundUrl, std::string abaseUrl);
  static std::string limpiarLink(std::string currUrl, std::string baseUrl);
  static std::string completarUrl(std::string url);
  void printLinks();
  Response getResponse();

private:
  std::string uri;
  std::string keyword;
  Response response;
  CURLcode connection;
};
