#ifndef _JUDGE_MESSAGE_H
#define _JUDGE_MESSAGE_H

#include "header.h"
/*
    <JUDGE_MESSAGE>
        - judge time
        - stack size
        - compiler time
        - language
*/
namespace JUDGE_MESSAGE{
    //time set
    int judge_time = 2500;
    int compiler_time = 1500;

    int stack_size = 8192;
    int compiler_error = 1;

    //language
    const int lang_c = 1;
    const int lang_cpp = 2;
    const int lang_java = 3;

    //setting
    bool save_out_file = false;
    bool save_in_file = false;

}
/*
    <RESULT_MESSAGE>
*/
namespace RESULT_MESSAGE{
    const int RU = -2;
    const int PD = -1;
    const int UN = 0;
    const int AC = 1;
    const int RE = 2;
    const int PE = 3;
    const int WA = 4;
    const int TLE = 5;
    const int OLE = 6;
    const int MLE = 7;
    const int CE  = 8;
}
/*
    <EXIT_MESSAGE>
*/
namespace EXIT_MESSAGE{

    const int root_permission_error = -1;
    const int ok = 0;
    const int time_out  = 3;
    const int parameter_error = 41;
    const int file_type_error = 6;
    const int set_limit_error = 4;
    const int set_timer_error = 5;
    const int io_open = 7;
    const int fork_error = 8;
    const int security_control = 9;
    const int compiler_error = 13;
    const int run_source_code = 15;
    const int compare_file = 18;
    const int get_mysql_code = 22;
    const int get_file_list = 16;

}
namespace ERROR_MESSAGE{
    const int parameter = 1;    //参数错误
}

#endif