
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include "inc/logger.h"
//check that the folder exists and creates it
int create_folder(void);
/*opens file and add message to end
  path: path to file
  message: message to add
*/
int write_to_file(char *path, char *message);

int write_to_file(char *path, char *message){
    FILE *file = NULL;
    file = fopen(path, "a"); // open in append mode
    if(file == NULL){
        fclose(file);
        return -1;
    }
    fwrite(message, 1, strlen(message), file);
    fclose(file);
    return 0;
}

int write_log(char *message){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char path[128];
    char temp_message[128];
    int err;
    // check if folder exist
    err = create_folder();
    if(err != 0){
        return -1;
    }
    //prepear message
    sprintf(temp_message, "%02d:%02d:%02d -> %s\n", tm.tm_hour, tm.tm_min, tm.tm_sec, message);
    //prepear path to file
    sprintf(path, "logs/log_%02d.%02d", tm.tm_mday, tm.tm_mon + 1);
    err = write_to_file(path, temp_message);
    if(err != 0){
        return err;
    }
    return 0;
}


int create_folder(void){
    const char *folder = "./logs";
    struct stat sb;
    int err;
    if(stat(folder, &sb) == 0 && S_ISDIR(sb.st_mode)){
        return 0; // folder already exist
    }else{
        err = system("mkdir logs");
    }
    if(err != 0){
        return err;
    }
    return 0;
}
