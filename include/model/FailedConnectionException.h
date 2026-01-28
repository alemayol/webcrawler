#pragma once

#include <exception>
#include <string>
#define FAILEDCONNECTIONEXCEPTION_H

class FailedConnectionException : public std::exception {
public:
  FailedConnectionException() {
    this->message = "Unable to initilize connection with curl";
  };
  FailedConnectionException(std::string message) { this->message = message; };

  virtual const char *what() const throw() { return this->message.c_str(); }

private:
  std::string message;
};
