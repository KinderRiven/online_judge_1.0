#ifndef _LOGD_SETTING
#define _LOGD_SETTING
#include "run_mysql.h"
#include <string.h>
#include <stdio.h>
#include "judge_message.h"

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 2048
#endif
namespace LOAD_SETTING{

	char config[] = "judge.config";

	void change(char* key, char* value){

		if(strcmp(key, "mysql_ip") == 0){

			LOG_TEXT::log_write("Mysql Host [%s].", value);
			strcpy(MYSQL_MESSAGE::host, value);
		}

		if(strcmp(key, "mysql_usr") == 0){

			LOG_TEXT::log_write("Mysql UserName [%s].", value);
			strcpy(MYSQL_MESSAGE::username, value);
		}

		if(strcmp(key, "mysql_pwd") == 0){

			LOG_TEXT::log_write("Mysql PassWord [%s].", value);
			strcpy(MYSQL_MESSAGE::password, value);
		}

		if(strcmp(key, "mysql_data") == 0){

			LOG_TEXT::log_write("Mysql Database [%s].", value);
			strcpy(MYSQL_MESSAGE::database_name, value);
		}

		if(strcmp(key, "save_out") == 0){

			if(strcmp(value, "true") == 0){

				//LOG_TEXT::log_write("Save out file true.");
				JUDGE_MESSAGE::save_out_file = true;

			}
			else{

				//LOG_TEXT::log_write("Save out file false.");
				JUDGE_MESSAGE::save_out_file = false;

			}

		}

		if(strcmp(key, "save_in") == 0){

			if(strcmp(value, "true") == 0){

				//LOG_TEXT::log_write("Save in file true.");
				JUDGE_MESSAGE::save_in_file = true;

			}
			else{

				//LOG_TEXT::log_write("Save in file false.");
				JUDGE_MESSAGE::save_in_file = false;

			}
		}
	}

	bool start(){

		FILE *file = fopen(config, "r");

		if(file == NULL){

			LOG_TEXT::log_write("Can't find config file!");
			return false;

		}

		char buffer[BUFFER_SIZE];
		char setting[2][BUFFER_SIZE];
		int len = 0, length = 0, mark = 0;
		while((len = fread(buffer, sizeof(char), sizeof(buffer), file)) > 0){

			for(int i = 0; i < len; i++){

				if(buffer[i] == '<'){

					length = 0;
					mark = 0;

				}

				else if(buffer[i] == '>'){

					setting[mark][length] = '\0';
					change(setting[0], setting[1]);
					length = mark = 0;
				}
				else if(buffer[i] == '-'){

					setting[mark][length] = '\0';
					mark = 1;
					length = 0;

				}
				else{
					setting[mark][length++] = buffer[i];
				}
			}
		}

		fclose(file);
	}
}
#endif