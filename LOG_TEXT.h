#ifndef _LOG_TEXT_H
#define _LOG_TEXT_H
#include "header.h"


/*
    <LOG_TEXT>
    write system's error to [log_path]

    <Function>
        1.get_log_path()
            Change log file's path

        2.log_write()
            Write information into log file.

*/
namespace LOG_TEXT{

    char log_path[1024] = "oj_log";

    void get_log_path(const char path[]){
        strcpy(log_path, path);
    }

    void log_write(const char *fmt, ...){

        va_list ap;
        char buffer[2048];
        
        FILE *fp = fopen(log_path, "a+");
        if(fp == NULL){
            return;
        }
        
        va_start(ap, fmt);
        vsprintf(buffer, fmt, ap);
        fprintf(fp, "%s\n", buffer);
        va_end(ap);
        fclose(fp);
    }
};
#endif // _LOG_TEXT_H
