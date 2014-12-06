#ifndef CONTEXT_HPP
#define CONTEXT_HPP 

#include <map>
#include <string>

#include "websocketpp/config/asio_no_tls.hpp"
#include "websocketpp/server.hpp"

#include "Room.hpp"

namespace chat {

struct Context {

  typedef websocketpp::server<websocketpp::config::asio> ServerType;
  ServerType server;

  typedef std::map<std::string, Room> RoomsType;
  RoomsType rooms;

  

};





}



#endif /* CONTEXT_HPP */
