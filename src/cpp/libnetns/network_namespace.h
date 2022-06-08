#ifndef NETWORK_NAMESPACE_H__
#define NETWORK_NAMESPACE_H__
void create_network_namespace(char* name);
void specified_network_namespace(char* name,char* cmd);
void create_default_veth_pair();
void create_specified_veth_pair(char* vethA,char* vethB);
void veth_into_namespace(char* veth_name,char* namespace_name);
void config_ip_online(char* namespace_name,char* veth_name,char* ip);
#endif