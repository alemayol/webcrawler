#include "include/model/Crawler.h"
#include "include/model/HttpHandler.h"
#include "include/view/Visualizador.h"
#include <cstdlib>
#include <curl/curl.h>
#include <exception>
#include <iostream>

int main(int argc, char *argv[]) {

  Crawler *crawler = nullptr;
  Visualizador menu = Visualizador();
  int opcionMenu = -1;

  try {

    crawler = new Crawler(2);

    while (opcionMenu != 0) {
      menu.displayMenu();
      std::cin >> opcionMenu;

      if (std::cin.fail()) {
        std::cin.clear();
        menu.limpiarBuffer();
        opcionMenu = -1;
        std::cout << "Opción no válida. Por favor, ingrese un número."
                  << std::endl;
        ;
        continue;
      }

      menu.limpiarBuffer();

      switch (opcionMenu) {
      case 0: {
        break;
      }
      // Analizar nueva url
      case 1: {
        std::string web = menu.obtenerURL();
        int nivelProf = menu.aumentarNivelDeProfundidad();
        int maxPag;
        crawler->setProfundidad(nivelProf);
        crawler->analizarPagina(web);
        break;
      }
        // Cargar metricas desde archivo
      case 2: {
        std::cout << "Intentando cargar datos desde archivo..." << std::endl;
        break;
      }
        // Buscar palabra clave en URLs
      case 3: {

        std::string palabra = menu.buscarPalabraClave();
        std::vector<std::string> camino = crawler->buscarFrase(palabra);

        std::cout << "Camino vacio: " << camino.empty() << std::endl;

        if (camino.empty()) {
          std::cout
              << "No se logro encontrar la palabra clave en las URLs analizadas"
              << std::endl;
          break;
        }
        menu.printShortestPath(camino);
        break;
      }
      case 4: {
        menu.mostrarMetricas(crawler->getMetricas());
        break;
      }
      case 5: {
        std::cout << "Guardando datos analizados en memoria..." << std::endl;
        break;
      }
      default: {
        std::cout
            << "Por favor ingrese una opciona valida. Si desea salir ingrese 0"
            << std::endl;
        break;
      }
      }
    }
  } catch (std::exception &e) {
    std::cout << "Ha ocurrido una execepcion!" << std::endl;
    std::cout << e.what() << std::endl;
  }

  if (crawler != nullptr) {
    free(crawler);
    crawler = nullptr;
  }

  std::cout << "Finalizando programa..." << std::endl;

  return EXIT_SUCCESS;
}
