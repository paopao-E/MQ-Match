#ifndef _GRAPH_H
#define _GRAPH_H

#include <unordered_map>
#include <unordered_set>
#include <map>
#include <vector>
#include <set>
#include <string>
using namespace std;

#include <chrono>
#define Get_Time() std::chrono::high_resolution_clock::now()
#define Duration(start) std::chrono::duration_cast<\
    std::chrono::microseconds>(Get_Time() - start).count()/(float)1000


using vec = std::vector<int>;
using two_vec = std::vector<vec>;
using u_set = std::unordered_set<int>;
using u_map = std::unordered_map<int, std::set<int>>;
using umap_bool = std::unordered_map<int, bool>;
using umap_uset = std::unordered_map<int, u_set>;
using umap_twovec = std::unordered_map<int, two_vec>;
using two_u_map = std::unordered_map<int, u_map>;
using two_map = std::map<int, std::map<int, int>>;
using two_umap = std::unordered_map<int, std::unordered_map<int, int>>;
using two_umap_twovec = std::unordered_map<int, umap_twovec>;
using tri_umap_bool = std::unordered_map<int, std::unordered_map<int, umap_bool>>;
using tri_umap = std::unordered_map<int, two_umap>;
using tri_u_map = std::unordered_map<int, two_u_map>; 


class Graph
{
public:
	Graph();
	struct vertex_Q
	{
		int q_id;
		int comm_id;         
		int q_label;
		std::set<int> q_neis; 
		u_set q_neiLabels;    
		u_map q_neiSet;     
	};
	void inputQ(const std::string& path);
	two_u_map& getQAdjList();
	std::vector<vertex_Q>& getQ();
	std::unordered_map<int, std::multimap<int, vec>>& getQueryEdges();
	struct vertex_G
	{
		int label; 
		u_map nei; 
		two_u_map cand;
		u_map self_LI;
		u_map nbr_LI; 
	};
	std::vector<vertex_G> G;
	void inputG(const std::string& path);
	std::vector<vertex_G>& getG();
	vec update; 
	void inputUpdate(const std::string& path);
	vec& getStream();
	void genOrder(); 
	tri_u_map& getLabelEdgeOfCommSub();
	two_umap_twovec& getOrderOfCommSub();
	umap_twovec& getOrderOfRemainQ();
	two_umap_twovec& getOrderOfEdgeOfRemainQ();
	tri_u_map& getLabelEdgeOfRemainingQ();
	two_u_map& getRemainQ_adjList();
	two_u_map& getRemainQ_initialCandU();
	struct od_node   
	{
		int label;
		vec u_set;
	};
	std::vector<od_node>& getOdTreeNodes();
	two_u_map& getOdTreeStartNodes();
	tri_u_map& getOrderTree();
	void constructIndex();

private:
	//multi-query
	std::vector<vertex_Q> Q; 
	two_u_map Q_adjList;      
	std::unordered_map<int, std::multimap<int, vec>> q_labelEdge;
	std::map<int, vec> rangeOfQ; 
	two_u_map commSubgraph;     
	void storeCommSubgraph(int, int, int);
	tri_u_map labelEdgeOfCommSub;  
	tri_u_map labelEdgeOfRemainingQ;
	two_u_map remainQ_adjList; 
	two_u_map remainQ_initialCandU; 
	void storeLabelEdge(bool, int, int);
	void addLabelEdge(bool, int, int);
	void traverseRemainingQ(u_map);
	void cmpOrderOfCommSub();
	void cmpOrderOf_UiUj(bool, int, int, int, u_map, std::set<int>);
	vec cmpMajor(int, u_map, std::set<int>);
	vec cmpFinalMajor(vec, vec);
	two_umap_twovec OrderOfCommSub;
	void addOrderOfCommSub(bool, int, int, int);
	u_map removeMatchedNodeFromQ(int, u_map);
	u_map removeAnEdgeFromQ(int, int, u_map);
	void cmpMinorForCommSub(bool, int, int);
	u_map removeCommSubFromQ(int, int);
	int cmpDifBtw_ActualId_And_IdInCommSub(int, int);
	umap_twovec OrderOfRemainQ; 
	void cmpOrderOfRemainingQ(int, u_map, std::set<int>);
	std::multimap<int, int> cmpNextMajorFromRemainingQ(int, u_map, std::set<int>);
	void addOrderOfRemainingQ(bool, int, std::set<int>);

	struct Aux          
	{
		unordered_map<int, int> major; 
		u_map minor;
		u_map adj;
		set<int> matched;
	};
	unordered_map<int, unordered_map<int, Aux>> Auxiliary; 
	void cmpOrderOfEdgeOfRemainQ();
	two_umap_twovec OrderOfEdgeOfRemainQ;
	void updateAuxiliary_Matched(bool, int, int, int);
	void updateAuxiliary_Adj(bool, int, int, u_map);
	void cmpMajorAndMinor(int, int, int);
	void updateMajorAndMinor(int, int);
	void addMajorInAuxiliary(int, int, int);
	void addMinorInAuxiliary(int, int, int);
	void deleteMinorInAuxiliary(int, int, int);
	bool compareDegree(int, int, int);
	void generateOrderOfEdgeOfRemainQ(multimap<int, int>, multimap<int, int>);
	multimap<int, int> divideNextByLabel(string);
	multimap<int, int> sortLabelByNum(unordered_map<int, int>);
	void cmpNextForAllEdge(multimap<int, int>, multimap<int, int>);
	void addOrderOfEdgeOfRemainQ(bool, int, int, int);
	std::vector<od_node> odTree_nodes;
	two_u_map odTree_startNodes;  
	tri_u_map orderTree;
	void summaryAllOrder(int, int);
	two_u_map classfiyNodes(two_u_map, int, int, int);
	void update_odNode_uSet(u_map, int, int);
	void addOdTree_startNodes(int, int, int);
	void addOdTree_adjKey(int, int, int);
	void update_OdTree_adj(int, int, int, int);
	void combineAGroupOfOrders(int, int, u_map, int, int);
	void update_nbr_LI(int, int, int, int);
};


#endif // !_GRAPH_H
