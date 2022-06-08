#ifndef CREATE_SERVICE_H__
#define CREATE_SERVICE_H__
int test_in_linux(char* py_path);
int create(char* service_name,char* py_path,char* slice_name);
int modify_limit(char* service_name,char* modify_property,char* modify_content);
#endif


