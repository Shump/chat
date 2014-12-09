#ifndef ROOM_HPP
#define ROOM_HPP 

#include <map>
#include <string>

#include "websocketpp/server.hpp"

namespace chat {

struct UserData {
  UserData(const std::string& name): 
    name(name) {};
  std::string name;
};


//struct Room {
//
//  std::map<websocketpp::connection_hdl, UserData, 
//    std::owner_less<websocketpp::connection_hdl>> users;
//
//
//};

typedef std::map<websocketpp::connection_hdl, UserData, std::owner_less<websocketpp::connection_hdl>> Room;

}




#endif /* ROOM_HPP */
