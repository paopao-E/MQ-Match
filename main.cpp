#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include"graph.h"
#include "match.h"
#include <time.h>
#include <string.h>
#include <cstring>
#include <sys/resource.h>

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

#define VMRSS_LINE 22

using namespace std;

int main(int argc, char** argv) {

    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-q")
            q_path = argv[i + 1];
        else if (string(argv[i]) == "-d")
            d_path = argv[i + 1];
        else if (string(argv[i]) == "-u")
            u_path = argv[i + 1];
    }
    Graph graph{};
    graph.inputQ(q_path);
    graph.inputG(d_path);
    graph.inputUpdate(u_path);
    graph.genOrder();
    graph.constructIndex();

    std::vector<Graph::vertex_Q>& qg = graph.getQ();
    two_u_map& Q_adjList = graph.getQAdjList();
    std::unordered_map<int, std::multimap<int, vec>>& qg_edges = graph.getQueryEdges();
    std::vector<Graph::vertex_G>& initialG = graph.getG();
    vec& updateStream = graph.getStream();
    tri_u_map& commSub = graph.getLabelEdgeOfCommSub();
    tri_u_map& remainQ = graph.getLabelEdgeOfRemainingQ();
    two_umap_twovec& od_commSub = graph.getOrderOfCommSub();
    umap_twovec& od_remainQ = graph.getOrderOfRemainQ();
    two_umap_twovec& od_edgeOfRemainQ = graph.getOrderOfEdgeOfRemainQ();
    two_u_map& remainQ_adjList = graph.getRemainQ_adjList();
    two_u_map& remainQ_initialCandU = graph.getRemainQ_initialCandU();
    std::vector<Graph::od_node>& ot_node = graph.getOdTreeNodes();
    two_u_map& ot_bgNode = graph.getOdTreeStartNodes();
    tri_u_map& ot = graph.getOrderTree();
    Match match = Match(Q_adjList, qg, qg_edges, initialG, updateStream, commSub, remainQ, remainQ_adjList, remainQ_initialCandU, od_commSub, od_remainQ, od_edgeOfRemainQ, ot_node, ot_bgNode, ot);
    match.updateAndMatch();
}

