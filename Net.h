//
// Created by jihua on 2020/2/3.
//

#ifndef MPM_NET_H
#define MPM_NET_H

#include <string>
#include <curl/curl.h>
#include <cjson/cJSON.h>
using namespace std;
CURLcode curl_get_req(const std::string &url, std::string &response);
cJSON* get_version_file(const string& id,const string& version);
int download(const char* url, const char* path);
void download_require(const cJSON* file,const char* version);
void download_all(const char* id,const char* version);
#endif //MPM_NET_H
