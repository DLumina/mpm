#include <iostream>
#include <curl/curl.h>
#include "Net.h"

int main(int argc,char** argv) {
    curl_global_init(CURL_GLOBAL_ALL);
    if (argc==1){
        cout<<"example: mpm 74072 1.12.2";
    } else{
        const char* version = argc>2?argv[2]:"1.12.2";
        download_all(argv[1],version);
    }
    curl_global_cleanup();
    return 0;
}
