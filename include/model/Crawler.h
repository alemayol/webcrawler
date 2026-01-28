#pragma once
#define CRAWLER_H

#include "WebpageMetrics.h"
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <libxml2/libxml/HTMLparser.h>

class Crawler {
public:
  Crawler(int depth);
  Crawler(std::string startUrl, int depth);
  void analizarPagina(std::string url);
  std::vector<std::string> buscarFrase(std::string palabra);
  int getProfundidad();
  int getMaxPaginas();
  void setMaxPaginas(int p);
  void
  setAllFoundLinks(std::unordered_map<std::string, std::string> allFoundLinks);
  std::unordered_map<std::string, std::string> getGrafo();
  void setProfundidad(int n);
  void printLinks();
  void setMetricas(WebpageMetrics metricas);
  WebpageMetrics getMetricas();

private:
  WebpageMetrics metricas; // Objeto para guardar todas las metricas encontradas
                           // durante analisis
  int nivelProfundidad;    // Sitios a visitar a partir de url inicial
  int maxPag;              // Maximo de paginas a indexar
  std::unordered_set<std::string>
      linksVisitados; // Links ya visitados, para no hacer peticiones en
                      // circulos
  std::unordered_map<std::string, std::string>
      allFoundLinks; // Llave = url, Valor: url padre (de donde proviene, para
                     // poder reconstruir el camino a tomar)
  std::queue<std::string, std::deque<std::string>> foundLinks;
};
