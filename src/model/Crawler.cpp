#include "../../include/model/Crawler.h"
#include "../../include/model/FailedConnectionException.h"
#include "../../include/model/HttpHandler.h"
#include <algorithm>
#include <exception>
#include <iostream>
#include <queue>
#include <unordered_map>

Crawler::Crawler(int depth) {
  if (depth > 50) {
    this->nivelProfundidad = 2;

  } else {
    this->nivelProfundidad = depth;
  }
}

Crawler::Crawler(std::string url, int depth) {
  if (depth > 50) {
    this->nivelProfundidad = 2;

  } else {
    this->nivelProfundidad = depth;
  }
}

void Crawler::analizarPagina(std::string url) {
  HttpHandler http = HttpHandler();
  int nivelActual = 0;

  // Limpiando datos anteriores
  this->linksVisitados.clear();
  this->allFoundLinks.clear();

  this->foundLinks.push(url);
  // Nodo cabeza en el map no ordenado
  this->allFoundLinks.insert({url, "none"});

  while (nivelActual <= this->nivelProfundidad && !this->foundLinks.empty()) {

    std::cout << "Nivel actual: " << nivelActual << std::endl;
    // std::cout << "Nivel de profundidad: " << this->nivelProfundidad
    //          << std::endl;
    // Obtenemos el frente de la cola y lo quitamos de ella
    std::string currentURL = this->foundLinks.front();
    this->foundLinks.pop();

    std::cout << "🕷️ Transversando links, por favor espere..." << std::endl;
    std::cout << "Actual: " << currentURL << std::endl;

    // unordered_set.find retorna un iterador al elemento si es encontrado, de
    // lo contrario retorna el iterador end(), por lo tanto, si es != end(),
    // significa que ya lo visitamos
    // if (this->linksVisitados.find(currentURL) != this->linksVisitados.end())
    // {
    // std::cout << "Ya visitado: " << currentURL << std::endl;
    // continue;
    // }

    this->linksVisitados.insert(currentURL);
    // Visitamos la url actual
    try {
      std::string res = http.getRequest(currentURL);

      std::vector<std::string> links =
          http.findLinks(res, currentURL, this->metricas);

      std::cout << "Links encontrados: " << links.size() << std::endl;

      for (std::string &link : links) {

        std::cout << "🕷️ Link actual: " << link << std::endl;

        if (this->linksVisitados.find(link) == this->linksVisitados.end()) {

          std::cout << "🕷️ Link actual aceptado: " << link << std::endl;
          this->linksVisitados.insert(link);
          this->foundLinks.push(link);
          this->metricas.sameDomainLinks++;

          // Almacenamos todos los links unicos encontrados para metricas
          if (this->allFoundLinks.find(link) == this->allFoundLinks.end())
            this->allFoundLinks.insert({link, currentURL});
        }
      }

    } catch (FailedConnectionException &e) {
      std::cout << "Failed to reach website" << std::endl;
      continue;
    } catch (std::exception &e) {
      std::cout << e.what() << std::endl;
    }

    nivelActual++;
  }

  // Vaciando cola en caso de que hubiesen links por visitar antes de cumplir
  // con una condicion
  std::queue<std::string, std::deque<std::string>> colaVacia;
  std::swap(this->foundLinks, colaVacia);
}

std::vector<std::string> Crawler::buscarFrase(std::string palabra) {

  std::vector<std::string> camino;
  std::string urlMatch;
  bool palabraEncontrada = false;

  for (auto i = this->allFoundLinks.begin(); i != this->allFoundLinks.end();
       i++) {

    if (i->first.find(palabra) != std::string::npos) {
      std::cout << "✨ Keyword '" << palabra << "' found!" << std::endl;
      urlMatch = i->first;
      palabraEncontrada = true;
      break;
    }
  }

  if (!palabraEncontrada)
    return camino;

  // Buscamos hasta el par cuya url padre sea none, ya que establecimos esa
  // palabra como señal
  while (urlMatch != "none") {
    camino.push_back(urlMatch);
    urlMatch = this->allFoundLinks[urlMatch];
  }

  // Revertimos el vector ya que estan desde el ultimo hasta la cabeza, y
  // queremos el orden inverso para imprimirlo
  std::reverse(camino.begin(), camino.end());

  return camino;
}

void Crawler::setAllFoundLinks(
    std::unordered_map<std::string, std::string> allFoundLinks) {
  this->allFoundLinks = allFoundLinks;
}

void Crawler::setProfundidad(int n) {
  if (n > 20) {

    std::cout << "Nivel de profundidad maxima excedido. Nivel establecido: 10"
              << std::endl;
    this->nivelProfundidad = 20;
  } else {
    this->nivelProfundidad = n;
  }
}

int Crawler::getProfundidad() { return this->nivelProfundidad; }

int Crawler::getMaxPaginas() { return this->maxPag; }

void Crawler::setMaxPaginas(int p) {
  if (p > 150) {
    this->maxPag = 100;
  } else {
    this->maxPag = p;
  }
}

std::unordered_map<std::string, std::string> Crawler::getGrafo() {
  return this->allFoundLinks;
}

void Crawler::setMetricas(WebpageMetrics metrics) { this->metricas = metrics; }

WebpageMetrics Crawler::getMetricas() { return this->metricas; }

void Crawler::printLinks() {

  std::cout << "Numero imagenes: " << this->metricas.nroImages << std::endl;
  std::cout << "Numero de links foraneos: " << this->metricas.foreignLinks
            << std::endl;

  for (auto i = this->allFoundLinks.begin(); i != this->allFoundLinks.end();
       i++) {
    std::cout << "Url: " << i->first << " Parent Url: " << i->second
              << std::endl;
  }
}
