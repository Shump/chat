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
  void operator()(Context& ctx, const picojson::object& obj) {
    std::cout << "TestMessageHandler called!" << std::endl;
  };
};

struct RegisterMessageHandler {
  void operator()(Context& ctx, const picojson::object& obj) {
    std::cout << "Register called!" << std::endl;
  };
};


class MessageManager {

public:

  MessageManager() {

    handlers["test"] = TestMessageHandler();
    handlers["register"] = RegisterMessageHandler();

  };

  void process_message(Context& ctx, const picojson::object& obj) {

    if(priv::key_exists(obj, std::string("type")) &&
        obj.at("type").is<std::string>()) {

      std::string type = obj.at("type").get<std::string>();

      if(priv::key_exists(handlers, type)) {
        auto hdl = handlers.at(type);
        hdl(ctx, obj);
      } else {
        std::cerr << "Error: no handler for type: " + type << std::endl;
      };

    } else {

      std::cout << "No type member!" << std::endl;

    };

  };

private:

  typedef std::map<std::string, std::function<void(Context&, const picojson::object&)>> HandlerMap;
  HandlerMap handlers;


};


}


#endif /* MESSAGES_HPP */
