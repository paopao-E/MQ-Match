#pragma once
using namespace std;
#include <string>
#include <vector>
#include <queue>
#include <algorithm>

#ifdef WIN32
#include <windows.h>  
#include <psapi.h>  
//#include <tlhelp32.h>
#include <direct.h>
#include <process.h>
#else
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/time.h>
#include <unistd.h>
#endif

string datagraphFile;
string insertionFile;
string querygraphFile;
string insertion_num_need;
int number;
int cnt_node = 0;
int cnt_node_query = 0;
int count_edge_number = 0;
int count_query_edge_number = 0;
int MAX_query_node= 5000;
int current_level = 1;
int match_result = 0;
int non_tree;
int * nodes_label;
int * nodes_label_query;
int* nodes_label_query_c;
int* label_id_c;
int * label_id;
//const int MAX_DATA_EDGE = 30000000;
vector<vector<int> > non_tree_check;//判断邻节点
vector<vector<int>>nodes;
vector<vector<int>>CAND;
vector<vector<int>>BFS_parent;
vector<vector<int>>BFS_child;
vector<vector<int>>query_nodes_c;
vector<int>ORDER1;
vector<int>ORDER2;

int total_result = 0;

int match_num = 0;



vector<vector<vector<int>>>query_edge_id;
bool output = false;
vector<vector<vector<vector<int>>>>stree_or_match;//match or stree
vector<vector<int>>key_index;
vector<vector<int>>query_nodes;
//vector<vector<int>>is_path;
//vector<vector<int>>is_stree;
//vector<vector<int>>is_match;


//map<string, int>edge_label1;
//map<string, int>edge_label1_query;
//map<string, int>edge_direction1_query;
vector<int>del;
//vector<pair<string, int> > edge_label1;
//pair<string, pair<int,int>>edge_label_direction;
const int MAX_QUERY_NODE = 1000;
int MAX_DEGREE_QUERY = 0;
int bin_query[MAX_QUERY_NODE];
int pos_query[MAX_QUERY_NODE];
int vert_query[MAX_QUERY_NODE];
int core_number_query[MAX_QUERY_NODE];
int match_cur = 0;
int simulation_sequence_index = 0;
int cnt_node_query_c = 0;
int check[50000]; //need memset set before use
bool isTree;
char visited_for_query[MAX_QUERY_NODE]; //need memset set before use
vector < pair<int, int> > level_index;
int BFS_level_query[MAX_QUERY_NODE];
int BFS_parent_query[MAX_QUERY_NODE];
vector<vector<vector<pair<int, int>>>>N_stree;
vector<vector<vector<pair<int, int>>>>N_match;
//vector<vector<vector<pair<int, int>>>>N_match;
queue<pair<int, int>> Q_UP;
queue<pair<int, int>> Q_UP_location;
queue<pair<int, int>> Q_DOWN;
queue<pair<int, int>> Q_DOWN_location;

int stop_update = -1;

int parent_stree; int parent_match;
int child_stree; int child_match;
int query_num;


//pair<int, int>  N_stree[MAX_QUERY_NODE][];
//pair<int, int>  N_match[MAX_QUERY_NODE][];

int query_nodes_array_info[MAX_QUERY_NODE + 1];
vector <int> query_nodes_array;
//int core_number_query[MAX_QUERY_NODE];

int root_node_id;		// the selected root node of current query graph
vector <int> core_tree_node_child_array;
vector <int> core_tree_node_nte_array;
int core_tree_node_child_array_index = 0;
int core_tree_node_nte_array_index = 0;
int exploreCRSequence_indx = 0;
int visited_int_for_query[MAX_QUERY_NODE];
int visited_int_for_query1[MAX_QUERY_NODE];
float weight_for_query[MAX_QUERY_NODE];
int queue_array_query[MAX_QUERY_NODE];
int simulation_sequence[MAX_QUERY_NODE];
char * visited_query1 = visited_for_query;
char * visited_query2 = visited_for_query;
int exploreCRSequence[MAX_QUERY_NODE]; // this is exclusively for the cross level nte verification in the CR exploration process
inline unsigned int split(const string &txt, vector<string> &strs, char ch) {
	//this is the general case
	size_t pos = txt.find(ch);
	size_t initialPos = 0;
	strs.clear();
	// Decompose statement
	while (pos != string::npos) {
		strs.push_back(txt.substr(initialPos, pos - initialPos + 1));
		initialPos = pos + 1;
		pos = txt.find(ch, initialPos);
	}
	// Add the last one
	strs.push_back(txt.substr(initialPos, min(pos, txt.size()) - initialPos + 1));
	//return the size of the vector
	return strs.size();
}

struct core_query_tree_node {
	int parent_node = -1;
	pair<int, int> children;  // start_pos, length
	pair<int, int> nte;		  // start_pos, length
	vector<int> * cross_lvl_nte;
};

core_query_tree_node core_query_tree[MAX_QUERY_NODE];


inline void resetTreeNodes() {
	for (int i = 0; i < cnt_node_query; i++) {
		core_query_tree_node & c = core_query_tree[i];
		c.parent_node = -1;
		c.children = make_pair(0, 0);
		c.nte = make_pair(0, 0);
		c.cross_lvl_nte = NULL;
	}
}

inline void initializeTreeNode(core_query_tree_node & treeNode, int parent_node) {
	treeNode.parent_node = parent_node;
	treeNode.children = make_pair(0, 0);
	treeNode.nte = make_pair(0, 0);
	treeNode.cross_lvl_nte = NULL;
}

inline void addNonTreeEdgeToTreeNode(core_query_tree_node & treeNode, int otherEnd) {
	core_tree_node_nte_array[core_tree_node_nte_array_index++] =otherEnd;
	if (treeNode.nte.second == 0)
		treeNode.nte = make_pair(core_tree_node_nte_array_index - 1, 1);
	else
		treeNode.nte.second++;
}




bool verify_query(int a, int b, int c) {
	for (int i = 0; i < query_nodes[a].size(); i++)
	{
		if (query_nodes[a][i] == b)
		{
			for (int j = 0; j < query_edge_id[a][i].size(); j++)
			{
				if (query_edge_id[a][i][j] == c)
				{
					return true;
				}
			}
		}
	}
	return false;
}
bool query_parent_verify(int a, int b) {
	for (int i = 0; i < BFS_parent[a].size(); i++)
	{
		int parent = BFS_parent[a][i];
		for (int j = 0; j < query_nodes[a].size(); j++)
		{
			if (query_nodes[a][j]==parent)
			{
				for (int k = 0; k < query_edge_id[a][j].size(); k++)
				{
					if (query_edge_id[a][j][k]==b)
					{
						return false;
					}
				}
			}
		}
	}
	return true;
}



inline void addCrossLevelNTEToTreeNode(core_query_tree_node & treeNode, int otherEnd) {
	if (treeNode.cross_lvl_nte == NULL)
		treeNode.cross_lvl_nte = new vector <int>;
	treeNode.cross_lvl_nte->push_back(otherEnd);
}


inline void addChildToTreeNode(core_query_tree_node & treeNode, int child) {
	core_tree_node_child_array[core_tree_node_child_array_index++] = child;
	if (treeNode.children.second == 0)
		treeNode.children = make_pair(core_tree_node_child_array_index - 1, 1);
	else
		treeNode.children.second++;
}

struct NodeIndexUnit {
	long path_contained_sum;
	long * path; //the path number corresponding to each candidate每一条路径的候选集的数目
	int size; // the size for both path and candidates
	int * candidates; //candidate set
	int * * index = NULL; //the position based index
	int * size_of_index = NULL; // the size of each position based index element. The length of this array equals to its parent node candidate number
	int parent_cand_size;
};

NodeIndexUnit indexSet[MAX_QUERY_NODE];
bool repeat(vector<int> &v) {
	sort(v.begin(), v.end());
	vector<int>::iterator end_unique = unique(v.begin(), v.end());
	vector<int>vec;
	vector<int>::iterator start = v.begin();
	while (start != end_unique)
		vec.push_back(*start++);
	if (vec.size() != v.size())
		return true;

}