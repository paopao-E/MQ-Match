#include "match.h"
#include <iostream>
#include <algorithm>

#include <chrono>
#define Get_Time() std::chrono::high_resolution_clock::now()
#define Duration(start) std::chrono::duration_cast<\
    std::chrono::microseconds>(Get_Time() - start).count()/(float)1000


Match::Match(two_u_map& qAdjList, std::vector<Graph::vertex_Q>& qg, std::unordered_map<int, std::multimap<int, vec>>& qg_edges, std::vector<Graph::vertex_G>& initialG, vec& stream, tri_u_map& commSub, tri_u_map& remainQ, two_u_map& remainQ_adjList, two_u_map& remainQ_initialCandU, two_umap_twovec& od_commSub, umap_twovec& od_remainQ, two_umap_twovec& od_edgeOfRemainQ, std::vector<Graph::od_node>& ot_node, two_u_map& ot_bgNode, tri_u_map& ot)
{
	Q_adjList = qAdjList;
	q = qg;
	q_edges = qg_edges;
	initG = initialG;
	update = stream;
	CommSub = commSub;
	RemainQ = remainQ; 
	RemainQ_adjList = remainQ_adjList;
	RemainQ_initialCandU = remainQ_initialCandU;
	orderCommSub = od_commSub;
	orderRemainQ = od_remainQ;
	orderEdgeOfRemainQ = od_edgeOfRemainQ;
	orderTree_node = ot_node;
	orderTree_bgNode = ot_bgNode;
	orderTree = ot;
	numQForAComm = Q_adjList.size() / (q.end() - 1)->comm_id;
	numNodesForAQ = Q_adjList.begin()->second.size();
	
	double time_slot1 = 0.0, time_slot2 = 0.0;    

}


void Match::updateAndMatch()
{
	std::chrono::high_resolution_clock::time_point time1, time2;

	long long add_match = 0;
	long long del_match = 0;
	long int thr = update.size();

	for (int t = 0; t < thr; t += 2) {
		int v1 = update[t];
		int v2 = update[t + 1];

  		if (v1 < 0 || v2 < 0) {

  			bool skip = checkInputEdge(-v1, -v2);
			if (skip)
			{
				del_match += searchMatch(-v1, -v2);
				updateIndex(v1, v2);
			}
		}

  		else {
  			bool skip = checkInputEdge(v1, v2);
			if (skip)
			{
				time1= Get_Time();
				updateIndex(v1, v2);
				time_slot1 += Duration(time1);

				time2 = Get_Time();
				add_match += searchMatch(v1, v2);
				time_slot2 += Duration(time2);
			}
		}

		changed_selfLI.clear();
		stream_matched_edges.clear();
	}

	std::cout << "positive matches: " << add_match << std::endl;
	std::cout << "negative matches: " << del_match << std::endl;
	print_time();
    int num = 0;
    for (int vi = 0; vi < initG.size(); ++vi){
        for (auto& nbrli : initG[vi].nbr_LI){
            num += nbrli.second.size();
        }
    }
    cout << "========= nbr LI : " << num << " ===========" << endl;
}

double Match::getTime_indexUpdate()
{
	return time_slot1;
}

double Match::getTime_search()
{
	return time_slot2;
}

int Match::getPartialMatchNum()
{
	return partialMatch_num;
}

bool Match::checkInputEdge(int v1, int v2)
{     

	if (initG[v1].label == -1 || initG[v2].label == -1) 
	{
		return false;
	}
	if (q_edges.at(initG[v1].label).find(initG[v2].label) == q_edges.at(initG[v1].label).end())
	{
		return false;
	}
	return true;
}

  void Match::updateIndex(int v1, int v2)
{
	std::chrono::high_resolution_clock::time_point time1, time2, time3;

	if (v1 < 0 || v2 < 0)
	{
		del_G_Nei(-v1, -v2);
		del_G_Nei(-v2, -v1);
		delEdge_UpdateCandAndSelfLI(-v1, -v2);
		delEdge_update_nbrLI_v_and_nbrs(-v1, -v2);
		delEdge_update_nbrLI_v_and_nbrs(-v2, -v1);
		delEdge_update_nbrLI_vi_and_vj(-v1, -v2);
	}
	else
	{
		time1= Get_Time();
		add_G_Nei(v1, v2);
		add_G_Nei(v2, v1);
		time_slot1_1 += Duration(time1);

		time2 = Get_Time();
		addEdge_UpdateCandAndSelfLI(v1, v2);
		addEdge_UpdateCandAndSelfLI(v2, v1);
		time_slot1_2 += Duration(time2);

		time3 = Get_Time();
		addEdge_UpdateNbrLI_v_and_nbrs(v1, v2);
		addEdge_UpdateNbrLI_v_and_nbrs(v2, v1);
		addEdge_UpdateNbrLI_vi_and_vj(v1, v2);
		time_slot1_3 += Duration(time3);

  	}
}

void Match::store_Changed_selfLI(int v, int Qi, int u)
{
	if (changed_selfLI.at(v).find(Qi) != changed_selfLI.at(v).end())
	{
		changed_selfLI.at(v).at(Qi).emplace(u);
	}
	else
	{
		set<int> temp;
		temp.emplace(u);
		changed_selfLI.at(v).emplace(Qi, temp);
	}
}

  void Match::add_G_Nei(int v, int v_nbr)
{
	if (initG[v].nei.find(initG[v_nbr].label) != initG[v].nei.end())
	{
		initG[v].nei.at(initG[v_nbr].label).emplace(v_nbr);
	}
	else
	{
		set<int> temp;
		temp.emplace(v_nbr);
		initG[v].nei.emplace(initG[v_nbr].label, temp);
	}
}

void Match::addEdge_UpdateCandAndSelfLI(int v, int v_nbr)
{
	u_map temp;
	changed_selfLI.emplace(v, temp);
	multimap<int, vec>::iterator it1 = q_edges[initG[v].label].lower_bound(initG[v_nbr].label);
	multimap<int, vec>::iterator it2 = q_edges[initG[v].label].upper_bound(initG[v_nbr].label);
	for (; it1 != it2; it1++)
	{
		vec val = it1->second;
		long int thr = val.size();
		for (auto i = 1; i < thr; i++)      

		{
			int u = val[i];    

			int v_nbr_label = initG[v_nbr].label;
			if (initG[v].nei.at(v_nbr_label).size() == q[u].q_neiSet.at(v_nbr_label).size())
			{    

				add_CandAndSelfLI(v, val[0], u, v_nbr_label);
			}
		}
	}
}

void Match::add_CandAndSelfLI(int v, int Qi, int u, int vNbrLabel){
	if (initG[v].cand.find(Qi) != initG[v].cand.end()){
		if (initG[v].cand.at(Qi).find(u) != initG[v].cand.at(Qi).end()){
			initG[v].cand.at(Qi).at(u).emplace(vNbrLabel);
		}
		else{
			set<int> temp;
			temp.emplace(vNbrLabel);
			initG[v].cand.at(Qi).emplace(u, temp);
		}
	}
	else{
		set<int> temp;
		temp.emplace(vNbrLabel);
		u_map temp2;
		temp2.emplace(u, temp);
		initG[v].cand.emplace(Qi, temp2);
	}

	if (initG[v].cand.at(Qi).at(u).size() == q[u].q_neiLabels.size()){
  		store_Changed_selfLI(v, Qi, u);
		if (initG[v].self_LI.find(Qi) != initG[v].self_LI.end()){
			initG[v].self_LI.at(Qi).emplace(u);
		}
		else{
			set<int> temp;
			temp.emplace(u);
			initG[v].self_LI.emplace(Qi, temp);
		}
	}
}

void Match::addEdge_UpdateNbrLI_v_and_nbrs(int v, int v_nbr)
{
  	for (auto& i : changed_selfLI.at(v))    

	{
		for (auto& u : i.second)
		{
			for (auto& uNbr : q[u].q_neiSet)
			{
				int label = uNbr.first;
				set<int> uNbrSet = uNbr.second;
				set<int> vnbr_set = initG[v].nei.at(label);
				vnbr_set.erase(v_nbr);    

				for (auto& vNbr : vnbr_set)
				{
					if (initG[vNbr].self_LI.find(i.first) != initG[vNbr].self_LI.end())
					{
						set<int> vNbr_self = initG[vNbr].self_LI.at(i.first);
						vec vNbr_match_uNbr;
						set_intersection(uNbrSet.begin(), uNbrSet.end(), vNbr_self.begin(), vNbr_self.end(), back_inserter(vNbr_match_uNbr));
						if (!vNbr_match_uNbr.empty())
						{
							for (auto& matched_uNbr : vNbr_match_uNbr)
							{
								add_nbrLI(v, matched_uNbr, vNbr);  

								add_nbrLI(vNbr, u, v);             

							}
						}
					}
				}
			}
		}
	}
}

void Match::addEdge_UpdateNbrLI_vi_and_vj(int v, int v_nbr)
{
	int vnbr_label = initG[v_nbr].label;
	for (auto& i : initG[v].self_LI)
	{
		if (initG[v_nbr].self_LI.find(i.first) != initG[v_nbr].self_LI.end())
		{
			set<int> vNbr_self = initG[v_nbr].self_LI.at(i.first);
			unordered_map<int, vec> temp;
			stream_matched_edges.emplace(i.first, temp);
			for (auto& u : i.second)
			{
				if (q[u].q_neiSet.find(vnbr_label) != q[u].q_neiSet.end())
				{
					set<int> u_vNbrL_nbrs = q[u].q_neiSet.at(vnbr_label);
					vec vNbr_match_uNbr;
					set_intersection(u_vNbrL_nbrs.begin(), u_vNbrL_nbrs.end(), vNbr_self.begin(), vNbr_self.end(), back_inserter(vNbr_match_uNbr));
					if (!vNbr_match_uNbr.empty())
					{
						stream_matched_edges.at(i.first).emplace(u, vNbr_match_uNbr);
						for (auto& matched_uNbr : vNbr_match_uNbr)
						{
							add_nbrLI(v, matched_uNbr, v_nbr);  

							add_nbrLI(v_nbr, u, v);             

						}
					}
				}
			}

			if (stream_matched_edges.at(i.first).size() == 0)
			{
				stream_matched_edges.erase(i.first);
			}
		}
	}
}

void Match::add_nbrLI(int v, int u_nbr, int v_nbr)
{
	if (initG[v].nbr_LI.find(u_nbr) != initG[v].nbr_LI.end())
	{
		initG[v].nbr_LI.at(u_nbr).emplace(v_nbr);
	}
	else
	{
		set<int> temp;
		temp.emplace(v_nbr);
		initG[v].nbr_LI.emplace(u_nbr, temp);
	}
}

  void Match::del_G_Nei(int v, int v_nbr)
{
	initG[v].nei.at(initG[v_nbr].label).erase(v_nbr);
	if (initG[v].nei.at(initG[v_nbr].label).size() == 0)
	{
		initG[v].nei.erase(initG[v_nbr].label);
	}
}

void Match::delEdge_UpdateCandAndSelfLI(int vi, int vj)
{
  	u_map temp;
	changed_selfLI.emplace(vi, temp);
	changed_selfLI.emplace(vj, temp);

  	if (initG[vi].nei.find(initG[vj].label) != initG[vi].nei.end())
	{
		for (auto& i : stream_matched_edges)
		{
			for (auto& edge : i.second)
			{
				if (q[edge.first].q_neiSet.at(initG[vj].label).size() - initG[vi].nei.at(initG[vj].label).size() == 1)
				{    

					del_CandaAndSelfLI(vi, i.first, edge.first, initG[vj].label);
				}
			}
		}
	}
	else
	{
		for (auto& i : stream_matched_edges)
		{
			for (auto& edge : i.second)
			{
				if (q[edge.first].q_neiSet.at(initG[vj].label).size() == 1)
				{
					del_CandaAndSelfLI(vi, i.first, edge.first, initG[vj].label);
				}
			}
		}
	}

  	if (initG[vj].nei.find(initG[vi].label) != initG[vj].nei.end())
	{
		for (auto& i : stream_matched_edges)
		{
			for (auto& edge : i.second)
			{
				for (auto& uj : edge.second)
				{
					if (q[uj].q_neiSet.at(initG[vi].label).size() - initG[vj].nei.at(initG[vi].label).size() == 1)
					{    

						del_CandaAndSelfLI(vj, i.first, uj, initG[vi].label);
					}
				}
			}
		}
	}
	else
	{
		for (auto& i : stream_matched_edges)
		{
			for (auto& edge : i.second)
			{
				for (auto& uj : edge.second)
				{
					if (q[uj].q_neiSet.at(initG[vi].label).size() == 1)
					{
						del_CandaAndSelfLI(vj, i.first, uj, initG[vi].label);
					}
				}
			}
		}
	}


  }

void Match::del_CandaAndSelfLI(int v, int Qi, int u, int vNbrLabel)
{
	if (initG[v].cand.at(Qi).find(u) != initG[v].cand.at(Qi).end())
	{
		initG[v].cand.at(Qi).at(u).erase(vNbrLabel);
		if (initG[v].cand.at(Qi).at(u).size() == 0)
		{
			initG[v].cand.at(Qi).erase(u);
			if (initG[v].cand.at(Qi).size() == 0) {
				initG[v].cand.erase(Qi);
			}
		}
	}

	if (initG[v].self_LI.find(Qi) != initG[v].self_LI.end())
	{
		if (initG[v].self_LI.at(Qi).find(u) != initG[v].self_LI.at(Qi).end())
		{     

			store_Changed_selfLI(v, Qi, u);
			initG[v].self_LI.at(Qi).erase(u);
			if (initG[v].self_LI.at(Qi).size() == 0)
			{
				initG[v].self_LI.erase(Qi);
			}
		}
	}
}

void Match::delEdge_update_nbrLI_v_and_nbrs(int v, int v_nbr)
{
  	for (auto& i : changed_selfLI.at(v))
	{
		u_set processed_uNbrs;
		u_set delNbr_of_u;

		for (auto& u : i.second)    

		{
			for (auto& uNbr : q[u].q_neis)
			{
				if (initG[v].nbr_LI.find(uNbr) != initG[v].nbr_LI.end())
				{
					for (auto& vv : initG[v].nbr_LI.at(uNbr))
					{
						del_nbrLI(vv, u, v);     

					}
				}

				if (processed_uNbrs.find(uNbr) == processed_uNbrs.end())
				{
					processed_uNbrs.emplace(uNbr);

					if (initG[v].self_LI.find(i.first) != initG[v].self_LI.end())
					{
						vec comm;
						set_intersection(q[uNbr].q_neiSet.at(initG[v].label).begin(), q[uNbr].q_neiSet.at(initG[v].label).end(), initG[v].self_LI.at(i.first).begin(), initG[v].self_LI.at(i.first).end(), back_inserter(comm));
						if (comm.empty())
						{
							delNbr_of_u.emplace(uNbr);    

						}
					}
					else
					{
						delNbr_of_u.emplace(uNbr);    

					}
				}
			}
		}

		for (auto& dn : delNbr_of_u)
		{
			initG[v].nbr_LI.erase(dn);
		}
	}
}

void Match::delEdge_update_nbrLI_vi_and_vj(int vi, int vj)
{
	for (auto& i : stream_matched_edges)
	{
		for (auto& edge : i.second)    

		{
			del_nbrLI(vj, edge.first, vi);

			for (auto& uj : edge.second)
			{
				del_nbrLI(vi, uj, vj);
			}
		}
	}
}

void Match::del_nbrLI(int v, int u_nbr, int v_nbr)
{
	if (initG[v].nbr_LI.find(u_nbr) != initG[v].nbr_LI.end())
	{
		initG[v].nbr_LI.at(u_nbr).erase(v_nbr);
		if (initG[v].nbr_LI.at(u_nbr).size() == 0)
		{
			initG[v].nbr_LI.erase(u_nbr);
		}
	}
}


  int Match::searchMatch(int vi, int vj)
{
	int update_result = 0;
	if (initG[vi].label < initG[vj].label){

		bool flag = checkEdgeInCommSub(vi, vj); 
		if (flag)
		{
  		}
		bool flag2 = checkEdgeInOrderTree(vi, vj);
		if (flag2)
		{
			update_result += searchMatchedEdgeFromRemainQ(vi, vj);
		}
	}
	else if (initG[vi].label == initG[vj].label)
	{
		bool flag = checkEdgeInCommSub(vi, vj);
		if (flag)
		{
  		}
		bool flag2 = checkEdgeInOrderTree(vi, vj);
		if (flag2)
		{
  		}
	}
	else
	{
		bool flag = checkEdgeInCommSub(vj, vi);
		if (flag)
		{
  		}
		bool flag2 = checkEdgeInOrderTree(vj, vi);
		if (flag2)
		{
			update_result += searchMatchedEdgeFromRemainQ(vj, vi);
		}
	}
	return update_result;
}

bool Match::checkEdgeInCommSub(int vi, int vj)
{
	if (CommSub.find(initG[vi].label) != CommSub.end()) 
	{
		if (CommSub.at(initG[vi].label).find(initG[vj].label) != CommSub.at(initG[vi].label).end()) 
		{      

			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool Match::checkEdgeInOrderTree(int vi, int vj)
{
	if (orderTree.find(initG[vi].label) != orderTree.end())
	{
		if (orderTree.at(initG[vi].label).find(initG[vj].label) != orderTree.at(initG[vi].label).end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

  int Match::searchMatchedEdgeFromCommon(int vi, int vj){
    int result = 0;
    std::set<int> used_v;
    used_v.emplace(vi);
    used_v.emplace(vj);

    for (auto& i : CommSub.at(initG[vi].label).at(initG[vj].label)){
        for (auto& j : i.second){       

            bool flag = checkBidirectionalEdgeOnIndex(vi, vj, i.first, j);
            if (flag){
                u_map cand = cmpInitCand(vi, vj, i.first, j);
                if (!cand.empty()){
                    std::unordered_map<int, int> m;
                    m.emplace(i.first, vi);
                    m.emplace(j, vj);
                    int sum = orderCommSub.at(i.first).at(j).size();
                    result += countCommSubMatch(i.first, j, 0, sum, cand, m, used_v, 0);
                  }
            }
              int bg_q_id = (q[i.first].comm_id - 1) * numQForAComm;
            int ed_q_id = bg_q_id + numQForAComm;
            for (int k = bg_q_id + 1; k < ed_q_id; k++)
            {
                int dif = (k - bg_q_id) * numNodesForAQ;
                bool check = checkBidirectionalEdgeOnIndex(vi, vj, i.first + dif, j + dif);
                if (check)
                {
                    u_map cand = cmpInitCand(vi, vj, i.first + dif, j + dif);
                    if (!cand.empty())
                    {
                        std::unordered_map<int, int> m;
                        m.emplace(i.first + dif, vi);
                        m.emplace(j + dif, vj);
                        int sum = orderCommSub.at(i.first).at(j).size();
                        result += countCommSubMatch(i.first, j, 0, sum, cand, m, used_v, dif);
                        break;
                    }
                }
            }
        }
    }

      if (Auxiliary.find(vi) != Auxiliary.end()){
        if (Auxiliary.at(vi).find(vj) != Auxiliary.at(vi).end()){
            int end_size = Auxiliary.at(vi).at(vj).candidate.size();
            for (int i = 0; i < end_size; i++){
                u_map cand = Auxiliary.at(vi).at(vj).candidate.at(i);
                set<int> used_v = Auxiliary.at(vi).at(vj).used_v.at(i);
                unordered_map<int, int> m = Auxiliary.at(vi).at(vj).match.at(i);
                int bg_q_id = (q[cand.begin()->first].comm_id - 1) * numQForAComm;
                int ed_q_id = bg_q_id + numQForAComm;
                int sum = orderRemainQ.at(bg_q_id).size();
                if (Auxiliary.at(vi).at(vj).comm_q_id.at(i) == bg_q_id){     

                    result += countJoinedMatch(bg_q_id, 0, sum, cand, m, used_v, 0);
                    for (int j = bg_q_id + 1; j < ed_q_id; j++){
                        int dif = (j - bg_q_id) * numNodesForAQ;
                        u_map other_cand = cmpJoinedInitCand(j, m, used_v, dif);
                        if (!other_cand.empty()){
                            result += countJoinedMatch(j, 0, sum, other_cand, m, used_v, dif);
                        }
                    }
                }
                else{
                    for (int j = Auxiliary.at(vi).at(vj).comm_q_id.at(i); j < ed_q_id; j++){
                        int dif = (j - Auxiliary.at(vi).at(vj).comm_q_id.at(i)) *numNodesForAQ;
                        u_map other_cand = cmpJoinedInitCand(j, m, used_v, dif);
                        if (!other_cand.empty()){
                            result += countJoinedMatch(j, 0, sum, other_cand, m, used_v, dif);
                        }
                    }
                }
            }
        }
    }
    return result;
}

int Match::countCommSubMatch(int ui, int uj, int index_u, int sum, u_map cand, unordered_map<int, int> match, set<int> used_v, int dif)
{
    int result = 0;
    if (index_u < sum - 1)
    {
        int u = orderCommSub.at(ui).at(uj)[index_u][0] + dif;
        if (cand.find(u) != cand.end())
        {
            for (auto& v : cand.at(u))
            {
                std::unordered_map<int, int> copy_match = match;
                copy_match.emplace(u, v);
                u_map copy_cand = cand;
                copy_cand.erase(u);
                std::set<int> copy_usedV = used_v;
                copy_usedV.emplace(v);
                copy_cand = updateCand(false, v, u, copy_cand, copy_match, copy_usedV, dif);
                int copy_index_u = index_u;
                if (!copy_cand.empty())
                {
                    copy_index_u += 1;
                    result += countCommSubMatch(ui, uj, copy_index_u, sum, copy_cand, copy_match, copy_usedV, dif);
                }
                else
                {
                    partialMatch_num += 1;
                }
            }
        }
    }
    else if (index_u == sum - 1)
    {
        int u = orderCommSub.at(ui).at(uj)[index_u][0] + dif;
        if (cand.find(u) != cand.end())
        {
            for (auto& v : cand.at(u))
            {
                std::unordered_map<int, int> copy_match = match;
                copy_match.emplace(u, v);
                u_map copy_cand = cand;
                copy_cand.erase(u);
                std::set<int> copy_usedV = used_v;
                copy_usedV.emplace(v);
                copy_cand = updateCand(false, v, u, copy_cand, copy_match, copy_usedV, dif);
                if (!copy_cand.empty())
                {
                    addAuxiliary(q[u].q_id, copy_match.at(ui+dif), copy_match.at(uj+dif), copy_match, copy_cand, copy_usedV);
                }
                else
                {
                    if (copy_match.size() == numNodesForAQ)
                    {     

                        result += 1;
                      }
                    else
                    {
                        partialMatch_num += 1;
                    }
                }
            }
        }
    }

    return result;
}

void Match::addAuxiliary(int q_id, int vi, int vj, std::unordered_map<int, int> match, u_map cand, std::set<int> used_v)
{
    if (Auxiliary.find(vi) != Auxiliary.end())
    {
        if (Auxiliary.at(vi).find(vj) != Auxiliary.at(vi).end())
        {
            Auxiliary.at(vi).at(vj).match.emplace_back(match);
            Auxiliary.at(vi).at(vj).candidate.emplace_back(cand);
            Auxiliary.at(vi).at(vj).used_v.emplace_back(used_v);
            Auxiliary.at(vi).at(vj).comm_q_id.emplace_back(q_id);
        }
        else
        {
            Aux temp;
            temp.match.emplace_back(match);
            temp.candidate.emplace_back(cand);
            temp.used_v.emplace_back(used_v);
            temp.comm_q_id.emplace_back(q_id);
            Auxiliary.at(vi).emplace(vj, temp);
        }
    }
    else
    {
        Aux temp;
        temp.match.emplace_back(match);
        temp.candidate.emplace_back(cand);
        temp.used_v.emplace_back(used_v);
        temp.comm_q_id.emplace_back(q_id);
        unordered_map<int, Aux> temp2;
        temp2.emplace(vj, temp);
        Auxiliary.emplace(vi, temp2);
    }
}

int Match::countJoinedMatch(int q_id, int index_u, int sum, u_map cand, unordered_map<int, int> match, set<int> used_v, int dif)
{
    int result = 0;
    if (index_u < sum - 1)
    {
        int u = orderRemainQ.at(q_id)[index_u][0];
        if (cand.find(u) != cand.end())
        {
            for (auto& v : cand.at(u))
            {
                std::unordered_map<int, int> copy_match = match;
                copy_match.emplace(u, v);
                u_map copy_cand = cand;
                copy_cand.erase(u);
                std::set<int> copy_usedV = used_v;
                copy_usedV.emplace(v);
                copy_cand = updateCand(true, v, u, copy_cand, copy_match, copy_usedV, dif);
                int copy_index_u = index_u;
                if (!copy_cand.empty())
                {
                    copy_index_u += 1;
                    result += countJoinedMatch(q_id, copy_index_u, sum, copy_cand, copy_match, copy_usedV, dif);
                }
                else
                {
                    partialMatch_num += 1;
                }
            }
        }
    }
    else if (index_u == sum - 1)
    {
        int u = orderRemainQ.at(q_id)[index_u][0];
        if (cand.find(u) != cand.end())
        {
            vec diff;
            set_difference(cand.at(u).begin(), cand.at(u).end(), used_v.begin(), used_v.end(), back_inserter(diff));
            result += diff.size();


          }
        else
        {
            partialMatch_num += 1;
        }



    }

    return result;
}

  int Match::searchMatchedEdgeFromRemainQ(int vi, int vj)
{
	int result = 0;
	std::set<int> used_v;
	used_v.emplace(vi);
	used_v.emplace(vj);

	u_map cand;
	bool flag = false;
	std::unordered_map<int, int> match;
	for (auto& i : RemainQ.at(initG[vi].label).at(initG[vj].label))
	{
		for (auto& j : i.second)
		{
			flag = checkBidirectionalEdgeOnIndex(vi, vj, i.first, j);
			if (flag)
			{
				cand = cmpInitCand(vi, vj, i.first, j);
				match.emplace(i.first, vi);
				match.emplace(j, vj);
				break;
			}
		}
		if (flag)
		{
			break;
		}
	}

  	for (auto& i : RemainQ.at(initG[vi].label).at(initG[vj].label))
	{      

		for (auto& j : i.second)
		{
			bool flag = checkBidirectionalEdgeOnIndex(vi, vj, i.first, j);
			if (flag)
			{
				u_map cand = cmpInitCand(vi, vj, i.first, j);
				if (!cand.empty())
				{
					std::unordered_map<int, int> m;
					m.emplace(i.first, vi);
					m.emplace(j, vj);
					int sum = orderEdgeOfRemainQ.at(i.first).at(j).size();
                    result += countRemainQMatch(i.first, j, 0, sum, cand, m, used_v);
				}
			}
		}
	}

	return result;
}


  int Match::countRemainQMatch(int ui, int uj, int index_u, int sum, u_map cand, unordered_map<int, int> match, set<int> used_v)
{
	int result = 0;
	if (index_u < sum - 1)
	{
		int u = orderEdgeOfRemainQ.at(ui).at(uj)[index_u][0];
		if (cand.find(u) != cand.end())
		{
			for (auto& v : cand.at(u))
			{
				std::unordered_map<int, int> copy_match = match;
				copy_match.emplace(u, v);
				u_map copy_cand = cand;
				copy_cand.erase(u);
				std::set<int> copy_usedV = used_v;
				copy_usedV.emplace(v);
				copy_cand = updateCand(false, v, u, copy_cand, copy_match, copy_usedV, 0);
				int copy_index_u = index_u;
				if (!copy_cand.empty())
				{
					copy_index_u += 1;
					result += countRemainQMatch(ui, uj, copy_index_u, sum, copy_cand, copy_match, copy_usedV);
				}
                else {
                    partialMatch_num += 1;
                }
			}
		}
	}
	else if (index_u == sum - 1)
	{
		int u = orderEdgeOfRemainQ.at(ui).at(uj)[index_u][0];
		if (cand.find(u) != cand.end())
		{
			vec dif;
			set_difference(cand.at(u).begin(), cand.at(u).end(), used_v.begin(), used_v.end(), back_inserter(dif));
			result += dif.size();
		}
        else {
            partialMatch_num += 1;
        }
  	}
	return result;
}


u_map Match::cmpInitCand(int vi, int vj, int ui, int uj)        

{   
	u_map cand;
	cand = genEdgeCand(vi, ui, vj, uj, cand);
	if (q[ui].q_neis.size() == 1 && cand.empty())
	{
		cand = genEdgeCand(vj, uj, vi, ui, cand);
	}
	if (!cand.empty())
	{
		cand = genEdgeCand(vj, uj, vi, ui, cand);
	}

	return cand;
}

bool Match::checkBidirectionalEdgeOnIndex(int vi, int vj, int ui, int uj)
{
	int q_id = q[ui].q_id;
	if (initG[vi].self_LI.find(q_id) != initG[vi].self_LI.end() && initG[vj].self_LI.find(q_id) != initG[vj].self_LI.end())
	{
		if (initG[vi].self_LI.at(q_id).find(ui) != initG[vi].self_LI.at(q_id).end() && initG[vj].self_LI.at(q_id).find(uj) != initG[vj].self_LI.at(q_id).end())
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

u_map Match::genEdgeCand(int v, int u, int v_nbr, int u_nbr, u_map cand)
{
	std::set<int> nbrs = q[u].q_neis;
	nbrs.erase(u_nbr);
	for (auto& j : nbrs)
	{
		if (initG[v].nbr_LI.find(j) != initG[v].nbr_LI.end())
		{
			std::set<int> temp = initG[v].nbr_LI.at(j);
			temp.erase(v_nbr);
			if (temp.empty())
			{
				cand.clear();
				break;
			}
			if (cand.find(j) != cand.end())
			{
				vec comm;
				set_intersection(cand.at(j).begin(), cand.at(j).end(), temp.begin(), temp.end(), back_inserter(comm));
				if (!comm.empty())
				{
					std::set<int> comm_set(comm.begin(), comm.end());
					cand.at(j) = comm_set;
				}
				else
				{
					cand.clear();
					break;
				}
			}
			else
			{
				cand.emplace(j, temp);
			}
		}
		else
		{
			cand.clear();
			break;
		}
	}
	return cand;
}

u_map Match::updateCand(bool joinOperation, int v, int u, u_map cand, std::unordered_map<int, int> match, std::set<int> used_v, int dif)
{
  	bool flag = false;
	if (cand.size() <= initG[v].self_LI.at(q[u].q_id).size())
	{
		for (auto& i: cand)
		{
			if (initG[v].self_LI.at(q[u].q_id).find(i.first) != initG[v].self_LI.at(q[u].q_id).end())
			{
				cand.at(i.first).erase(v);
				if (cand.at(i.first).empty())
				{
					cand.clear();
					flag = true;
					break;
				}
			}
		}
	}
	else
	{
		for (auto& i : initG[v].self_LI.at(q[u].q_id))
		{
			if (cand.find(i) != cand.end())
			{
				cand.at(i).erase(v);
				if (cand.at(i).empty())
				{
					cand.clear();
					flag = true;
					break;
				}
			}
		}
	}


	if (flag)
	{
  		return cand;
	}
	else
	{     

		set<int> u_adjs;
		if (joinOperation)
		{    

			u_adjs = RemainQ_adjList.at(q[u].q_id).at(u);
		}
		else
		{    

			u_adjs = q[u].q_neis;
		}

		for (auto& u : u_adjs)
		{	
			int i = 0;
			if (joinOperation)
			{
				i = u - dif;
				if (match.find(i) == match.end())
				{    

					if (match.find(u) != match.end())
					{   

						continue;
					}
				}
				else
				{
					continue;
				}
			}
			else
			{
				i = u;
				if (match.find(i) != match.end())
				{
					continue;
				}
			}
			
			if (joinOperation)
			{
				i = i + dif;
			}
			else
			{
				i = u;
			}

			if (initG[v].nbr_LI.find(i) == initG[v].nbr_LI.end())
			{
  				cand.clear();
				break;
			}
			else
			{
				vec rest_c;
				set_difference(initG[v].nbr_LI.at(i).begin(), initG[v].nbr_LI.at(i).end(), used_v.begin(), used_v.end(), back_inserter(rest_c));
				if (rest_c.empty())
				{
					cand.clear();
					break;
				}
				else
				{
					std::set<int> rest_c_set(rest_c.begin(), rest_c.end());
					if (cand.find(i) != cand.end())
					{
						vec comm;
						set_intersection(cand.at(i).begin(), cand.at(i).end(), rest_c_set.begin(), rest_c_set.end(), back_inserter(comm));
						if (comm.empty())
						{
							cand.clear();
							break;
						}
						else
						{
							std::set<int> temp(comm.begin(), comm.end());
							cand.erase(i);
							cand.emplace(i, temp);
						}
					}
					else
					{
						cand.erase(i);
						cand.emplace(i, rest_c_set);
					}
				}
			}
		}
	}
	
	return cand;
}

u_map Match::cmpJoinedInitCand(int q_id, std::unordered_map<int, int> match, std::set<int> used_v, int dif)
{
	u_map cand;
	for (auto& i : RemainQ_initialCandU.at(q_id))
	{
		if (match.find(i.first-dif) == match.end())
		{
			bool flag = false;
			for (auto& j : i.second)
			{     

				int u = j - dif;
				if (initG[match.at(u)].nbr_LI.find(i.first) != initG[match.at(u)].nbr_LI.end())
				{
					vec rest_c;
					set_difference(initG[match.at(u)].nbr_LI.at(i.first).begin(), initG[match.at(u)].nbr_LI.at(i.first).end(), used_v.begin(), used_v.end(), back_inserter(rest_c));
					if (!rest_c.empty())
					{
						std::set<int> rest_c_set(rest_c.begin(), rest_c.end());
						if (cand.find(i.first) != cand.end())
						{
							vec comm;
							set_intersection(cand.at(i.first).begin(), cand.at(i.first).end(), rest_c_set.begin(), rest_c_set.end(), back_inserter(comm));
							if (!comm.empty())
							{
								std::set<int> comm_set(comm.begin(), comm.end());
								cand.at(i.first) = comm_set;
							}
							else
							{
								flag = true;
								break;
							}
						}
						else
						{
							cand.emplace(i.first, rest_c_set);
						}
					}
					else
					{
						flag = true;
						break;
					}
				}
				else
				{
					flag = true;
					break;
				}
			}
			if (flag)
			{
				cand.clear();
				break;
			}
		}
	}

	return cand;
}






  void Match::print_v(int vi)
{
	cout << "vi = " << vi << endl;
  	for (auto& nb : initG[vi].nbr_LI)
	{
		cout << "u_nbr=" << nb.first << ", v_nbr={";
		for (auto& nbr : nb.second)
		{
			cout << nbr << ",";
		}
		cout << "}" << endl;
	}
	cout << endl;
}

void Match::print_time()
{
	cout << "updateindex 1 cost (ms):" << time_slot1_1 << endl;
	cout << "updateindex 2 cost (ms):" << time_slot1_2 << endl;
	cout << "updateindex 3 cost (ms):" << time_slot1_3 << endl;
	cout << "update index total cost (ms):" << time_slot1 << endl;

  }

void Match::print_unordered_map(std::unordered_map<int, int> p)
{
  	std::map<int, int> m;
	for (auto& k : p)
	{
		m.emplace(k.first, k.second);
	}
	for (auto& k : m)
	{
		cout << "(" << k.first << " - " << k.second << ")";
	}
	cout << endl;
}

void Match::print_umap(u_map p)
{
  	for (auto& k : p)
	{
		cout << k.first << "-{";
		for (auto& d : k.second)
		{
			cout << d << ", ";
		}
		cout << "}" << endl;
	}
	cout << endl;
}

void Match::print_multimap(std::multimap<int, int> p)
{
	cout << "-------------print_multimap ---------" << endl;
	for (auto& k : p)
	{
		cout << "(" << k.first << " - " << k.second << ")";
	}
	cout << endl;
}

void Match::print_set(std::set<int> s)
{
	for (auto& u: s)
	{
		cout << u << ", ";
	}
	cout << endl;
}

void Match::printSelfLI() {
    cout << "--------------- match print selfLI ---------------" << endl;
    for (int vi=0; vi<initG.size(); ++vi) {
        cout << "v" << vi << "'selfLI : ";
        for (auto& qid_selfLI : initG[vi].self_LI){
            cout << "{Q" << qid_selfLI.first << ": ";
            for (auto& u : qid_selfLI.second){
                cout << "u" << u << ", ";
            }
            cout << "}; ";
        }
        cout << endl;
    }
}

void Match::printNbrLI() {
    cout << "--------------- match print nbrLI ---------------" << endl;
    for (int vi=0; vi<initG.size(); ++vi) {
        cout << "v" << vi << "'nbrLI : ";
        for (auto& nbrLI : initG[vi].nbr_LI){
            cout << "{u" << nbrLI.first << " :";
            for (auto& v : nbrLI.second) {
                cout << "v" << v<< ", ";
            }
            cout << "}; ";
        }
        cout << endl;
    }
}

