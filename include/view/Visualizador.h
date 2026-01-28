#pragma once
#define VISUALIZADOR_H
#include "../model/WebpageMetrics.h"

#include <string>
#include <vector>
class Visualizador {
public:
  Visualizador();
  void printShortestPath(std::vector<std::string> urls);
  void displayMenu();
  std::string obtenerURL();
  int aumentarNivelDeProfundidad();
  int aumentarPaginasMaximas();
  void mostrarMetricas(WebpageMetrics metricas);
  std::string buscarPalabraClave();
  void limpiarBuffer();
};
