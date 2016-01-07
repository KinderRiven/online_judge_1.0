#ifndef _JUDGE_PROBLEM_H
#define _JUDGE_PROBLEM_H


//header
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/syscall.h>
#include <sys/ptrace.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <sys/resource.h>
#include <mysql/mysql.h>
using namespace std;
/*
    PROBLEM sturct
*/
struct PROBLEM{

    int lang;                   //language
    int time_limit;             //time limit
    int stack_limit;            //stack
    int memory_limit;           //memory limit
    int code_lenght_limit;      //code length

    int time_used;              //time uese
    int memory_used;            //memory used
    int result;                 //result -> RESULT_MESSAGE

    //PATH
    string run_dir;             //run dir -> solution_id
    string code_path;           //code path -> solution + code_name
    string io_path;
    string my_out_path;
    string my_result_path;

    //FILE
    string file_name;           // [name]
    string in_file;             // .in
    string out_file;            // .out
    string exec_file;           // .run
    string my_out_file;         // .my
    string result_file;         // .result
    string compiler_file;       //

    void init(){

        result = time_used = memory_used = 0;
        
    }
    PROBLEM(){

        init();

        io_path = "IO";
        my_out_path = "MY";
        my_result_path = "RES";
        compiler_file = "compiler";

        //default
        time_limit = 1000;              //ms
        memory_limit = 65535;           //kb
        code_lenght_limit = 1024000;    //length

        run_dir = code_path = "";

        file_name = "a";
        in_file = file_name + ".in";
        out_file = file_name + ".out";
        exec_file = file_name + ".run";
        result_file = file_name + ".result";
        my_out_file = file_name +".my";


    }
    /*
        Output resutlt
    */
    void out_result(string out_path){
        
        FILE* fp = fopen(out_path.c_str(), "w");
        fprintf(fp, "%s", "[result]: ");

        /*
            const int UN = 0;
            const int AC = 1;
            const int RE = 2;
            const int PE = 3;
            const int WA = 4;
            const int TLE = 5;
            const int OLE = 6;
        */
        switch(result){

            case RESULT_MESSAGE::UN: 
                fprintf(fp, "%s", "Unknow\n"); 
                break;

            case RESULT_MESSAGE::AC: 
                fprintf(fp, "%s", "Accepted\n"); 
                break;

            case RESULT_MESSAGE::RE: 
                fprintf(fp, "%s", "Runtime Error\n"); 
                break;

            case RESULT_MESSAGE::PE: 
                fprintf(fp, "%s", "Presentation Error\n"); 
                break;

            case RESULT_MESSAGE::WA: 
                fprintf(fp, "%s", "Wrong Answer\n"); 
                break;

            case RESULT_MESSAGE::TLE: 
                fprintf(fp, "%s", "Time Limit Exceeded\n"); 
                break;

            case RESULT_MESSAGE::OLE: 
                fprintf(fp, "%s", "Output Limit Exceeded\n"); 
                break;

            case RESULT_MESSAGE::MLE: 
                fprintf(fp, "%s", "Memory Limit Exceeded\n"); 
                break;

            case RESULT_MESSAGE::CE: 
                fprintf(fp, "%s", "Compiler Error\n"); 
                break;

            default:
                fprintf(fp, "%s", "Unknow\n");
                break;
        }
        fprintf(fp, "[time]: %d ms\n", time_used);
        fprintf(fp, "[memory]: %d kb\n", memory_used);
    }
    /*
        show result
    */
    void out_used(){

        printf("\n/////////////// PROBLEM  OUTPUT ///////////////\n");
        printf("[result]: ");

        switch(result){

            case RESULT_MESSAGE::UN: 
                printf("Unknow Error\n"); 
                break;

            case RESULT_MESSAGE::AC: 
                printf("Accepted\n"); 
                break;

            case RESULT_MESSAGE::RE: 
                printf("Runtime Error\n"); 
                break;

            case RESULT_MESSAGE::PE: 
                printf("Presentation Error\n"); 
                break;

            case RESULT_MESSAGE::WA: 
                printf("Wrong Answer\n"); 
                break;

            case RESULT_MESSAGE::TLE: 
                printf("Time Limit Exceeded\n"); 
                break;

            case RESULT_MESSAGE::OLE: 
                printf("Output Limit Exceeded\n"); 
                break;

            case RESULT_MESSAGE::MLE: 
                printf("Memory Limit Exceeded\n"); 
                break;

            default:
                printf("Unknow Error\n");
                break;
        }
        printf("[time]: %d ms\n", time_used);
        printf("[memory]: %d kb\n", memory_used);
        printf("//////////////////////////////////////////////////\n");
    }
    /*
        show problem
    */
    void out_show(){
        printf("\n/////////////// RESOURCE OUTPUT ///////////////\n");

        printf("[Language ]: ");
        switch(lang){

            case 1:
                printf("C\n"); 
                break;

            case 2:
                printf("C++\n"); 
                break;

            case 3:
                printf("Java\n"); 
                break;

            default:
                printf("Unknow\n"); 
                break;
        }

        printf("[Run   Dir]: %s\n", run_dir.c_str());
        printf("[Code Path]: %s\n", code_path.c_str());
        printf("[Time   Limit]: %d ms\n",time_limit);
        printf("[Memory Limit]: %d kb\n",memory_limit);
        printf("[Length Limit]: %d B\n", code_lenght_limit);
        printf("//////////////////////////////////////////////////\n");
    }
}problem;
#endif // _JUDGE_PROBLEM_H
