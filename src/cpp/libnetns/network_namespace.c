#include<string.h>
#include<stdlib.h>
#include "network_namespace.h"

void create_network_namespace(char* name){
    char cmd[100];
    memset(cmd, 0, sizeof(cmd));
    strcpy(cmd,"ip netns add ");
    strcat(cmd,name);
    system(cmd);
}

void specified_network_namespace(char* name,char* cmd){
    char t[100];
    memset(t, 0, sizeof(t));
    strcpy(t,"ip netns exec ");
    strcat(t,name);
    strcat(t," ");
    strcat(t,cmd);
}

void create_default_veth_pair(){
    char* cmd = "ip link add type veth";
    system(cmd);
}

void create_specified_veth_pair(char* vethA,char* vethB){
    char t[100];
    memset(t, 0, sizeof(t));
    strcpy(t,"ip link add ");
    strcat(t,vethA);
    strcat(t," type veth peer name ");
    strcat(t,vethB);
    system(t);
}

void veth_into_namespace(char* veth_name,char* namespace_name){
    char t[100];
    memset(t, 0, sizeof(t));
    strcpy(t,"ip link set ");
    strcat(t,veth_name);
    strcat(t," netns ");
    strcat(t,namespace_name);
    system(t);
}

void config_ip_online(char* namespace_name,char* veth_name,char* ip){
    char t[100];
    memset(t, 0, sizeof(t));
    strcpy(t,namespace_name);
    strcat(t," ip link set ");
    strcat(t,veth_name);
    strcat(t," up ");
    specified_network_namespace(namespace_name,t);

    strcpy(t,namespace_name);
    strcat(t," ip addr add ");
    strcat(t,ip);
    strcat(t," dev ");
    strcat(t,veth_name);
    specified_network_namespace(namespace_name,t);
}

