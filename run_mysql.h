#ifndef _RUN_MYSQL_H
#define _RUN_MYSQL_H


#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/resource.h>
#include <sys/ptrace.h>
#include <mysql/mysql.h>
#include <stdarg.h>
/*
    MySQL Message
*/

#ifndef BUFFER_LEN
    #define BUFFER_LEN 128
#endif
    
namespace MYSQL_MESSAGE{
    
    char host[BUFFER_LEN] = "192.168.131.156";        //127.0.0.1
    char username[BUFFER_LEN] = "sdustoj";            //root
    char password[BUFFER_LEN] = "sdust";              //

    //
    const int wait = -1;

    //name
    char database_name[BUFFER_LEN] = "sdustoj";       //OnlineJudge

    char solution_name[BUFFER_LEN] = "solution";
    char source_name[BUFFER_LEN]  = "source_code";
    char problem_name[BUFFER_LEN] = "problem";
}

/*
    MYSQL_CONNECT  
*/

struct MYSQL_CONNECT{
    string connect_name;
    MYSQL *mysql_connect;
    bool DEBUG;

    MYSQL_CONNECT(string cname = "null", bool DEBUG = false){
        connect_name = cname;
        this->DEBUG = DEBUG;
    }
    /*

    */
    bool connect_mysql_database(const char *database_name){
        mysql_connect = mysql_init(NULL);
        mysql_real_connect(
            mysql_connect,              //connect
            MYSQL_MESSAGE::host,        //ip
            MYSQL_MESSAGE::username,    //username
            MYSQL_MESSAGE::password,    //pwd
            database_name,              //database
            0, NULL, 0
        );
        if(mysql_connect){

            if(DEBUG)
                printf("MySQl[%s] has connected!\n", connect_name.c_str());

            return true;
        }
        else{

            if(DEBUG)
                printf("MySQL[%s] connect error!\n", connect_name.c_str());

            return false;
        }
    }
    /*
        run SQL{
            mysql_num_rows(mysql_result);
            mysql_num_fields(mysql_result);
            mysql_fetch_row(mysql_result);
        }
    */
    MYSQL_RES* run_sql_world(const char *sql...){

        char buffer[2048];
        va_list ap;
        va_start(ap, sql);
        vsprintf(buffer, sql, ap);
        va_end(ap);

        if(DEBUG)
            printf("%s\n", buffer);

        if(mysql_query(mysql_connect, buffer) == 0){
           MYSQL_RES* mysql_result = mysql_store_result(mysql_connect);
           return mysql_result;
        }
        else{
            return NULL;
        }
    }
};
#endif
