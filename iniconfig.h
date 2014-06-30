#ifndef _INI_CONFIG_H
#define _INI_CONFIG_H
#include <stdbool.h>

typedef struct _config_data CONFIG_DATA;
typedef struct _config_section CONFIG_SECTION, CONFIG_INI;

CONFIG_INI* read_iniconfig(char* filename);

CONFIG_SECTION* get_config_section(CONFIG_INI* config, char* key);

char* get_config_value(CONFIG_DATA* data);

CONFIG_DATA* get_config_key(CONFIG_SECTION* data, char* key);

void free_config(CONFIG_INI* config);

bool get_profilestring(CONFIG_INI* config,  char* section, char* key,  char** value);

bool get_profileini(CONFIG_INI* config, char* section, char* key, int* value);

#endif // _INI_CONFIG_H
