#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>

#include "create_service.h"

int test_in_linux(char* py_path) {
    FILE* fp;
    const char* path = py_path;
    fp = fopen(path, "w+");//例："/usr/lib/test2.py"
    if (fp) {//向test文件写入死循环代码
        fwrite("#!/usr/bin/python3.8", strlen("#!/usr/bin/python3.8"), 1, fp);
        //fwrite(row1, strlen(row1), 1, fp);
        fwrite("\r\n", 1, 2, fp);
        char* row2 = "while True:";
        fwrite(row2, strlen(row2), 1, fp);
        fwrite("\r\n", 1, 2, fp);
        char* row3 = "    pass";
        fwrite(row3, strlen(row3), 1, fp);
        fwrite("\r\n", 1, 2, fp);
        fclose(fp);
    }
    char buf[1024];
memset(buf, 0, sizeof(buf));
strcpy(buf, "chmod +x ");
strcat(buf, path);
const char* r = buf;
/* char *r1 = str_connect("chmod +x ",py_path);
printf("%s",r1);
const char* r = r1; */
system(r);
    
}

int create(char* service_name,char* py_path,char* slice_name) {
    FILE* fp;
    
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    strcpy(buf, "/etc/systemd/system/");
    strcat(buf, service_name);
    strcat(buf, ".service");
    
    const char* path = buf;
    fp = fopen(path, "w+");

    if(fp){
        
        fwrite("[Unit]", strlen("[Unit]"), 1, fp);
        fwrite("\r\n", 1, 2, fp);

        strcpy(buf, "Description=");
        strcat(buf, service_name);
        fwrite(buf, strlen(buf), 1, fp);
        fwrite("\r\n", 1, 2, fp);

        strcpy(buf, "ConditionFileIsExecutable=");
        strcat(buf, py_path);
        fwrite(buf, strlen(buf), 1, fp);
        fwrite("\r\n", 1, 2, fp);

        fwrite("[Service]", strlen("[Service]"), 1, fp);
        fwrite("\r\n", 1, 2, fp);

        fwrite("Type=simple", strlen("Type=simple"), 1, fp);
        fwrite("\r\n", 1, 2, fp);

        strcpy(buf, "ExecStart=");
        strcat(buf, py_path);
        fwrite(buf, strlen(buf), 1, fp);
        fwrite("\r\n", 1, 2, fp);

        strcpy(buf, "Slice=");
        strcat(buf, slice_name);
        strcat(buf, ".slice");
        fwrite(buf, strlen(buf), 1, fp);
        fwrite("\r\n", 1, 2, fp);

        fwrite("CPUAccounting=yes", strlen("CPUAccounting=yes"), 1, fp);
        fwrite("\r\n", 1, 2, fp);

        fwrite("MemoryAccounting=yes", strlen("MemoryAccounting=yes"), 1, fp);
        fwrite("\r\n", 1, 2, fp);

        fwrite("[Install]", strlen("[Install]"), 1, fp);
        fwrite("\r\n", 1, 2, fp);

        fwrite("WantedBy=multi-user.target", strlen("WantedBy=multi-user.target"), 1, fp);
        fwrite("\r\n", 1, 2, fp);
        
        fclose(fp); 
    }
    system("systemctl daemon-reload");
    strcpy(buf, "systemctl restart ");
    strcat(buf, service_name);
    const char* r = buf;
    system(r);
}

int modify_limit(char* service_name,char* modify_property,char* modify_content){
    FILE* fp;
    char buf[100];
    memset(buf, 0, sizeof(buf));
    strcpy(buf, "/etc/systemd/system/");
    strcat(buf, service_name);
    strcat(buf, ".service");
    const char* path = buf;
    //memset(buf, 0, sizeof(buf));
    fp = fopen(path, "r+");
    char str[100];
    char *temp;
    char buf1[1000];
    memset(buf1, 0, sizeof(buf1));
    char temp1[500];
    memset(temp1, 0, sizeof(temp1));
    char temp2[500];
    memset(temp2, 0, sizeof(temp2));
    //fscanf(fp,"%[^\n]\n",str);
    printf("%s\n",str);
    int flag = 0;
    int i =0;
    strcpy(temp2,modify_property);
    strcat(temp2,"=");
    strcat(temp2,modify_content);
    strcat(temp2,"\n");

    while(fgets(str, 100, fp) != NULL){
        if(strstr(str,modify_property)){
            
            flag = 1;
            strcpy(str,temp2);
        };
        if(flag == 0){

            if(strstr(str,"Install")){
                strcpy(str,temp2);
            strcat(str,"[Install]\n");
            }
        }
        sprintf(buf1, "%s%s", buf1, str);
    }
    rewind(fp);
    fwrite(buf1,strlen(buf1),1,fp);
    
    system("systemctl daemon-reload");
    strcpy(buf, "systemctl restart ");
    strcat(buf, service_name);
    const char* r = buf;
    system(r);
}