#include "../../include/model/HttpHandler.h"
#include "../../include/model/FailedConnectionException.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/urlapi.h>
#include <iostream>
#include <libpsl.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <libxml2/libxml/HTMLparser.h>
#include <libxml2/libxml/xpath.h>

// Constructores
HttpHandler::HttpHandler() { curl_global_init(CURL_GLOBAL_ALL); };

HttpHandler::HttpHandler(std::string uri) { this->uri = uri; };

HttpHandler::HttpHandler(std::string uri, std::string keyword) {
  curl_global_init(CURL_GLOBAL_ALL);
  this->uri = uri;
  this->keyword = keyword;
};

// Destructor
HttpHandler::~HttpHandler() { curl_global_cleanup(); }

// Metodos

// data = pointer to block of data received in the current chunk
// nmemb = number of bytes in that block of data
// size = size of elements received (nmemb is the quantity)
// it returns the number of bytes in the chunk
size_t HttpHandler::write_chunk(void *data, size_t size, size_t nmemb,
                                void *userdata) {

  size_t real_size = size * nmemb;

  Response *response = (Response *)userdata;

  // size of existing block of memory + size of current chunk of data + 1 to
  // account for the null terminator
  char *ptr = (char *)realloc(response->str, response->size + real_size + 1);

  if (ptr == NULL) {
    std::fprintf(stderr, "Unable to reallocate memory");
    // return 0;
    return CURL_WRITEFUNC_ERROR;
  }

  response->str = ptr;

  // Concat strings
  memcpy(&(response->str[response->size]), data, real_size);
  response->size += real_size;

  // Ultimo caracter de la cadena sera null terminator para completar la cadena
  response->str[response->size] = 0; // \0

  return real_size;
};

std::string HttpHandler::getRequest(std::string uri) {

  CURL *curl = curl_easy_init();
  Response response;
  response.str = nullptr;
  response.size = 0;

  curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &HttpHandler::write_chunk);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
  curl_easy_setopt(
      curl, CURLOPT_USERAGENT,
      "Mozilla/5.0 (X11; Linux x86_64; rv:146.0) Gecko/20100101 Firefox/146.0");

  this->connection = curl_easy_perform(curl);

  if (this->connection != CURLE_OK) {
    curl_easy_cleanup(curl);
    std::string error = "No se pudo establecer una conexion con la URL "
                        "proporcionada. Por favor ingrese una URL valida: ";

    throw FailedConnectionException(
        error.append(curl_easy_strerror(this->connection)));
  }

  // Finalizar la sesion de curl
  curl_easy_cleanup(curl);

  std::string res;

  if (response.str) {
    res = std::string(response.str, response.size);
    free(response.str);
    response.str = nullptr;
  }

  return res;
}

std::vector<std::string> HttpHandler::findLinks(std::string html,
                                                std::string currentUrl,
                                                WebpageMetrics &metricas) {

  std::vector<std::string> foundLinks;
  std::string targetDomain = this->obtenerDominio(currentUrl, "");

  // htmlReadMemory() parses the HTML string and builds a tree on which you can
  // apply XPath selectors.
  htmlDocPtr doc = htmlReadMemory(html.c_str(), html.length(), nullptr, nullptr,
                                  HTML_PARSE_NOERROR);

  xmlXPathContextPtr context = xmlXPathNewContext(
      doc); // Establecemos el contexto del xml como todo el documento html
  xmlXPathObjectPtr a_tags = xmlXPathEvalExpression(
      (xmlChar *)"//a/@href",
      context); // Extraemos todos los nodos que sean un
                // link y contengan el atrituto href (etiqueta <a /> de html)
  xmlXPathObjectPtr img_tags = xmlXPathEvalExpression(
      (xmlChar *)"//img", context); // Extraemos todos los nodos que sean un
                                    // link (etiqueta <a /> de html)

  if (a_tags && a_tags->nodesetval) {
    // Mientras i sea menor al numero de nodos (etiquetas) encontradas en el
    // html

    for (int i = 0; i < a_tags->nodesetval->nodeNr; i++) {
      xmlChar *href = xmlNodeListGetString(
          doc, a_tags->nodesetval->nodeTab[i]->children, 1);

      if (href) {
        std::string linkStr = (char *)
            href; // Recasteamos el puntero xmlChar a un puntero char* (string)

        // "Normalizamos" el link antes de analizarlo
        linkStr = this->limpiarLink(linkStr, currentUrl);

        // Si tenemos un link vacio, procedemos con la siguiente iteracion
        if (linkStr.empty()) {
          continue;
        }

        // Si el link proviene del mismo dominio, lo agregamos a la cola de
        // links a crawlear
        if (this->obtenerDominio(linkStr, currentUrl) == targetDomain) {
          foundLinks.push_back(linkStr);
        } else {
          // De lo contrario es un link que lleva a otro dominio, lo sumamos a
          // las metricas
          metricas.foreignLinks++;
        }

        xmlFree(href);
      }
    }
  }

  // La cantidad de nodos de etiquetas img encontrados es la cantidad de
  // imagenes que tiene la pagina actual
  metricas.nroImages += img_tags->nodesetval->nodeNr;

  xmlXPathFreeObject(a_tags);
  xmlXPathFreeObject(img_tags);
  xmlFreeDoc(doc);

  return foundLinks;
}

std::string HttpHandler::completarUrl(std::string url) {
  // Eliminamos salots de linea o espacios en blanco
  url.erase(0, url.find_first_not_of(" \t\n\r"));
  url.erase(url.find_last_not_of(" \t\n\r") + 1);

  if (url.empty())
    return "";

  if (url.find("://") == std::string::npos) {
    url = "https://" + url;
  }

  return url;
}

std::string HttpHandler::limpiarLink(std::string currUrl, std::string baseUrl) {

  if (baseUrl.empty())
    currUrl = completarUrl(currUrl);

  CURLU *str1 = curl_url();
  char *linkLimpio = nullptr;
  char *urlPart = nullptr;
  std::string res = "";

  // Asignamos la url base primero, luego haremos lo mismo con la url encontrada
  // en el atributo href de un link, de esta forma podemos manejar links
  // relativos en un pagina
  if (!baseUrl.empty())
    curl_url_set(str1, CURLUPART_URL, baseUrl.c_str(), 0);

  // Parsing URLs
  if (curl_url_set(str1, CURLUPART_URL, currUrl.c_str(), 0) == CURLUE_OK) {

    // En caso de ser ingresado un protocolo en mayusculas, lo minimizaremos
    if (curl_url_get(str1, CURLUPART_SCHEME, &urlPart, 0) == CURLUE_OK) {
      std::string protocolo = urlPart;
      std::transform(protocolo.begin(), protocolo.end(), protocolo.begin(),
                     ::tolower);
      // curl_url_set(str1, CURLUPART_SCHEME, protocolo.c_str(), 0);
      curl_url_set(str1, CURLUPART_SCHEME, "https", 0);
      curl_free(urlPart);
      urlPart = nullptr;
    }

    // Tambien minimizamos el domain eje: SCRAPINGCOURSE.COM ->
    // scrapingcourse.com
    if (curl_url_get(str1, CURLUPART_HOST, &urlPart, 0) == CURLUE_OK) {
      std::string host = urlPart;
      std::transform(host.begin(), host.end(), host.begin(), ::tolower);
      curl_url_set(str1, CURLUPART_HOST, host.c_str(), 0);
      curl_free(urlPart);
      urlPart = nullptr;
    }

    // Eliminamos los # usados en secciones internas de un html
    curl_url_set(str1, CURLUPART_FRAGMENT, nullptr, 0);

    if (curl_url_get(str1, CURLUPART_URL, &linkLimpio, 0) == CURLUE_OK) {
      res = linkLimpio;
      curl_free(linkLimpio);
    }
  }

  curl_url_cleanup(str1);

  if (!res.empty() && res.back() == '/') {
    // Buscamos la posicion donde termina el protocolo eje: https://
    size_t protocolo = res.find("://");
    // Eliminamos el ultimo / asegurandonos que no sea el ultimo del protocolo
    // eje: https:// -> https:/
    if (protocolo != std::string::npos && res.length() > protocolo + 3)
      res.pop_back();
  }

  return res;
}

std::string HttpHandler::obtenerDominio(std::string foundUrl,
                                        std::string baseUrl) {
  CURLU *str1 = curl_url();
  std::string dom = "";

  // Parsing URLs
  if (curl_url_set(str1, CURLUPART_URL, foundUrl.c_str(), 0) == CURLUE_OK) {
    // Extraemos el host del link y convertimos todo a minuscula en el proceso
    char *host = nullptr;
    curl_url_get(str1, CURLUPART_HOST, &host, 0);

    // Si logramos extraer el host, comparamos si son iguales. De serlo,
    // provienen del mismo dominio
    if (host) {
      // Usamos libpsl para encontrar el dominio real, es decir, manejaremos el
      // caso donde existan subdominios
      const psl_ctx_t *psl = psl_builtin();
      const char *raiz = psl_registrable_domain(psl, host);

      // Si psl logra encontrar el dominio real, asignamos el valor a dom. De lo
      // contrario devolvemos el host antes encontrado
      if (raiz) {
        dom = raiz;
      } else {
        dom = host;
      }

      curl_free(host);
    }
  }

  // Limpiamos memoria utilizando las funciones de curl
  curl_url_cleanup(str1);
  return dom;
}

bool HttpHandler::compararDominios(std::string url1, std::string url2) {
  std::string dom1 = this->obtenerDominio(url1, "");
  std::string dom2 = this->obtenerDominio(url2, "");

  return !dom1.empty() && dom1 == dom2;
  // return dom1 == dom2;
}
