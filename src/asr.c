#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include<sys/time.h>
#include "curl/curl.h"
#include "curl/easy.h"
#include "uuid/uuid.h"

#define MAX_BUFFER_SIZE 512

#define MAX_BODY_SIZE 1000000
int g_package_size = 32000;
struct timeval g_start, g_end;
static size_t writefunc(void *ptr, size_t size, size_t nmemb, char **result) {
    gettimeofday(&g_end, NULL);
    printf("recv-send=%ld ms\n", (g_end.tv_sec-g_start.tv_sec)*1000+(g_end.tv_usec-g_start.tv_usec)/1000);
    size_t result_len = size * nmemb;
    *result = (char *)realloc(*result, result_len + 1);
    if (*result == NULL) {        
        printf("realloc failure!\n");
        return 1;
    }    
    memcpy(*result, ptr, result_len);
    (*result)[result_len] = '\0';
    printf("%s\n", *result);
    return result_len;
}

static void sendReq(char *domain, char *appid, char *reqid, int seqid, char *property, char *audiodata, int content_len) {
    char host[MAX_BUFFER_SIZE];
    memset(host, 0, sizeof(host));
    snprintf(host, sizeof(host), "%s", "aiasr.jd.com/voice");
    char tmp[MAX_BUFFER_SIZE];
    memset(tmp, 0, sizeof(tmp));
    CURL *curl;
    CURLcode res;
    char *resultBuf = NULL;
    struct curl_slist *headerlist = NULL;
    snprintf(tmp, sizeof(tmp), "Domain:%s", domain);
    headerlist = curl_slist_append(headerlist, tmp);
    snprintf(tmp, sizeof(tmp), "Application-Id:%s", appid);
    headerlist = curl_slist_append(headerlist, tmp);
    snprintf(tmp, sizeof(tmp), "Request-Id:%s", reqid);
    headerlist = curl_slist_append(headerlist, tmp);
    snprintf(tmp, sizeof(tmp), "Sequence-Id:%d", seqid);
    headerlist = curl_slist_append(headerlist, tmp);
    snprintf(tmp, sizeof(tmp), "Asr-Protocol:%d", 0);
    headerlist = curl_slist_append(headerlist, tmp);
    snprintf(tmp, sizeof(tmp), "Net-State:%d", 2);
    headerlist = curl_slist_append(headerlist, tmp);
    snprintf(tmp, sizeof(tmp), "Applicator:%d", 0);

    headerlist = curl_slist_append(headerlist, tmp);
    if (seqid==-1||seqid==1) {        
        snprintf(tmp, sizeof(tmp), "Property:%s", property);
        headerlist = curl_slist_append(headerlist, tmp);
    }
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, host);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3000);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, audiodata);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, content_len);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resultBuf);
    gettimeofday(&g_start, NULL);
    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {        
        printf("perform curl error:%d.\n", res);
        return ;
    }

    curl_slist_free_all(headerlist);
    curl_easy_cleanup(curl);
}

int main (int argc,char* argv[]) {
    if (argc != 4) {
        printf("Usage: %s domain package_size audio_file\n", argv[0]);
        return -1;
    }
    FILE *fp = NULL;
    fp = fopen(argv[3], "r");
    if (NULL == fp) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    int content_len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *audiodata = (char *)malloc(content_len);
    fread(audiodata, content_len, sizeof(char), fp);
    char *domain = argv[1];
    char reqid[64];
    uuid_t uuid;
    uuid_generate(uuid);
    uuid_unparse(uuid, reqid);

    char tmp[MAX_BUFFER_SIZE];
    memset(tmp, 0, sizeof(tmp));
    snprintf(tmp, sizeof(tmp), "%s", "{\"autoend\":false,\"encode\":{\"channel\":1,\"format\":\"wav\",\"sample_rate\":16000},\"platform\":\"Linux\",\"version\":\"0.0.0.1\"}");

    g_package_size = atoi(argv[2]);
    if (g_package_size <= 0) {
        printf("package_size <= 0!\n");
        return 0;
    }

    char *package_data = (char *)malloc(g_package_size);
    int package_len = g_package_size;
    int seqid = 1;

    for (int pos=0; pos<content_len; pos+=g_package_size,++seqid) {
        package_len = g_package_size<content_len-pos? g_package_size:content_len-pos;
        usleep(1000*package_len/32);
        seqid = g_package_size<content_len-pos?seqid:-seqid;
        memcpy(package_data, audiodata+pos, package_len);
        sendReq(domain, "linux_demo", reqid, seqid, tmp, package_data, package_len);
    }

    fclose(fp);
    free(audiodata);
    free(package_data);
    return 0;
}
