typedef struct Information{    
    double latitude;
    double longitude;
    double temperature;
    double temperature_max;
    double temperature_min;
    double humidity;
} Infos;

void save_info(char * data);