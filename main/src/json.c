#include <stdio.h>
#include "../inc/json.h"
#include "cJSON.h"

extern Infos info;

void save_info(char * data){   
    printf("Original data: %s", data);
    cJSON * json = cJSON_Parse(data); // Parse to json to access
    
    if (data[2] == 'c'){ //coordinates came from Weather Map Infos (third char)
        cJSON * obj_json = cJSON_GetObjectItem(json, "main"); // Getting object with informations
        
        info.humidity = cJSON_GetObjectItemCaseSensitive(obj_json, "humidity")->valuedouble;
        info.temperature = cJSON_GetObjectItemCaseSensitive(obj_json, "temp")->valuedouble;
        info.temperature_max = cJSON_GetObjectItemCaseSensitive(obj_json, "temp_max")->valuedouble;
        info.temperature_min = cJSON_GetObjectItemCaseSensitive(obj_json, "temp_min")->valuedouble;
    }else{
        if (data[2] == 'l'){ //location came from Stack Infos
            info.longitude = cJSON_GetObjectItemCaseSensitive(json, "longitude")->valuedouble;
            info.latitude = cJSON_GetObjectItemCaseSensitive(json, "latitude")->valuedouble;
        }
    } 
     
    cJSON_Delete(json);
}