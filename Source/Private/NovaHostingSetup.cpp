#include "NovaMinimal.h"
#include "NovaCore.h"
#include "json.hpp"

using json = nlohmann::json;
using namespace Core;

int main(int argc, const char* argv[]) 
{
  RestClient::Response r = RestClient::get("http://2.58.113.177/api/message/de/errors.com.epicnova.test");

  json j = json::parse(r.body.c_str());
  NovaLog::Log(j.value("LanguageCode", "").c_str());

  return 0;
}
