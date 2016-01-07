#ifndef _JUDGE_HEADER_H
#define _JUDGE_HEADER_H


//C
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
#include <sys/resource.h>
#include <sys/ptrace.h>
#include <mysql/mysql.h>
#include <stdarg.h>
#include <dirent.h>

//C++
#include <string>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>


//my
#include "solution_table.h"
#include "problem_table.h"
#include "resouce_table.h"
#include "judge_message.h"
#include "judge_problem.h"
#include "run_mysql.h"
#include "LOG_TEXT.h"
#include "load_setting.h"

using namespace std;
//Define
#ifndef BUFFER_SIZE
#define BUFFER_SIZE 1024
#endif

#endif // _JUDGE_HEADER_H


