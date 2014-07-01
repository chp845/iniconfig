/*
 * =====================================================================================
 *
 *       Filename:  iniconfig.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年06月30日 16时48分21秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  cheng (), chp845@hotmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
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

bool get_profileint(CONFIG_INI* config, char* section, char* key, int* value);

#endif // _INI_CONFIG_H
