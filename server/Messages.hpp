#ifndef MESSAGES_HPP
#define MESSAGES_HPP 

#include <map>
#include <functional>
#include <string>

#include <iostream>

#include "Context.hpp"

#include "picojson.h"

namespace chat {

  namespace priv {
    template<typename K, typename V>
    bool key_exists(std::map<K, V> m, K k) {
      auto search = m.find(k);
      return search != m.end() ? true : false;
    };
  }


struct TestMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::cout << "TestMessageHandler called!" << std::endl;
  };
};

struct RegisterMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::cout << "Register called!" << std::endl;
  };
};

struct TextMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::cout << "Text message recieved!" << std::endl;
  };
};

struct UsersMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::cout << "Users command recieved!" << std::endl;
  };
};

struct RoomsMessageHandler {
  void operator()(Context& ctx, websocketpp::connection_hdl hdl, const picojson::object& obj) {
    std::cout << "Rooms command recieved!" << std::endl;

    for( auto room_pair : ctx.rooms) {
      std::cout << room_pair.first << std::endl;
    };

  };
};


class MessageManager {

public:

  MessageManager() {

    handlers["test"] = TestMessageHandler();
    handlers["register"] = RegisterMessageHandler();
    handlers["text"] = TextMessageHandler();
    handlers["users"] = UsersMessageHandler();
    handlers["rooms"] = RoomsMessageHandler();

  };

  void process_message(Context& ctx, websocketpp::connection_hdl con_hdl, const picojson::object& obj) {

    if(priv::key_exists(obj, std::string("type")) &&
        obj.at("type").is<std::string>()) {

      std::string type = obj.at("type").get<std::string>();

      if(priv::key_exists(handlers, type)) {
        auto hdl = handlers.at(type);
        hdl(ctx, con_hdl, obj);
      } else {
        std::cerr << "Error: no handler for type: " + type << std::endl;
      };

    } else {

      std::cout << "No type member!" << std::endl;

    };

  };

private:

  typedef std::map<std::string, std::function<void(Context&, websocketpp::connection_hdl, const picojson::object&)>> HandlerMap;
  HandlerMap handlers;


};


}


#endif /* MESSAGES_HPP */
