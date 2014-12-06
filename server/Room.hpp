#ifndef ROOM_HPP
#define ROOM_HPP 

#include <list>
#include <string>

#include "websocketpp/server.hpp"

namespace chat {

struct User {
  std::string name;
  websocketpp::connection_hdl connection;
};


struct Room {

  std::list<User> users;


};


}




#endif /* ROOM_HPP */
