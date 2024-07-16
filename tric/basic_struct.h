#include <iostream>
#include <fstream>  //文件操作库
#include <algorithm>  //find()库
#include <vector>   //向量库
#include <unordered_map>  //无序哈希表map
#include <map>
#include <utility>  //pair库

using namespace std;

//**************类的定义*********************************************
//G为无向图
class GNode{
public:
    int v_label;  //储存节点字母
    int v_id;  //节点id
    vector<int> two_way_neighbor_id; //双向保存邻居节点id
public:
    GNode();
    void save_G_node_info(int id,int label);   //储存节点信息
    void add_G_neighbor(int id);   //添加邻居节点
};



//*****************************************************************
//Q为有向图
class QNode{
public:
    int u_label;  //储存节点字母
    int u_id;  //节点id
    int Q_id;
    vector<int> one_way_neighbor_id; //存邻居节点id
    vector<int> two_way_neighbor_id;  //双向邻居节点

    QNode();

    void save_Q_node_info(int local_Q_id,int local_u_id,int local_u_label);   //储存节点信息
    void add_Q_neighbor(int local_u_id);   //添加邻居节点
};



//*****************************************************************
//边对节点
class EdgePairNode{
public:
    int Q_id;      //即：这条边属于哪个Q
    int first_node_in_degree;   //边对中第一个节点的入度 <D,E>
    int second_node_out_degree; //边对中第二个节点的出度
    pair<int,int> label_pair;
    pair<int,int> id_pair;
    vector<EdgePairNode*> child;   //树中的指针

    EdgePairNode();

    void clear();
};


//*****************************************************************
//matV中的node
class GmatV_Node{
public:
    pair<int,int> vid_pair;
    pair<int,int> uid_pair;
    vector<GmatV_Node*> child;
    GmatV_Node *parent;

    GmatV_Node();
};




