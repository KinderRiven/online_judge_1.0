/*
    Judge Client for OnlineJudge

*/
#include "header.h"
#include "judge_problem.h"

#ifndef BUFFER_SIZE
    #define BUFFER_SIZE 1024
#endif

#define MAX_VALUE   1002400


#define DEBUG 0

int run_id;
PROBLEM run_problem;
vector<string>vecInFile;
vector<string>vecOutFile;


///////////////////////////////////////////FUNCTION//////////////////////////////////////////////

void getIOFile();                                               // get problem input/output file
void get_now_dir();                                             // get now dir
void get_file_type();                                           // get file type
void security_control();                                        // safety control
void get_user_message();                                        // get user information
void sendResult(int op = 0);                                    // sendResult to database
void compiler_source_code();                                    // compiler
void set_run_problem_limit();                                   // set limit
void signal_time_out(int _signo);                               // signal time out
bool compare_suffix(string s1, string s2);                      // compare
int set_timer(int which, int milliseconds);                     // set timer
void io_file_open(string in_path, string out_path);             // I/O File open
void parameter_analysis(int argc, char *argv[]);                // get parameter
void get_problem_message(int argc, char *argv[]);               // get problem message
int execute_cmd(const char *fmt, ...);                          //run shell
void getFileNumber(string path,                                 
    std::vector<string> &vec, string type = "");                //get file list depend type
int compare_output(std::string file_std, std::string file_exec);
void get_result(string out1, string out2, string result);       //get file result 

///////////////////////////////////////DEBUG///////////////////////////////////////////////////
/*
    <get_user_message>
        Get user's message [uid and username].
*/
void get_user_message(){
    uid_t uid = getuid();
    printf("%d\n", uid);
    struct passwd *user = getpwuid(uid);
    printf("%d %s\n", uid, user -> pw_name);
}

/*
    Get now dir
*/
void get_now_dir(){

    char cwd[1024];
    getcwd(cwd, 1024);
    printf("%s\n", cwd);

}

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
///////////////////////////////////////////////////////////////////////////////////////////////
void getFileNumber(string path, vector<string>&vec, string type){

    DIR *dir;
    struct dirent *ptr;

    execute_cmd("rm %s/*~", path.c_str());
    
    const char *buffer = type.c_str();
    if((dir = opendir(path.c_str())) == NULL){

        LOG_TEXT::log_write("Get File List ERROR!");

        exit(EXIT_MESSAGE::get_file_list);
    }

    while((ptr = readdir(dir)) != NULL){

        string file = string(ptr -> d_name);

        if(type == "" || strstr(ptr -> d_name, buffer)){

            vec.push_back(file);

        }

    }
}

void getIOFile(){

    getFileNumber(run_problem.run_dir + "/" + run_problem.io_path, 
        vecInFile,run_problem.in_file);

    getFileNumber(run_problem.run_dir + "/" + run_problem.io_path, 
        vecOutFile,run_problem.out_file);


    sort(vecInFile.begin(), vecInFile.end());

    //for(int i = 0; i < vecInFile.size(); i++)
    //    cout << vecInFile[i] << endl;

    sort(vecOutFile.begin(), vecOutFile.end());

    //for(int i = 0; i < vecOutFile.size(); i++)
    //    cout << vecOutFile[i] << endl;


    if(vecOutFile.size() != vecInFile.size()){

        LOG_TEXT::log_write("IO File Number error!");

        exit(EXIT_MESSAGE::get_file_list);

    }
}

///////////////////////////////////////////////////////////////////////////////////////////////

/*
    Set timer
*/
void signal_time_out(int _signo){
    if(_signo == SIGALRM){
        LOG_TEXT::log_write("Time out!");
        exit(EXIT_MESSAGE::time_out);
    }
}

/*
    Set timer
        if (OK) return 0
        if (ERROR) return -1
*/
int set_timer(int which, int milliseconds){
    struct itimerval t;
    t.it_value.tv_sec = milliseconds / 1000;
    t.it_value.tv_usec = milliseconds % 1000 * 1000; //微秒
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 0;
    return setitimer(which, &t, NULL);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
/*
    -t  time limit
    -m  memory limit
    -c  code path
    -r  run  path
*/
void parameter_analysis(int argc, char *argv[]){
    extern char *optarg;
    int op;
    while((op = getopt(argc, argv, "t:m:c:r:")) != -1){
        switch(op){
            case 't':
                run_problem.time_limit = atoi(optarg);
                break;
            case 'm':
                run_problem.memory_limit = atoi(optarg);
                break;
            case 'c':
                run_problem.code_path = string(optarg);
                break;
            case 'r':
                run_problem.run_dir = string(optarg);
                run_id = atoi(optarg);
                break;
            default:
                LOG_TEXT::log_write("Parameter error.");
                exit(EXIT_MESSAGE::parameter_error);
        }
    }
}
/*
    Get resource language from file type.
        [.c]    C language
        [.cpp]  C++ language
        [.java] Java language
*/
bool compare_suffix(string s1, string s2){

    return s1.size() > s2.size() && s1.compare(s1.size() - s2.size(), s2.size(), s2) == 0;

}

void get_file_type(){
    
    if(compare_suffix(run_problem.code_path, "c"))
        run_problem.lang = JUDGE_MESSAGE::lang_c;

    else if(compare_suffix(run_problem.code_path, "cpp"))
        run_problem.lang = JUDGE_MESSAGE::lang_cpp;

    else if(compare_suffix(run_problem.code_path, "java"))
        run_problem.lang = JUDGE_MESSAGE::lang_java;

    else{
        LOG_TEXT::log_write("File type error!");
        exit(EXIT_MESSAGE::file_type_error);
    }
}
/*
    Get problem's message

*/
void get_problem_message(int argc, char *argv[]){

    parameter_analysis(argc, argv);
    get_file_type();
}

/*
    Compare output file
*/
int compare_output(std::string file_std, std::string file_exec) {

    FILE *fp_std = fopen(file_std.c_str(), "r");
    if (fp_std == NULL) {
        LOG_TEXT::log_write("Open standard output file failed.");
        exit(EXIT_MESSAGE::compare_file);
    }

    FILE *fp_exe = fopen(file_exec.c_str(), "r");
    if (fp_exe == NULL) {
        LOG_TEXT::log_write("Open executive output file failed.");
        exit(EXIT_MESSAGE::compare_file);
    }
    int a, b, Na = 0, Nb = 0;
    enum {
        AC = RESULT_MESSAGE::AC,
        PE = RESULT_MESSAGE::PE,
        WA = RESULT_MESSAGE::WA,
    }status = AC;
    while (true) {
        a = fgetc(fp_std);
        b = fgetc(fp_exe);
        Na++, Nb++;

        if (a == '\r') {
            a = fgetc(fp_std);
            Na++;
        }
        if (b == '\r') {
            b = fgetc(fp_std);
            Nb++;
        }
#define is_space_char(a) ((a == ' ') || (a == '\t') || (a == '\n'))

        if (feof(fp_std) && feof(fp_exe)){
            break;
        } else if (feof(fp_std) || feof(fp_exe)) {
            FILE *fp_tmp;
            if (feof(fp_std)) {
                if (!is_space_char(b)) {
                    //LOG_TEXT::log_write("Well, Wrong Answer.");
                    status = WA;
                    break;
                }
                fp_tmp = fp_exe;
            } else {
                if (!is_space_char(a)) {
                    //LOG_TEXT::log_write("Well, Wrong Answer.");
                    status = WA;
                    break;
                }
                fp_tmp = fp_std;
            }
            int c;
            while ((c = fgetc(fp_tmp)) != EOF) {
                if (c == '\r') c = '\n';
                if (!is_space_char(c)) {
                    //LOG_TEXT::log_write("Well, Wrong Answer.");
                    status = WA;
                    break;
                }
            }
            break;
        }

        if (a != b) {
            status = PE;
            if (is_space_char(a) && is_space_char(b)) {
                continue;
            }
            if (is_space_char(a)) {
                ungetc(b, fp_exe);
                Nb--;
            } else if (is_space_char(b)) {
                ungetc(a, fp_std);
                Na--;
            } else {
                //LOG_TEXT::log_write("Well, Wrong Answer.");
                status = WA;
                break;
            }
        }
    }
    fclose(fp_std);
    fclose(fp_exe);
    return status;
}

/*
    Output result to result file
*/
int result_array[10] = {0};
int max_time = 0, max_memory = 0;
void get_result(string out1_path, string out2_path, string result_path){
    
    if(run_problem.result == RESULT_MESSAGE::UN){

        run_problem.result = compare_output(out1_path, out2_path);

    }

    run_problem.out_result(result_path);

    result_array[run_problem.result] ++;

    max_time = max(max_time, run_problem.time_used);

    max_memory = max(max_memory, run_problem.memory_used);
}
/*
    Start a new process to compiler code
    if compiler_time > compiler_time_limit
        result = RE
    else 
        result = OK
*/

                           
void compiler_source_code(){

    pid_t pid = fork();

    if(pid < 0){

        LOG_TEXT::log_write("Fork error!");
        exit(EXIT_MESSAGE::fork_error);


    }
    else if(pid == 0){
        /*
            Set compiler_time
        */
        set_timer(ITIMER_REAL, JUDGE_MESSAGE::compiler_time);

        string exec_out_path = run_problem.run_dir + "/" + run_problem.exec_file;
        string compiler_out_path = run_problem.run_dir + "/" + run_problem.compiler_file;

        freopen(compiler_out_path.c_str(), "w", stderr);

        /*
            C
            C++
        */
        switch(run_problem.lang){

            //C
            case JUDGE_MESSAGE::lang_c:
                execlp("gcc", "gcc", "-o", exec_out_path.c_str(), run_problem.code_path.c_str(), 
                    "-static", "-w", "-lm", "-std=c99",  "-O2",  "-DONLINE_JUDGE", NULL);
                break;

            //C++
            case JUDGE_MESSAGE::lang_cpp:
                execlp("g++", "g++", "-o", exec_out_path.c_str(), run_problem.code_path.c_str(), 
                "-static", "-w", "-lm", "-std=c++11", "-O2", "-DONLINE_JUDGE", NULL);
                break;

            //Java
            case JUDGE_MESSAGE::lang_java:
                break;
        }

        LOG_TEXT::log_write("Compiler error!");

        exit(EXIT_MESSAGE::compiler_error);
    }
    else{
        int status;
        int t = waitpid(pid, &status, WUNTRACED);

        if(t == -1){
            LOG_TEXT::log_write("Waitpid error!");
            exit(EXIT_MESSAGE::compiler_error);
        }

        /*
            EXIT OK
        */
        if(WIFEXITED(status)){
            int val = WEXITSTATUS(status);
            if(val == EXIT_SUCCESS){

                LOG_TEXT::log_write("[%d] Compiler successfully!", run_id);

            }
            else if(val == JUDGE_MESSAGE::compiler_error){
                LOG_TEXT::log_write("[%d] Compiler error!", run_id);
                run_problem.result = RESULT_MESSAGE::CE;

                //CE
                sendResult(1);

                exit(EXIT_MESSAGE::compiler_error);
            }
        }
        /*
            STOP BY SIGNAL
        */
        else if(WIFSIGNALED(status)){
            if(WTERMSIG(status) == SIGALRM){

                LOG_TEXT::log_write("Compiler time out!");
                run_problem.result = RESULT_MESSAGE::RE;

                //CE
                sendResult(1);

                exit(EXIT_MESSAGE::compiler_error);
            }
        }
        /*
            OTHER FACTORS
        */
        else{
            LOG_TEXT::log_write("Compiler stop by unknow factors!");
            exit(EXIT_MESSAGE::compiler_error);
        }
    }
}

/*
    Set low power user
        - nobody uid is 65534 here.
        - change root dir
*/
void security_control(){

    struct passwd *nobody = getpwnam("nobody");

    if(nobody == NULL){
        LOG_TEXT::log_write("Can not find tht user -- nobody!");
        exit(EXIT_MESSAGE::security_control);
    }

    /*
        Change run dir
    */
    if(chdir(run_problem.run_dir.c_str()) != EXIT_SUCCESS){
        LOG_TEXT::log_write("Can not change run dir!");
        exit(EXIT_MESSAGE::security_control);
    }

    char cwd[1024], *tmp = getcwd(cwd, 1024);
    if(tmp == NULL){
        LOG_TEXT::log_write("Can't get read and write access!");
        exit(EXIT_MESSAGE::security_control);
    }

    /*
        Change root directory
        Change user permissions
    */
    if(run_problem.lang != JUDGE_MESSAGE::lang_java){

        if(chroot(cwd) != EXIT_SUCCESS){
            LOG_TEXT::log_write("Can't change root directory!");
            exit(EXIT_MESSAGE::security_control);
        }
        
        if((setuid(nobody -> pw_uid)) != EXIT_SUCCESS){
            LOG_TEXT::log_write("Can't change user permissions!\n");
            exit(EXIT_MESSAGE::security_control);
        }
    }
}

/*
    open I/O
        -stdin
        -stdout
*/
void io_file_open(string in_path, string out_path){

    //cout << in_path << " " << out_path << endl;

    stdin   = freopen(in_path.c_str(), "r", stdin);
    stdout  = freopen(out_path.c_str(), "w", stdout);


    if(stdin == NULL || stdout == NULL){
        LOG_TEXT::log_write("I/O open is error!");
        exit(EXIT_MESSAGE::io_open);
    }
}
/*
    Set run_problem limit
*/
void set_run_problem_limit(){
    rlimit lim;

    /*
        CPU (seconds) (SIGXCPU signal)
    */
    lim.rlim_max = (run_problem.time_limit / 1000 + 1);
    lim.rlim_cur =  lim.rlim_max;
    if(setrlimit(RLIMIT_CPU, &lim) < 0){
        exit(EXIT_MESSAGE::set_limit_error);
    }

    /* 
        STACK 
    */
    getrlimit(RLIMIT_STACK, &lim);
    int stack_size_set = JUDGE_MESSAGE::stack_size * 1024;
    if(lim.rlim_max <= stack_size_set){
        exit(EXIT_MESSAGE::set_limit_error);
    }
    else{
        lim.rlim_max = stack_size_set;
        lim.rlim_cur = stack_size_set;
        if(setrlimit(RLIMIT_STACK, &lim) < 0){
            exit(EXIT_MESSAGE::set_limit_error);
        }
    }

    /*
        CODE LENGTH
    */
    lim.rlim_max = run_problem.code_lenght_limit * 1024;
    lim.rlim_cur = lim.rlim_max;
    if(setrlimit(RLIMIT_FSIZE, &lim) < 0){
        exit(EXIT_MESSAGE::set_limit_error);
    }
}

/*
    Get program status
        -mark
*/
int get_proc_status(int pid, const char *mark){

    FILE* pf;
    char fn[BUFFER_SIZE], buf[BUFFER_SIZE];
    int ret = 0;

    sprintf(fn, "/proc/%d/status", pid);
    pf = fopen(fn, "r");
    int m = strlen(mark);

    while(pf && fgets(buf, BUFFER_SIZE - 1, pf)){

        buf[strlen(buf) - 1] = 0;
        if(strncmp(buf, mark, m) == 0){
            sscanf(buf + m + 1, "%d", &ret);
        }

    }
    if(pf)
        fclose(pf);
    return ret;
}

/*
    Run source code
*/
void run_solution_file(string in_path, string out_path){

    /*
        Open [input file | output file]
    */
    io_file_open(in_path, out_path);
    security_control();

    string run_file = "./" + run_problem.exec_file;

    //get_now_dir();

    set_run_problem_limit();

    if(ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0){
        LOG_TEXT::log_write("Ptrace Error!");
        exit(EXIT_MESSAGE::run_source_code);
    }

    if(set_timer(ITIMER_REAL, run_problem.time_limit) != EXIT_SUCCESS){
        LOG_TEXT::log_write("Set timer error!");
        exit(EXIT_MESSAGE::run_source_code);
    }

    if(run_problem.lang != JUDGE_MESSAGE::lang_java){
        //execl(run_file.c_str(), run_problem.exec_file.c_str(), NULL);
        execl(run_problem.exec_file.c_str(), run_problem.exec_file.c_str(), NULL);
    }

    LOG_TEXT::log_write("Run Error! [%d]", errno);
    exit(EXIT_MESSAGE::run_source_code);
}

//////////////////////////////////////////////////////////////////////////////////////////
/*
 * 这个函数不是我写的
 */
#include "rf_table.h"
//系统调用在进和出的时候都会暂停, 把控制权交给judge
static bool in_syscall = true;
static
bool is_valid_syscall(int lang, int syscall_id, pid_t child, user_regs_struct regs) {
    in_syscall = !in_syscall;
    //FM_LOG_DEBUG("syscall: %d, %s, count: %d", syscall_id, in_syscall?"in":"out", RF_table[syscall_id]);
    if (RF_table[syscall_id] == 0)
    {
        //如果RF_table中对应的syscall_id可以被调用的次数为0, 则为RF
        long addr;
        if(syscall_id == SYS_open)
        {
#if __WORDSIZE == 32
            addr = regs.ebx;
#else
            addr = regs.rdi;
#endif
#define LONGSIZE sizeof(long)
            union u{ unsigned long val; char chars[LONGSIZE]; }data;
            unsigned long i = 0, j = 0, k = 0;
            char filename[300];
            while (true)
            {
                data.val = ptrace(PTRACE_PEEKDATA, child, addr + i,  NULL);
                i += LONGSIZE;
                for (j = 0; j < LONGSIZE && data.chars[j] > 0 && k < 256; j++)
                {
                    filename[k++] = data.chars[j];
                }
                if (j < LONGSIZE && data.chars[j] == 0)
                    break;
            }
            filename[k] = 0;
            //FM_LOG_TRACE("syscall open: filename: %s", filename);
            if (strstr(filename, "..") != NULL)
            {
                return false;
            }
            if (strstr(filename, "/proc/") == filename)
            {
                return true;
            }
            if (strstr(filename, "/dev/tty") == filename)
            {
                //run_problem.result = RESULT_MESSAGE::RE;

                sendResult(2);

                //exit(EXIT_MESSAGE::ok);
            }
        }
        return false;
    } else if (RF_table[syscall_id] > 0) {
        //如果RF_table中对应的syscall_id可被调用的次数>0
        //且是在退出syscall的时候, 那么次数减一
        if (in_syscall == false)
            RF_table[syscall_id]--;
    } else {
        //RF_table中syscall_id对应的指<0, 表示是不限制调用的
        ;
    }
    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////
/*
    Watch child's process while run source code
*/
void watch_solution_file(int pid){

    int status = 0;
    struct rusage rused;
    int syscall_id = 0;                 //system
    struct user_regs_struct regs;       //register
    init_RF_table(run_problem.lang);    //初始化系统调用表
    while(true){

        if(wait4(pid, &status, 0, &rused) < 0){
            LOG_TEXT::log_write("Wait error in run source code!");
            exit(EXIT_MESSAGE::run_source_code);
        }

        if(WIFEXITED(status)){
            int value = WEXITSTATUS(status);
            LOG_TEXT::log_write("Everything is ok for running! [%d]", run_id);
            break;
        }

        if(WIFSIGNALED(status) ||
            (WIFSTOPPED(status) && WSTOPSIG(status) != SIGTRAP)){
            int signo = 0;
            if (WIFSIGNALED(status)) {
                signo = WTERMSIG(status);
                LOG_TEXT::log_write("Child signaled by %d : %s", signo, strsignal(signo));
            } else {
                signo = WSTOPSIG(status);
                LOG_TEXT::log_write("Child stop by %d : %s", signo, strsignal(signo));
            }
            switch (signo) {
                //TLE
                case SIGALRM:
                case SIGXCPU:
                case SIGVTALRM:
                case SIGKILL:
                    //TIME_LIMIT_EXCESS
                    if(DEBUG)
                        LOG_TEXT::log_write("Time Limit Exceeded!");
                    run_problem.result = RESULT_MESSAGE::TLE;
                    break;
                case SIGXFSZ:
                    //OUT_PUT_LIMIT
                    if(DEBUG)
                        LOG_TEXT::log_write("File Limit Exceeded!");
                    run_problem.result = RESULT_MESSAGE::OLE;
                    break;
                case SIGSEGV:
                case SIGFPE:
                case SIGBUS:
                case SIGABRT:
                    //RUNTIME_ERROR
                    if(DEBUG)
                        LOG_TEXT::log_write("Runtime Error!");
                    run_problem.time_used = 0;
                    run_problem.time_used = 0;
                    run_problem.result = RESULT_MESSAGE::RE;
                    break;
                default:
                    //UNKNOW_ERROR
                    run_problem.time_used = 0;
                    run_problem.time_used = 0;
                    run_problem.result = RESULT_MESSAGE::RE;
                    break;
            }

            ptrace(PTRACE_KILL, pid, NULL, NULL);
            break;
        }


        if(run_problem.lang != JUDGE_MESSAGE::lang_java){
            run_problem.memory_used =
                max(run_problem.memory_used, get_proc_status(pid, "VmPeak:"));
        }
        //Java
        else{

        }

        //MemoryLimit
        if (run_problem.memory_used > run_problem.memory_limit) {
            run_problem.time_used = 0;
            run_problem.memory_used = 0;
            run_problem.result = RESULT_MESSAGE::MLE;
            ptrace(PTRACE_KILL, pid, NULL, NULL);
            break;
        }

        //
        if (ptrace(PTRACE_GETREGS, pid, NULL, &regs) < 0) {
            LOG_TEXT::log_write("Ptrace error!");
            break;
        }


#ifdef __i386__
            syscall_id = regs.orig_eax;
#else
            syscall_id = regs.orig_rax;
#endif


        //
        if (syscall_id > 0 &&
                !is_valid_syscall(run_problem.lang, syscall_id, pid, regs)) {

                LOG_TEXT::log_write("restricted fuction %d", syscall_id);

                if (syscall_id == SYS_rt_sigprocmask){
                    LOG_TEXT::log_write("The glibc failed");

                } else {
                    LOG_TEXT::log_write("restricted fuction table");
                }
                run_problem.result = RESULT_MESSAGE::RE;
                ptrace(PTRACE_KILL, pid, NULL, NULL);
                break;
            }

        if (ptrace(PTRACE_SYSCALL, pid, NULL, NULL) < 0) {
            LOG_TEXT::log_write("Ptrace error!");
            break;
        }
    }
    /*
        run time
    */
    if(run_problem.result == RESULT_MESSAGE::UN){

        run_problem.time_used += (rused.ru_utime.tv_sec * 1000 + rused.ru_utime.tv_usec / 1000);
        run_problem.time_used += (rused.ru_stime.tv_sec * 1000 + rused.ru_stime.tv_usec / 1000);

    }
    else{

        run_problem.time_used = 0;
        run_problem.memory_used = 0;

    }
}


/*
    Judge
*/
void judge(int id){

    run_problem.init();

    string in_path = run_problem.run_dir + "/" + run_problem.io_path + "/" + vecInFile[id];

    string out_path = run_problem.run_dir + "/" + run_problem.io_path + "/" + vecOutFile[id];

    string myout_path = run_problem.run_dir + "/" + run_problem.my_out_path + "/" + vecOutFile[id];

    string result_path = run_problem.run_dir + "/" + run_problem.my_result_path + "/" + vecOutFile[id];

    pid_t pid = fork();
    /*
        judge error
    */
    if(pid < 0){

        exit(EXIT_MESSAGE::run_source_code);

    }
    else if(pid == 0){

        run_solution_file(in_path, myout_path);

    }
    else{

        watch_solution_file(pid);

        get_result(out_path, myout_path, result_path);
    
    }
    
}

//EXIT 
void sendResult(int op){
  
    //Judge Result
    int result;
    if(op == 0){

        if(result_array[RESULT_MESSAGE::MLE]){

            result = RESULT_MESSAGE::MLE;

        }
        else if(result_array[RESULT_MESSAGE::TLE]){

            result = RESULT_MESSAGE::TLE;

        }
        else if(result_array[RESULT_MESSAGE::OLE]){

            result = RESULT_MESSAGE::OLE;

        }
        else if(result_array[RESULT_MESSAGE::RE]){

            result = RESULT_MESSAGE::RE;

        }
        else if(result_array[RESULT_MESSAGE::WA]){

            result = RESULT_MESSAGE::WA;

        }
        else if(result_array[RESULT_MESSAGE::PE]){

            result = RESULT_MESSAGE::PE;

        }
        else{

            result = RESULT_MESSAGE::AC;
        }
    }
    
    //Compiler Error
    else if(op == 1){

        result = RESULT_MESSAGE::CE;
    }

    //Runtime error
    else if(op == 2){

        LOG_TEXT::log_write("Runtime error!");
        result = RESULT_MESSAGE::RE;

    }
    MYSQL_CONNECT mysql_con;
    mysql_con.connect_mysql_database(MYSQL_MESSAGE::database_name);

    if(result != RESULT_MESSAGE::AC)
        max_time = max_memory = 0;

    //UPDATE sdustoj.solution SET time = 1 , memory = 2, result = 3 where solution_id = 3;

    mysql_con.run_sql_world("UPDATE %s SET time = %d, memory = %d, result = %d where solution_id = %d",
        MYSQL_MESSAGE::solution_name, max_time, max_memory, result, run_id);


    // Delete Input Folder

    char buffer[BUFFER_SIZE];

    if(JUDGE_MESSAGE::save_in_file == false){

        sprintf(buffer, "%d/%s", run_id, run_problem.io_path.c_str());
        execute_cmd("rm -rf %s", buffer);
    }

    if(JUDGE_MESSAGE::save_out_file == false){

        sprintf(buffer, "%d/%s", run_id, run_problem.my_out_path.c_str());
        execute_cmd("rm -rf %s", buffer);

    }

    exit(EXIT_MESSAGE::ok);

    //remove

}
/*
    test
*/
void test(){


}
/*
    Main
*/
int main(int argc, char *argv[]){

    if (geteuid() != 0) {
        LOG_TEXT::log_write("You must run this program by root.");
        exit(EXIT_MESSAGE::root_permission_error);
    }

    get_problem_message(argc, argv);

    LOAD_SETTING::start();

    getIOFile();

    if(set_timer(ITIMER_REAL, JUDGE_MESSAGE::judge_time) == -1){

        LOG_TEXT::log_write("Set timer error.");

        exit(EXIT_MESSAGE::set_timer_error);

    }

    signal(SIGALRM, signal_time_out);

    compiler_source_code();

    for(int i = 0; i < vecInFile.size(); i++){

        judge(i);

    }

    sendResult();

    return 0;
}
