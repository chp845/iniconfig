#include "iniconfig.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DATA_SOURCE_ADD(T,S,D) \
    if (S && D){\
    T next = S;\
    while (next->next){next = next->next;}\
    next->next = D;}

static bool Is_number(char* value)
{
    char* pOver = value + strlen(value);
    while (value < pOver)
    {
        if (*value < '0' || *value > '9')
        {
            return false;
        }
        ++value;
    }
    return true;
}


struct _config_data
{
    CONFIG_DATA *next;
    char *key;
    char *value;
};

struct _config_section
{
    char *key;
    CONFIG_SECTION *next;
    CONFIG_DATA *data;
};

static inline CONFIG_SECTION* CreateNodeSource()
{
    CONFIG_SECTION *pSource = (CONFIG_SECTION*)malloc(sizeof(CONFIG_SECTION));
    memset(pSource, 0, sizeof(CONFIG_DATA));
    return pSource;
}

static inline CONFIG_DATA* CreateNodeData()
{
    CONFIG_DATA *pData = (CONFIG_DATA*)malloc(sizeof(CONFIG_DATA));
    memset(pData, 0, sizeof(CONFIG_DATA));
    return pData;
}

static void FreeConfigData(CONFIG_DATA *pData)
{
    CONFIG_DATA *pNext = pData;
    while (pData)
    {
        pNext = pData->next;
        free(pData->key);
        free(pData->value);
        free(pData);
        pData = pNext;
    }
}

void free_config(CONFIG_INI* config)
{
    CONFIG_SECTION *pSection = config;
    while (config)
    {
        pSection  = config->next;
        free(config->key);
        FreeConfigData(config->data);
        config = pSection ;
    }
}

static bool IsSkip(char* buffer, int len)
{
    char *pStart = buffer, *pEnd = buffer + len;
    bool bHasChar = false;
    if (buffer[0] == '\n' || buffer[0] == '#')
    {
        return true;
    }
    while (pStart < pEnd)
    {
        if (*pStart == '#' || *pStart == '\n')
        {
            break;
        }
        else if (*pStart != ' ')
        {
            bHasChar = true;
        }
        ++pStart;
    }
    // convert the char after '# to zero
    memset(pStart, 0, pEnd - pStart);
    return !bHasChar;
}

static bool IsLineOver(char* buffer, int len)
{
    return buffer[len - 1] == 0 || buffer[len - 1] == '\n';
}

static void Strip(char **start, char **end)
{
    // strip the head string with space
    char *pStart = *start, *pEnd = *end;
    while (pStart < pEnd)
    {
        if (*pStart != ' ')
        {
            break;
        }
        ++pStart;
    }

    // strip the trail string with space
    while (pEnd >= pStart)
    {
        if (*pEnd != ' ')
        {
            break;
        }
        --pEnd;
    }
    *start = pStart;
    *end = pEnd;
}

CONFIG_SECTION* ParseSource(char *pValue, int nLen)
{
    CONFIG_SECTION *pSource = CreateNodeSource();
    pSource->key = (char*)malloc(nLen + 1);
    memcpy(pSource->key, pValue, nLen);
    pSource->key[nLen] = 0;
    return pSource;
}

CONFIG_DATA* ParseData(char *pValue, int nVLen)
{
    char *pEnd = pValue + nVLen, *pStart = pValue;
    CONFIG_DATA *pData = CreateNodeData();
    int nLen;
    while (pStart < pEnd)
    {
        if (*pStart == '=')
        {
            break;
        }
        ++pStart;
    }
    if (*pStart == '=')
    {
        // get the key
        char* pTemp = pStart;
        --pStart;
        Strip(&pValue, &pStart);
        nLen = pStart - pValue + 1;
        pData->key = (char*)malloc(nLen + 1);
        memcpy(pData->key, pValue, nLen);
        pData->key[nLen] = 0;

        // get the value
        pStart = pTemp + 1;
        --pEnd;
        Strip(&pStart, &pEnd);
        nLen = pEnd - pStart + 1;
        pData->value = (char*)malloc(nLen + 1);
        memcpy(pData->value, pStart, nLen);
        pData->value[nLen] = 0;
    }
    else
    {
        // get the key
        --pEnd;
        Strip(&pStart, &pEnd);
        nLen = pEnd - pStart + 1;
        pData->key = (char*)malloc(nLen + 1);
        memcpy(pData->key, pStart, nLen);
        pData->key[nLen] = 0;
    }

    return pData;
}


CONFIG_INI* read_iniconfig(char *szFileName)
{
    CONFIG_SECTION *pSource = NULL;
    CONFIG_SECTION *pNode = NULL;
    char buffer[4098];
    FILE *in;
    char c;
    char *pStart, *pEnd;
    bool bIsParse = true;
    if (!szFileName)
    {
        return NULL;
    }
    // Read config data from file
    in = fopen(szFileName, "r");
    if (!in)
    {
        perror("OpenConfigSource,fopen()");
        return NULL;
    }
    while (fgets(buffer, sizeof(buffer), in))
    {
        if (IsSkip(buffer, sizeof(buffer)))
        {
            continue;
        }
        // sigle line only support 4098
        if (!IsLineOver(buffer, sizeof(buffer)))
        {
            do
            {
                c = getc(in);
            }while(c != EOF && c != '\n');
            continue;
        }

        // get the string not with space
        pStart = buffer;
        pEnd = pStart + strlen(buffer) - 1;
        Strip(&pStart, &pEnd);
        if (pStart == pEnd && *pStart == 0)
        {
            continue;
        }

        // parse the string
        if (*pStart == '[' && *pEnd == ']')
        {
            CONFIG_SECTION *pTemp = ParseSource(pStart + 1, pEnd - pStart -1);
            if (!pTemp)
            {
                bIsParse = false;
                break;
            }
            if (pSource)
            {
                DATA_SOURCE_ADD(CONFIG_SECTION*, pSource, pTemp);
            }
            if (!pSource)
            {
                pNode = pTemp;
            }
            pSource = pTemp;
        }
        else if (*pStart != '=')
        {
            CONFIG_DATA* pData = ParseData(pStart, pEnd - pStart + 1);
            if (!pData || !pSource)
            {
                continue;
            }
            if (pSource->data == NULL)
            {
                pSource->data = pData;
            }
            else
            {
                DATA_SOURCE_ADD(CONFIG_DATA*, pSource->data, pData);
            }
        }
    }

    fclose(in);
    if (!bIsParse)
    {
        free_config(pNode);
    }
    return pNode;
}


CONFIG_SECTION* get_config_section(CONFIG_INI* config, char* key)
{
    while ( config)
    {
        if (strcmp( config->key, key) == 0)
            return  config;
         config =  config->next;
    }
    return NULL;
}

CONFIG_DATA* get_config_key(CONFIG_SECTION* pSection, char* key)
{
    CONFIG_DATA *pData = pSection ? pSection->data : NULL;
    while (pData)
    {
        if (strcmp(pData->key,  key) == 0)
            return  pData;
        pData = pData->next;
    }
    return NULL;
}

char* get_config_value(CONFIG_DATA* data)
{
    return data ? data->value : NULL;
}

bool get_profilestring(CONFIG_INI* config,  char* section, char* key,  char** value)
{
    if (!config || !section || !key || !value )
    {
        return false;
    }
    CONFIG_SECTION* pSection = get_config_section(config, key);
    if (!pSection)
    {
        return false;
    }
    CONFIG_DATA* pData = get_config_key(pSection, key);
    if (!pData)
    {
        return false;
    }
    *value = pData->value;
    return true;
}

bool get_profileini(CONFIG_INI* config, char* section, char* key, int* value)
{
    if (!config || !section || !key || !value )
    {
        return false;
    }
    CONFIG_SECTION* pSection = get_config_section(config, key);
    if (!pSection)
    {
        return false;
    }
    CONFIG_DATA* pData = get_config_key(pSection, key);
    if (!pData)
    {
        return false;
    }
    if (Is_number(pData->value))
    {
        *value = atoi(pData->value);
        return true;
    }
    return false;
}
