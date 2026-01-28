#include "../../include/data/GestorJSON.h"
#include "../../include/model/Crawler.h"
#include "../../include/model/WebpageMetrics.h"
#include <fstream>
#include <nlohmann/json.hpp>

bool GestorJSON::guardarArchivo(std::string nombreA, Crawler *crawler) {
  nlohmann::json J;

  J["nivel_prof"] = crawler->getProfundidad();
  J["max_pags"] = crawler->getMaxPaginas();
  WebpageMetrics metricas = crawler->getMetricas();
  J["metricas"] = {{"total_img", metricas.nroImages},
                   {"pag_foraneas", metricas.foreignLinks},
                   {"pag_analizadas", metricas.sameDomainLinks}};

  J["graph"] = crawler->getGrafo();

  std::ofstream file(nombreA);

  if (file.is_open()) {
    file << J.dump(4); // El 4 es para identacion en el json
    return true;
  }

  return false;
}

bool GestorJSON::cargarArchivo(std::string nombreA, Crawler *crawler) {
  std::ifstream file(nombreA);

  if (!file.is_open())
    return false;

  nlohmann::json j;

  file >> j;

  crawler->setProfundidad(j.at("nivel_prof"));
  crawler->setMaxPaginas(j.at("max_pags"));

  auto m = j.at("metricas");
  crawler->setMetricas(
      {m.at("total_img"), m.at("pag_foraneas"), m.at("pag_analizadas")});

  crawler->setAllFoundLinks(
      j.at("graph").get<std::unordered_map<std::string, std::string>>());
  return true;
}
