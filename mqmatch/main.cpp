#include <chrono>
#include <string>
#include <vector>
#include <iostream>
#include "graph.h"
#include "match.h"
#include <time.h>
#include <string.h>
#include <cstring>
 #define Get_Time() std::chrono::high_resolution_clock::now()
#define Duration(start) std::chrono::duration_cast<\
    std::chrono::microseconds>(Get_Time() - start).count()/(float)1000
#define Print_Time(str, start) std::cout << str << Duration(start) << " "<< Duration(start)/1000 << std::endl;
int main(int argc, char **argv) {
    std::chrono::high_resolution_clock::time_point start, start_index;
    string q_path, d_path, u_path;
    q_path = "/root/autodl-tmp/lxj/data/QueryGraphs/HepPh/lb10/q_20/dense/ol_0.4/er_0.4/single/Q_51";
    d_path = "/root/autodl-tmp/lxj/data/DataGraphs/cit-HepPh/ins_0.2/data.graph";
    u_path = "/root/autodl-tmp/lxj/data/DataGraphs/cit-HepPh/ins_0.2/insertion.graph";
    for (int i = 1; i < argc; i++) {
         if (string(argv[i]) == "-q")
            q_path = argv[i + 1];
        else if (string(argv[i]) == "-d")
            d_path = argv[i + 1];
        else if (string(argv[i]) == "-u")
            u_path = argv[i + 1];
    }
    printf("argc=%d\n", argc);
    for (int i = 0; i < argc; ++i){
        printf("argv[%d]=%s\n", i, argv[i]);
    }
    Graph graph{};
    graph.inputQ(q_path);
    graph.inputG(d_path);
    graph.inputUpdate(u_path);
    graph.genOrder();
    start_index = Get_Time();
    graph.constructIndex();
    Print_Time("processing time of initial index (ms s): ", start_index);
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
    start = Get_Time();    

    match.updateAndMatch();
    Print_Time("Incremental Matching (ms s): ", start);
    cout << "Index Maintenance Time: " << match.getTime_indexUpdate() << endl;
    cout << "Search Time: " << match.getTime_search() << endl;
    cout << "Partial Matches Number= " << match.getPartialMatchNum() << endl;
    cout << "----------------------end----------------------" << endl;
}
