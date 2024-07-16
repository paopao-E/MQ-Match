#include "basic_struct.h"



GNode::GNode() {
    this->v_label = -1;
    this->v_id = -1;
}

void GNode::save_G_node_info(int id, int label) {
    this->v_label = label;
    this->v_id = id;
}

void GNode::add_G_neighbor(int id) {
    this->two_way_neighbor_id.push_back(id);
}

QNode::QNode() {
    this->u_id =  this->Q_id = this->u_label =  -1 ;
}

void QNode::save_Q_node_info(int local_Q_id, int local_u_id, int local_u_label) {
    this->Q_id = local_Q_id;
    this->u_label = local_u_label;
    this->u_id = local_u_id;
}

void QNode::add_Q_neighbor(int local_u_id) {
    this->one_way_neighbor_id.push_back(local_u_id);
}

EdgePairNode::EdgePairNode() {
    this->first_node_in_degree = 0;  //默认入度为0，后面只需要管那些入度不为0的，不需要再让入度本身为0的节点再进行一次赋0操作
    this->second_node_out_degree = 0;
    this->Q_id = -1;
}

void EdgePairNode::clear() {
    this->first_node_in_degree = 0;  //默认入度为0，后面只需要管那些入度不为0的，不需要再让入度本身为0的节点再进行一次赋0操作
    this->second_node_out_degree = 0;
    this->Q_id = -1;
    this->child.clear();
}

GmatV_Node::GmatV_Node() {
    parent = nullptr;
}











