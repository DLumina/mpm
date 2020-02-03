//
// Created by jihua on 2020/2/3.
//

#include "Net.h"
#include <iostream>
#include <cjson/cJSON.h>
#include <cstdlib>
using namespace std;
size_t onDataReceive(void *buffer, size_t size, size_t nmemb, void *user_p)
{
    FILE *fp = (FILE*)user_p;
    size_t receivedDataLen;
    receivedDataLen = fwrite(buffer, size, nmemb, fp);
    return receivedDataLen;
}

// 显示下载进度
int progress_callback(void *clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow)
{
    if (0 != dltotal) {
        std::cout << "download progress: " << 100 * dlnow / dltotal << "%" << std::endl;
    }
    return 0;
}

int download(const char* url,const char* filename)
{
    CURLcode code;
    CURL *easy_handle = curl_easy_init();
    void *clientp = NULL;
    FILE *fp;
    fopen_s(&fp, filename, "ab+");
    curl_easy_setopt(easy_handle, CURLOPT_NOPROGRESS, 0);

    curl_easy_setopt(easy_handle, CURLOPT_XFERINFOFUNCTION, progress_callback);

    curl_easy_setopt(easy_handle, CURLOPT_XFERINFODATA, clientp);

    curl_easy_setopt(easy_handle, CURLOPT_URL, url);
    curl_easy_setopt(easy_handle, CURLOPT_FOLLOWLOCATION , 2);
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYPEER, false); // if want to use https
    curl_easy_setopt(easy_handle, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false
    curl_easy_setopt(easy_handle, CURLOPT_WRITEFUNCTION, &onDataReceive);

    curl_easy_setopt(easy_handle, CURLOPT_WRITEDATA, fp);

    curl_easy_perform(easy_handle);

    curl_easy_cleanup(easy_handle);
    fclose(fp);

    return 0;
}
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp){
    char *d = (char*)buffer;
    string *b = (string*)(userp);
    int result = 0;
    if (b != NULL){
        b->append(d, size*nmemb);
        result = size*nmemb;
    }
    return result;
}

CURLcode curl_get_req(const std::string &url, std::string &response) {
    CURL *curl = curl_easy_init();
    CURLcode res;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION , 2);
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str()); // url
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false); // if want to use https
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false); // set peer and host verify false
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &response);
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3); // set transport and time out time
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
        res = curl_easy_perform(curl);
    }
    curl_easy_cleanup(curl);
    return res;
}

cJSON* get_version_file(const string& id,const string& version){
    auto url = "https://addons-ecs.forgesvc.net/api/v2/addon/"+id+"/files";
    string req;
    cJSON *ret = nullptr;
    curl_get_req(url,req);
    auto json = cJSON_Parse(req.c_str());
    auto max = cJSON_GetArraySize(json);
    for (int i = 0; i < max; ++i) {
        auto item = cJSON_GetArrayItem(json, i);
        auto versions = cJSON_GetObjectItem(item,"gameVersion");
        int versions_num = cJSON_GetArraySize(versions);
        for (int i = 0; i < versions_num; ++i) {
            if (string(cJSON_GetArrayItem(versions,i)->valuestring)==version){
                ret = item;
                break;
            }
        }
    }
    return ret;
}

void download_require(const cJSON* file,const char* version){
    auto require = cJSON_GetObjectItem(file,"dependencies");
    int require_num = cJSON_GetArraySize(require);
    for (int i = 0; i < require_num; ++i) {
        auto re = cJSON_GetArrayItem(require,i);
        if (cJSON_GetObjectItem(re,"type")->valueint==3){
            char* str = new char[25];
            itoa(cJSON_GetObjectItem(re,"addonId")->valueint,str,10);
            auto j = get_version_file(str,version);
            auto url = cJSON_GetObjectItem(j,"downloadUrl");
            auto name = cJSON_GetObjectItem(j,"fileName");
            download(url->valuestring,name->valuestring);
            download_require(j,version);
        }
    }
}

void download_all(const char* id,const char* version){
    auto json = get_version_file(id ,version);
    auto o = cJSON_GetObjectItem(json,"downloadUrl");
    download(o->valuestring,cJSON_GetObjectItem(json,"fileName")->valuestring);
    download_require(json,version);
}