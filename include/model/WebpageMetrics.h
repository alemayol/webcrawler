#pragma once

#include <string>
#include <vector>
#define WEBPAGEMETRICS_H

typedef struct WebpageMetrics {
  std::vector<std::string> foreignLinks;
  std::vector<std::string> sameDomainLinks;
  int nroImages;
} WebpageMetrics;
