#include "header.h"
#include "run_mysql.h"

#define DEBUG 1
#define PRINT_DEBUG 1
#define MAX_JUDGE_PRO 1024
#define MAX_JUDGE_CLIENT 1024


string io_file_path = "iofile/";

MYSQL_CONNECT mysql_con("root", false);
int judge_client_num = 1;
int running_judge_clinet = 0;
int sleep_time = 3;                     //seconds

////////////////////////////////////////////////////////////////////////////////////

void work();                                                    //work run client
void test();                                                    //test
void watch_judge_queue();                                       //watch judge queue
void run_judge_client(int judge_id);                            //to start client
int execute_cmd(const char * fmt, ...);                         //shell
void run_judge_clinet_mkdir(int solution_id);                   //build dir
void parameter_analysis(int argc, char *argv[]);
string run_judge_client_getCodePath(int id, int lang);
void get_judge_problems(int judge_problem[], int& num);
bool run_judge_clinet_code(const char *path, int solution_id);
bool run_judge_client_problem(int solution_id, 
    int problem_id, int &time_limit, int &memory_limit);

//////////////////////////////////////////////////////////////////////////////////////
/*
    CMD_SHELL
*/
int execute_cmd(const char * fmt, ...){
    char cmd[1024]; 
    int ret = 0;
    va_list ap;
    va_start(ap, fmt);
    vsprintf(cmd, fmt, ap);
    ret = system(cmd);
    va_end(ap);
    return ret;

}

/*
    Get parameter
        -n the number of judge server running at the same time.
        -s 1 or 0 to loadding setting. 
*/
void parameter_analysis(int argc, char *argv[]){

    extern char *optarg;
    int op;
    while((op = getopt(argc, argv, "n:")) != -1){
        switch(op){

            case 'n':
                judge_client_num = atoi(optarg);
                break;

            default:
                judge_client_num = 0;
                LOG_TEXT::log_write("Parameter_error while starting...");
                exit(EXIT_MESSAGE::parameter_error);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
/*
    Get code path
*/
string run_judge_client_getCodePath(int id, int lang){

    char buffer[BUFFER_SIZE];
    sprintf(buffer, "%d", id);
    string code_path = string(buffer);

    switch(lang){

        case JUDGE_MESSAGE::lang_c:
            code_path += ("/" + problem.file_name + ".c");
            break;

        case JUDGE_MESSAGE::lang_cpp:
            code_path += ("/" + problem.file_name + ".cpp");
            break;

        case JUDGE_MESSAGE::lang_java:
            code_path += ("/" + problem.file_name + ".java");
            break;

        default:
            break;
    }

    return code_path;
}

/*
    Build Dir to run exec
*/
void run_judge_clinet_mkdir(int solution_id){

    execute_cmd("mkdir %d -m 777", solution_id);

    execute_cmd("mkdir %d/%s -m 777", solution_id,  problem.my_out_path.c_str());

    execute_cmd("mkdir %d/%s -m 777", solution_id,  problem.my_result_path.c_str());

}

/*
    Get resource code from Database
*/
bool run_judge_clinet_code(const char *path, int solution_id){

    MYSQL_RES* mysql_result = mysql_con.run_sql_world("SELECT * FROM %s WHERE solution_id = %d",
        MYSQL_MESSAGE::source_name,
        solution_id);

    MYSQL_ROW mysql_row = mysql_fetch_row(mysql_result);

    if(mysql_row != NULL){

        FILE *fp = fopen(path, "w");
        fprintf(fp, "%s", mysql_row[RESOURCE_TABLE::code]);
        fclose(fp);

        return true;
    }
    else
        return false;
}
/*
    Get Problem's Input/Output/TimeLimit/MemoryLimit
*/
bool run_judge_client_problem(int solution_id, int problem_id, int &time_limit, int &memory_limit){

    MYSQL_RES* mysql_result = mysql_con.run_sql_world("SELECT * FROM %s WHERE problem_id = %d",
        MYSQL_MESSAGE::problem_name,
        problem_id);

    MYSQL_ROW mysql_row = mysql_fetch_row(mysql_result);

    if(mysql_row != NULL){

/*
        Get I/O File from database.

        //Inpit
        char path[BUFFER_SIZE];

        sprintf(path, "%d", solution_id);

        string file = string(path) + "/" + problem.in_file;

        FILE *fp = fopen(file.c_str(), "w");

        fprintf(fp, "%s", mysql_row[PROBLEM_TABLE::input]);

        fclose(fp);

        //Output
        file = string(path) + "/" + problem.out_file;

        fp = fopen(file.c_str(), "w");

        fprintf(fp, "%s", mysql_row[PROBLEM_TABLE::output]);

        fclose(fp);
*/

        //IO FILE
        char cp_from[BUFFER_SIZE], cp_to[BUFFER_SIZE];

        sprintf(cp_from, "%s/%d", io_file_path.c_str(), problem_id);

        sprintf(cp_to, "%d/%s", solution_id, problem.io_path.c_str());

        execute_cmd("cp -a %s %s", cp_from, cp_to);

        //TimeLimit
        time_limit = atoi(mysql_row[PROBLEM_TABLE::time_limit]);

        //MemoryLimit
        memory_limit = atoi(mysql_row[PROBLEM_TABLE::memory_limit]);
    }
    else
        return false;
}
/*

*/
void run_judge_client(int judge_id){

    MYSQL_RES* mysql_result = mysql_con.run_sql_world("SELECT * FROM %s WHERE solution_id = %d",
        MYSQL_MESSAGE::solution_name,
        judge_id
        );

    MYSQL_ROW mysql_row = mysql_fetch_row(mysql_result);

    if(mysql_row != NULL){

        int solution_id = atoi(mysql_row[SOLUTION_TABLE::solution_id]);

        int problem_id = atoi(mysql_row[SOLUTION_TABLE::problem_id]);

        int language = atoi(mysql_row[SOLUTION_TABLE::language]);

        int time_limit = 0, memory_limit = 0;

        run_judge_clinet_mkdir(solution_id);

        string code_path = run_judge_client_getCodePath(solution_id, language);

        run_judge_clinet_code(code_path.c_str(), solution_id);

        run_judge_client_problem(solution_id, problem_id, time_limit, memory_limit);

        /*
            -t  time limit
            -m  memory limit
            -c  code path
            -r  run  path
        */

        const char *ps = code_path.c_str();

        
        mysql_con.run_sql_world("UPDATE %s SET result = %d where solution_id = %d",
            MYSQL_MESSAGE::solution_name, RESULT_MESSAGE::RU, solution_id);

        /*
        if(execl("judge_client", "judge_client",
            "-t", time_limit,
            "-m", memory_limit,
            "-c", ps,
            "-r", solution_id,
            NULL) < 0){

                printf("%s\n", strerror(errno));

            }
        */

        execute_cmd("./judge_client -t %d -m %d -c %s -r %d",
            time_limit, memory_limit, ps, solution_id);
        
    }
    else{
        LOG_TEXT::log_write("Cannot find the code!");
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/*

*/
void get_judge_problems(int judge_problem[], int& num){

    MYSQL_RES* mysql_result = mysql_con.run_sql_world("SELECT * FROM %s WHERE result = %d",
        MYSQL_MESSAGE::solution_name,
        MYSQL_MESSAGE::wait
        );

    MYSQL_ROW mysql_row;

    num = 0;

    while((mysql_result != NULL) && (mysql_row = mysql_fetch_row(mysql_result))){

        judge_problem[num++] = atoi(mysql_row[SOLUTION_TABLE::solution_id]);

        if(num > MAX_JUDGE_PRO)
            break;
    }

    //printf("%d\n", num);

}

/*
    Control client running
*/
void work(){

    int judge_problem[MAX_JUDGE_PRO], judge_problem_num = 0;

    memset(judge_problem, 0, sizeof(judge_problem));

    get_judge_problems(judge_problem, judge_problem_num);

    pid_t run_pid[MAX_JUDGE_CLIENT];

    for(int i = 0; i < judge_problem_num; i++){

        int id;

        if(running_judge_clinet >= judge_client_num){

            /*
                Wait any judge clinet.
            */
            int temp = waitpid(-1, NULL, 0);

            running_judge_clinet --;

            for(int j = 0; j < judge_client_num; j++){
                if(temp == run_pid[j]){
                    id = j;
                    if(PRINT_DEBUG)
                        printf("Judge Client [%d] run finish.\n", temp);
                    run_pid[j] = 0;
                    break;
                }

            }
        }
        else{
            for(int j = 0; j < judge_client_num; j++)
                if(run_pid[j] == 0){
                    id = j;
                    break;
                }
        }
        /*
            Start a new judge client.
        */
        if(running_judge_clinet < judge_client_num){

            running_judge_clinet ++;
            run_pid[id] = fork();
            if(run_pid[id] == 0){
                run_judge_client(judge_problem[i]);
                exit(EXIT_MESSAGE::ok);

            }
            else{
                if(PRINT_DEBUG)
                    printf("Judge Client [%d] start run.\n", run_pid[id]);
            }
        }
    }

    while(running_judge_clinet){

        int temp = waitpid(-1, NULL, 0);

        if(PRINT_DEBUG)
            printf("Judge Client [%d] run finish.\n", temp);

        running_judge_clinet --;
    }
}
void watch_judge_queue(){

    mysql_con.connect_mysql_database(MYSQL_MESSAGE::database_name);

    while(true){

        //if(DEBUG)
        //    LOG_TEXT::log_write("Start new judge turn.");

        work();
        
        //break;
        
        sleep(sleep_time);
    }
}
/*
    test
*/
void test(){

    LOAD_SETTING::start();
    exit(0);
}
/*
    main
*/
int main(int argc, char *argv[]){
    /*
        run with root permission
        test();
    */
    //test();

    if(getuid() != 0){

        LOG_TEXT::log_write("You must run with root permission!");
        exit(0);

    }

    parameter_analysis(argc, argv);

    LOAD_SETTING::start();

    LOG_TEXT::log_write("Start with %d client!", judge_client_num);

    if(judge_client_num < 1 || judge_client_num > MAX_JUDGE_CLIENT){

        LOG_TEXT::log_write("The number of judge client error!");
        exit(0);

    }
    /*
        run
    */
    watch_judge_queue();
    exit(0);
}
