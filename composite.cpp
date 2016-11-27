#include <verbly.h>
#include <yaml-cpp/yaml.h>
#include <twitter.h>
#include <chrono>
#include <string>
#include <thread>
#include <iostream>

int main(int argc, char** argv)
{
  YAML::Node config = YAML::LoadFile("config.yml");
    
  twitter::auth auth;
  auth.setConsumerKey(config["consumer_key"].as<std::string>());
  auth.setConsumerSecret(config["consumer_secret"].as<std::string>());
  auth.setAccessKey(config["access_key"].as<std::string>());
  auth.setAccessSecret(config["access_secret"].as<std::string>());
  
  twitter::client client(auth);
  verbly::data database("data.sqlite3");
    
  for (;;)
  {
    std::cout << "Generating..." << std::endl;
    auto nq = database.nouns().with_stress({{true, false, false}}).random().limit(1).run().front();
    std::string noun = nq.singular_form();
    noun[0] = std::toupper(noun[0]);
    
    std::string output = "Full Metal " + noun;
    std::cout << output << std::endl;
    
    try
    {
      client.updateStatus(output);
    } catch (const twitter::twitter_error& e)
    {
      std::cout << "Twitter error: " << e.what() << std::endl;
    }
    
    std::cout << "Waiting..." << std::endl;
    std::this_thread::sleep_for(std::chrono::hours(1));
  }
}
