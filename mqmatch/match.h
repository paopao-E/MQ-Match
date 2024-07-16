#ifndef _MATCH_H
#define _MATCH_H

#include <vector>
#include "graph.h"
using namespace std;
using classifier = std::unordered_map<std::string, u_map>;

class Match
{
public:
	Match(two_u_map&, std::vector<Graph::vertex_Q>&, std::unordered_map<int, std::multimap<int, vec>>&, std::vector<Graph::vertex_G>&, vec&, tri_u_map&, tri_u_map&, two_u_map&, two_u_map&, two_umap_twovec&, umap_twovec&, two_umap_twovec&, std::vector<Graph::od_node>&, two_u_map&, tri_u_map&);
	void updateAndMatch();

private:
	two_u_map Q_adjList;
	std::vector<Graph::vertex_Q> q;
	std::unordered_map<int, std::multimap<int, vec>> q_edges;
	std::vector<Graph::vertex_G> initG;
	vec update; 
	tri_u_map CommSub;
	tri_u_map RemainQ;
	two_u_map RemainQ_adjList;
	two_u_map RemainQ_initialCandU;
	two_umap_twovec orderCommSub;
	umap_twovec orderRemainQ;
	two_umap_twovec orderEdgeOfRemainQ;
	std::vector<Graph::od_node> orderTree_node;
	two_u_map orderTree_bgNode;
	tri_u_map orderTree;
	bool checkInputEdge(int, int);
	void updateIndex(int, int);
	void store_Changed_selfLI(int, int, int);
	two_u_map changed_selfLI; 
	std::unordered_map<int, std::unordered_map<int, vec>> stream_matched_edges; 
	void add_G_Nei(int, int);
	void addEdge_UpdateCandAndSelfLI(int, int);
	void add_CandAndSelfLI(int, int, int, int);
	void addEdge_UpdateNbrLI_v_and_nbrs(int, int);
	void addEdge_UpdateNbrLI_vi_and_vj(int, int);
	void add_nbrLI(int, int, int);
	void del_G_Nei(int, int);
	void delEdge_UpdateCandAndSelfLI(int, int);
	void del_CandaAndSelfLI(int, int, int, int);
	void delEdge_update_nbrLI_v_and_nbrs(int, int);
	void delEdge_update_nbrLI_vi_and_vj(int, int);
	void del_nbrLI(int, int, int);
	struct Aux
	{
		vector<std::unordered_map<int, int>> match;
		vector<u_map> candidate;
		vector<std::set<int>> used_v;
		vec comm_q_id;  
	};
	unordered_map<int, unordered_map<int, Aux>> Auxiliary; 
	int numQForAComm;
	int numNodesForAQ;
	int searchMatch(int, int);
	bool checkEdgeInCommSub(int, int);
	bool checkEdgeInOrderTree(int, int);
	int searchMatchedEdgeFromCommon(int, int); 
	int countCommSubMatch(int, int, int, int, u_map, unordered_map<int, int>, set<int>, int dif);
	void addAuxiliary(int, int, int, std::unordered_map<int, int> copy_match, u_map, std::set<int>);
	int countJoinedMatch(int, int, int, u_map, unordered_map<int, int>, set<int>, int);
	int searchMatchedEdgeFromRemainQ(int, int);
	int countRemainQMatch(int, int, int, int, u_map, unordered_map<int, int>, set<int>);
	u_map cmpInitCand(int, int, int, int);
	bool checkBidirectionalEdgeOnIndex(int, int, int, int);
	u_map genEdgeCand(int, int, int, int, u_map);
	u_map updateCand(bool, int, int, u_map, std::unordered_map<int, int>, std::set<int>, int);	
	u_map cmpJoinedInitCand(int, std::unordered_map<int, int>, std::set<int>, int);
};


#endif // !_MATCH_H
