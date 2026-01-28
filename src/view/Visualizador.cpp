#include "../../include/view/Visualizador.h"
#include "../../include/model/HttpHandler.h"
#include <ios>
#include <iostream>
#include <limits>
#include <vector>

Visualizador::Visualizador() { this->opcionMenu = 0; }

void Visualizador::limpiarBuffer() {

  std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                  '\n'); // Limpiando buffer
}

void Visualizador::displayMenu() {
  std::cout << "\n--- Web Crawler ---" << std::endl;
  std::cout << "1. Analizar un nuevo sitio web" << std::endl;
  std::cout << "2. Cargar análisis desde archivo" << std::endl;
  std::cout << "3. Buscar ruta a palabra clave" << std::endl;
  std::cout << "4. Mostrar métricas del sitio" << std::endl;
  std::cout << "5. Guardar análisis en archivo" << std::endl;
  std::cout << "0. Salir" << std::endl;
  std::cout << "Seleccione una opción: ";
}

int Visualizador::aumentarNivelDeProfundidad() {
  int nivel = 0;

  std::cout << "Ingrese un nuevo nivel de profundidad: ";
  std::cin >> nivel;
  this->limpiarBuffer();
  return nivel;
}

int Visualizador::aumentarPaginasMaximas() {
  int pags = 0;

  std::cout
      << "Ingrese el numero maximo de paginas a visitar (no mas de 300): ";
  std::cin >> pags;
  this->limpiarBuffer();
  return pags;
}

std::string Visualizador::obtenerURL() {

  std::string web;
  std::cout << "Ingrese la URL (https://google.com): ";
  std::cin >> web;

  return HttpHandler::limpiarLink(web, "");
}

std::string Visualizador::buscarPalabraClave() {
  std::string palabra;
  std::cout << "Ingrese la palabra clave a buscar en la URL: ";
  std::cin >> palabra;
  return palabra;
}

void Visualizador::mostrarMetricas(WebpageMetrics metricas) {
  std::cout << "-------- Metricas del sitio web --------" << std::endl;

  std::cout << "Paginas foraneas: " << metricas.foreignLinks << std::endl;
  std::cout << "Paginas analizadas: " << metricas.sameDomainLinks << std::endl;
  std::cout << "Imagenes totales en paginas analizadas: " << metricas.nroImages
            << std::endl;
}

void Visualizador::printShortestPath(std::vector<std::string> urls) {

  unsigned int depth = 0;

  std::cout << "\n\n🏁 Shortest Path (" << urls.size() - 1
            << " links desde la url indicada):" << std::endl;

  for (std::string url : urls) {
    // Print indentation
    for (int i = 0; i < depth; ++i) {
      if (i == depth - 1) {
        std::cout << "|-- ";
      } else {
        std::cout << "|   ";
      }
    }

    if (depth == 0) {
      std::cout << url << std::endl;
    } else {
      std::cout << url << std::endl;
    }
    depth++;
  }
}
