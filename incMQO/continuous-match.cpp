using namespace std;
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <algorithm>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <queue>
#include <stdint.h>
#include <stack>
#include <unordered_map>
#include <unordered_set>
#include <iomanip>
#include <chrono>
#include <cfloat>
#include <sstream>
#include<functional>
#include<map>  
#include<time.h>
#include "continuous-match.h"
//#include "mytime.h"
//#include <windows.h>
//#include <psapi.h>
//#pragma comment(lib,"psapi.lib")


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


//ong getMemoryUse() {
//	int who = RUSAGE_SELF;
//	struct rusage usage;
//	getrusage(who, &usage);
//	return usage.ru_maxrss;
//}

// get current process pid
inline int GetCurrentPid()
{
	return getpid();
}

// get specific process physical memeory occupation size by pid (MB)
inline float GetMemoryUsage(int pid)
{
#ifdef WIN32
	uint64_t mem = 0, vmem = 0;
	PROCESS_MEMORY_COUNTERS pmc;

	// get process hanlde by pid
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (GetProcessMemoryInfo(process, &pmc, sizeof(pmc)))
	{
		mem = pmc.WorkingSetSize;
		vmem = pmc.PagefileUsage;
	}
	CloseHandle(process);

	// use GetCurrentProcess() can get current process and no need to close handle

	// convert mem from B to MB
	return mem / 1024.0 / 1024.0;

#else
	char file_name[64] = { 0 };
	FILE* fd;
	char line_buff[512] = { 0 };
	sprintf(file_name, "/proc/%d/status", pid);

	fd = fopen(file_name, "r");
	if (nullptr == fd)
		return 0;

	char name[64];
	int vmrss = 0;
	for (int i = 0; i < VMRSS_LINE - 1; i++)
		fgets(line_buff, sizeof(line_buff), fd);

	fgets(line_buff, sizeof(line_buff), fd);
	sscanf(line_buff, "%s %d", name, &vmrss);
	fclose(fd);

	// cnvert VmRSS from KB to MB
	return vmrss / 1024.0;
#endif
}

namespace mem {
	/**
	 * get peak virtual memory space of the current process
	 * https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process#answer-64166
	 */
	inline int parseLine(char* line) {
		// This assumes that a digit will be found and the line ends in " Kb".
		int i = strlen(line);
		const char* p = line;
		while (*p < '0' || *p > '9') p++;
		line[i - 3] = '\0';
		i = atoi(p);
		return i;
	}

	inline int getValue() { //Note: this value is in KB!
		FILE* file = fopen("/proc/self/status", "r");
		int result = -1;
		char line[128];

		while (fgets(line, 128, file) != NULL) {
			if (strncmp(line, "VmPeak:", 7) == 0) {
				result = parseLine(line);
				break;
			}
		}
		fclose(file);
		return result;
	}
}


// Time counting
#define Get_Time() std::chrono::high_resolution_clock::now()
#define Duration(start) std::chrono::duration_cast<\
    std::chrono::microseconds>(Get_Time() - start).count()/(float)1000
#define Print_Time(str, start) std::cout << str << Duration(start) << " "<< Duration(start)/1000 << std::endl



inline void readDataGraph(string datagraphFile) {
	int node_id = 0;
	ifstream fin(datagraphFile);
	string line;
	getline(fin, line);
	vector<string> v;
	split(line, v, ' ');
	cnt_node = atoi(v[2].c_str()); //get count_node_number
	nodes_label = new int[cnt_node];
	nodes.resize(cnt_node);
	while (getline(fin,line))
	{
		if (line.at(0) == 't')
		{
			continue;
		}
		if (line.at(0) == 'v')
		{
			split(line, v, ' ');
			int label = atoi(v[2].c_str());
			nodes_label[node_id] = label;
			node_id++;
		}
		if (line.at(0) == 'e')
		{
			count_edge_number++;
			split(line, v, ' ');
			int left_node = atoi(v[1].c_str());
			int right_node = atoi(v[2].c_str());
			//int label = atoi(v[3].c_str());
			if (left_node != right_node) {//put the nodes into the adjacent list�����ݱ������ڽӱ�
				nodes[left_node].push_back(right_node);
				nodes[right_node].push_back(left_node);
			
				//edge_direction[to_string(left_node) + "+" + to_string(right_node)].push_back(0);//0 is output and 1 is intput
				//edge_direction[to_string(right_node) + "+" + to_string(left_node)].push_back(1);//0 is output and 1 is intput
				//
				//edge_label[to_string(left_node) + "+" + to_string(right_node)].push_back(label);
				//edge_label[to_string(right_node) + "+" + to_string(left_node)].push_back(label);
				//map<string, vector<int> >::iterator map_it;//������������ 
				//map_it = edge_label.begin();
				//while (map_it != edge_label.end())//���������������������ΪMAP�ļ�ֵ���ϸ����������������ֵ���  
				//{
				//	string tmp = map_it->first;
				//	cout << tmp << " ";
				//	cout << edge_label[tmp][0] << " ";
				//	cout << endl;
				//	map_it++;

				//	//string predicate_label = v[1];
				//}
			}
		}
	}
	fin.close();
	//cout << "node_number: " << cnt_node << endl;
	//cout << "edge_number: " << count_edge_number << endl;
}

//inline void readQueryGraph(string querygraphFile) {
//	ifstream fin(querygraphFile);
//	string line;
//	//vector<string> v;
//	while (getline(fin, line))
//	{
//		if (line.at(0) == 'v')
//		{
//			cnt_node_query++;
//		}
//	}
//	fin.close();
//}










inline void constructAQG(string querygraphFile) {
	ifstream fin(querygraphFile);
	string line;
	vector<string> v;
	int query_id = 0;
	nodes_label_query = new int[MAX_query_node];
	label_id = new int[MAX_query_node];
	
    query_nodes_c.resize(MAX_query_node);

	//fin.ignore();
	while (getline(fin, line))
	{
		if (line.at(0) == 't') {
			split(line, v, ' ');
			cnt_node_query_c = atoi(v[2].c_str());
			query_id = atoi(v[1].c_str());
			nodes_label_query_c= new int[cnt_node_query_c];
			memset(nodes_label_query_c, -1, sizeof(int) * cnt_node_query_c);
			label_id_c = new int[cnt_node_query_c];//��ͬ�ı�ǩ�ò�ͬ��ID���
		}
		if (line.at(0) == 'v') {
			split(line, v, ' ');
			int label = atoi(v[2].c_str());
			int node_id = atoi(v[1].c_str());
			nodes_label_query_c[node_id] = label;
			int duplicate_label = 0;
			for (int i = 0; i < cnt_node_query_c; i++)
			{
				if (nodes_label_query_c[i]==label)
				{
					duplicate_label++;
				}
			}
			label_id_c[node_id] = duplicate_label;//�����µĲ�ѯ��ÿ���ڵ�ı�ǩ��Ϣ
		}


		if (line.at(0) == 'e')
		{
			split(line, v, ' ');
			int left_node = atoi(v[1].c_str());
			int right_node = atoi(v[2].c_str());
			int aa = 0; int bb = 0;
			if (left_node != right_node) {//put the nodes into the adjacent list�����ݱ������ڽӱ�
				for (int i = 0; i < cnt_node_query; i++)
				{
					if (nodes_label_query[i]== nodes_label_query_c[left_node]&&label_id[i]==label_id_c[left_node])//��߽ڵ�
					{
						aa++;
						for (int j = 0; j < query_nodes_c[i].size(); j++)
						{
							if (nodes_label_query[query_nodes_c[i][j]] == nodes_label_query_c[right_node] && label_id[query_nodes_c[i][j]] == label_id_c[right_node])
							{
								break;
							}
							else
							{
								bb++;
							}
						}
						if (bb== query_nodes_c[i].size())
						{
							
							int cc = 0;
							for (int m = 0; m < cnt_node_query; m++)
							{
								if (nodes_label_query[m] == nodes_label_query_c[right_node] && label_id[m] == label_id_c[right_node])
								{
									query_nodes_c[i].push_back(m);
									query_nodes_c[m].push_back(i);
									break;
								}
								else
								{
									cc++;
								}
							}
							if (cc==cnt_node_query)
							{
								query_nodes_c[i].push_back(cnt_node_query);
								query_nodes_c[cnt_node_query].push_back(i);
								nodes_label_query[cnt_node_query] = nodes_label_query_c[right_node];
								label_id[cnt_node_query] = label_id_c[right_node];
								cnt_node_query++;
							}
						}
						break;
					}
					if (nodes_label_query[i] == nodes_label_query_c[right_node] && label_id[i] == label_id_c[right_node])//�ұ߽ڵ�
					{
						aa++;
						for (int j = 0; j < query_nodes_c[i].size(); j++)
						{
							if (nodes_label_query[query_nodes_c[i][j]] == nodes_label_query_c[left_node] && label_id[query_nodes_c[i][j]] == label_id_c[left_node])
							{
								break;
							}
							else
							{
								bb++;
							}
						}
						if (bb == query_nodes_c[i].size())
						{
							
							int cc = 0;
							for (int m = 0; m < cnt_node_query; m++)
							{
								if (nodes_label_query[m] == nodes_label_query_c[left_node] && label_id[m] == label_id_c[left_node])
								{
									query_nodes_c[i].push_back(m);
									query_nodes_c[m].push_back(i);
									break;
								}
								else
								{
									cc++;
								}
							}
							if (cc ==cnt_node_query)
							{
								query_nodes_c[i].push_back(cnt_node_query);
								query_nodes_c[cnt_node_query].push_back(i);
								nodes_label_query[cnt_node_query] = nodes_label_query_c[left_node];
								label_id[cnt_node_query] = label_id_c[left_node];
								cnt_node_query++;
							}
						}
						break;
					}
				}
				if (aa==0)
				{
					int n1 = cnt_node_query;
					int n2 = cnt_node_query + 1;
					cnt_node_query = n2+1;
					query_nodes_c[n1].push_back(n2);
					query_nodes_c[n2].push_back(n1);
					nodes_label_query[n1] = nodes_label_query_c[left_node];
					label_id[n1] = label_id_c[left_node];
					nodes_label_query[n2] = nodes_label_query_c[right_node];
					label_id[n2] = label_id_c[right_node];
				}
			}
		}
	
	}
	fin.close();
/*********************************��ȡ��Ҫ��ͳ����Ϣ*********************************************************/
	query_nodes.resize(cnt_node_query);
	for (int i = 0; i < cnt_node_query; i++)
	{
		for (int j = 0; j < query_nodes_c[i].size(); j++)
		{
			query_nodes[i].push_back(query_nodes_c[i][j]);
		}
	}
	query_nodes_c.clear();
	delete[]nodes_label_query_c;
	delete[]label_id_c;
}



inline void labelAQG(string querygraphFile) {
	ifstream fin(querygraphFile);
	string line;
	vector<string> v;
	int query_id = 0;
	nodes_label_query = new int[cnt_node_query];
	label_id = new int[cnt_node_query];

	query_edge_id.resize(cnt_node_query);
	for (int i = 0; i < cnt_node_query; i++)
	{
		query_edge_id[i].resize(query_nodes[i].size());//ΪAQG��ÿһ���߸�����ѯid
	}
	query_nodes.clear();
	query_nodes.resize(cnt_node_query);
	cnt_node_query = 0;
   
	while (getline(fin, line))
	{
		if (line.at(0) == 't') {
			split(line, v, ' ');
			cnt_node_query_c = atoi(v[2].c_str());
			query_id = atoi(v[1].c_str());
			nodes_label_query_c = new int[cnt_node_query_c];
			memset(nodes_label_query_c, -1, sizeof(int) * cnt_node_query_c);
			label_id_c = new int[cnt_node_query_c];//��ͬ�ı�ǩ�ò�ͬ��ID���
		}
		if (line.at(0) == 'v') {
			split(line, v, ' ');
			int label = atoi(v[2].c_str());
			int node_id = atoi(v[1].c_str());
			nodes_label_query_c[node_id] = label;
			int duplicate_label = 0;
			for (int i = 0; i < cnt_node_query_c; i++)
			{
				if (nodes_label_query_c[i] == label)
				{
					duplicate_label++;
				}
			}
			label_id_c[node_id] = duplicate_label;//�����µĲ�ѯ��ÿ���ڵ�ı�ǩ��Ϣ
		}


		if (line.at(0) == 'e')
		{
			split(line, v, ' ');
			int left_node = atoi(v[1].c_str());
			int right_node = atoi(v[2].c_str());
			int aa = 0; int bb = 0;
			if (left_node != right_node) {//put the nodes into the adjacent list�����ݱ������ڽӱ�
				for (int i = 0; i < cnt_node_query; i++)
				{
					if (nodes_label_query[i] == nodes_label_query_c[left_node] && label_id[i] == label_id_c[left_node])//��߽ڵ�
					{
						aa++;
						for (int j = 0; j < query_nodes[i].size(); j++)
						{
							if (nodes_label_query[query_nodes[i][j]] == nodes_label_query_c[right_node] && label_id[query_nodes[i][j]] == label_id_c[right_node])
							{
								query_edge_id[i][j].push_back(query_id);
								for (int k = 0; k <   query_nodes[query_nodes[i][j]].size(); k++)
								{
									if (query_nodes[query_nodes[i][j]][k]==i)
									{
										query_edge_id[query_nodes[i][j]][k].push_back(query_id);
										break;
									}
								}

								
								break;
							}
							else
							{
								bb++;
							}
						}
						if (bb == query_nodes[i].size())
						{
							
							int cc = 0;
							for (int m = 0; m < cnt_node_query; m++)
							{
								if (nodes_label_query[m] == nodes_label_query_c[right_node] && label_id[m] == label_id_c[right_node])
								{
									query_nodes[i].push_back(m);
									int a = query_nodes[i].size() - 1;
									query_edge_id[i][a].push_back(query_id);
									query_nodes[m].push_back(i);
									int b = query_nodes[m].size() - 1;
									query_edge_id[m][b].push_back(query_id);
									break;
								}
								else
								{
									cc++;
								}
							}
							if (cc == cnt_node_query)
							{
								query_nodes[i].push_back(cnt_node_query);
								int a = query_nodes[i].size() - 1;
								query_edge_id[i][a].push_back(query_id);


								query_nodes[cnt_node_query].push_back(i);
								int b = query_nodes[cnt_node_query].size() - 1;
								query_edge_id[cnt_node_query][b].push_back(query_id);



								nodes_label_query[cnt_node_query] = nodes_label_query_c[right_node];
								label_id[cnt_node_query] = label_id_c[right_node];
								cnt_node_query++;
							}
						}
						break;
					}
					if (nodes_label_query[i] == nodes_label_query_c[right_node] && label_id[i] == label_id_c[right_node])//�ұ߽ڵ�
					{
						aa++;
						for (int j = 0; j < query_nodes[i].size(); j++)
						{
							if (nodes_label_query[query_nodes[i][j]] == nodes_label_query_c[left_node] && label_id[query_nodes[i][j]] == label_id_c[left_node])
							{
								query_edge_id[i][j].push_back(query_id);
								for (int k = 0; k < query_nodes[query_nodes[i][j]].size(); k++)
								{
									if (query_nodes[query_nodes[i][j]][k] == i)
									{
										query_edge_id[query_nodes[i][j]][k].push_back(query_id);
										break;
									}
								}
								break;
							}
							else
							{
								bb++;
							}
						}
						if (bb == query_nodes[i].size())
						{

							int cc = 0;
							for (int m = 0; m < cnt_node_query; m++)
							{
								if (nodes_label_query[m] == nodes_label_query_c[left_node] && label_id[m] == label_id_c[left_node])
								{
									query_nodes[i].push_back(m);
									int a = query_nodes[i].size() - 1;
									query_edge_id[i][a].push_back(query_id);
									query_nodes[m].push_back(i);
									int b = query_nodes[m].size() - 1;
									query_edge_id[m][b].push_back(query_id);
									break;
								}
								else
								{
									cc++;
								}
							}
							if (cc == cnt_node_query)
							{
								query_nodes[i].push_back(cnt_node_query);
								int a = query_nodes[i].size() - 1;
								query_edge_id[i][a].push_back(query_id);
								query_nodes[cnt_node_query].push_back(i);
								int b = query_nodes[cnt_node_query].size() - 1;
								query_edge_id[cnt_node_query][b].push_back(query_id);
								nodes_label_query[cnt_node_query] = nodes_label_query_c[left_node];
								label_id[cnt_node_query] = label_id_c[left_node];
								cnt_node_query++;
							}
						}
						break;
					}
				}
				if (aa == 0)
				{
					int n1 = cnt_node_query;
					int n2 = cnt_node_query + 1;
					cnt_node_query = n2 + 1;
					query_nodes[n1].push_back(n2);
					int a = query_nodes[n1].size() - 1;
					query_edge_id[n1][a].push_back(query_id);
					query_nodes[n2].push_back(n1);
					int b = query_nodes[n2].size() - 1;
					query_edge_id[n2][b].push_back(query_id);
					nodes_label_query[n1] = nodes_label_query_c[left_node];
					label_id[n1] = label_id_c[left_node];
					nodes_label_query[n2] = nodes_label_query_c[right_node];
					label_id[n2] = label_id_c[right_node];
				}
			}
		}

	}
	query_num = query_id+1;
	fin.close();
	delete[]nodes_label_query_c;
	delete[]label_id_c;
	delete[]label_id;
	cout << "query_node_number: " << cnt_node_query << endl;
}










inline void coreDecomposition_query() {
	int matrix_index = 0;
	for (int i = 0; i < cnt_node_query; i++)
	{
		query_nodes_array_info[i] = matrix_index;
		if (query_nodes[i].size() > MAX_DEGREE_QUERY)
			MAX_DEGREE_QUERY = query_nodes[i].size();
		core_number_query[i] = query_nodes[i].size();
		for (int j = 0; j <query_nodes[i].size(); j++)
		{
			query_nodes_array.push_back(query_nodes[i][j]);
			matrix_index++;
		}
	}
	query_nodes_array_info[cnt_node_query] = matrix_index;
	int * bin = bin_query;
	int * pos = pos_query;
	int * vert = vert_query;
	memset(bin, 0, sizeof(int) * (MAX_DEGREE_QUERY + 1));
	for (int i = 0; i < cnt_node_query; i++)
		bin[core_number_query[i]] ++;
	int start = 0;
	int num;
	for (int d = 0; d <= MAX_DEGREE_QUERY; d++) {
		num = bin[d];
		bin[d] = start;
		start += num;
	}
	for (int i = 0; i < cnt_node_query; i++) {
		pos[i] = bin[core_number_query[i]];
		vert[pos[i]] = i;
		bin[core_number_query[i]] ++;
	}
	for (int d = MAX_DEGREE_QUERY; d > 0; d--)
		bin[d] = bin[d - 1];
	bin[0] = 0;
	for (int i = 0; i < cnt_node_query; i++) {

		int v = vert[i];

		for (int j = query_nodes_array_info[v]; j < query_nodes_array_info[v + 1]; j++) {

			int u = query_nodes_array[j]; // nodes_query[v][j];v���ڽӱ��еĶ���

			if (core_number_query[u] > core_number_query[v]) {

				int du = core_number_query[u];//������ڽӵ�Ķȱȶ����Ķ���Ķ�
				int pu = pos[u];

				int pw = bin[du];//<pos[u]>�ҵ���u�Ķ���ͬ�Ķ���
				int w = vert[pw];//<u>

				if (u != w) {	//if not the same node, switch the position of the two nodes.��ΪɶҪת��λ��
					pos[u] = pw;
					pos[w] = pu;
					vert[pu] = w;
					vert[pw] = u;
				}

				bin[du] ++;
				core_number_query[u]--;
			}
		}
	}
}

inline int  start_node_selection_Tree(){

	double least_ranking = DBL_MAX;
	float*ranking = new float[cnt_node_query];
	memset(ranking, 0, sizeof(float) * cnt_node_query);
	int degree = 0;

	for (int i = 0; i < cnt_node_query; i++) {

		degree = query_nodes[i].size();
		for (int j = 0; j < cnt_node; j++)
		{

			if (nodes_label_query[i] == nodes_label[j])
			{
				ranking[i]++;
			}
		}
		ranking[i] = ranking[i] / degree;
	}
	int start_node=-1;
	float temp = ranking[0];
	for (int i = 1; i < cnt_node_query; i++)
	{
		if ( ranking[i] <= temp)
		{
			temp = ranking[i];
		}
	}
	for (int i = 0; i < cnt_node_query; i++)
	{
		if ( temp == ranking[i]) {
			start_node = i;
			break;
		}
	}
	cerr << "start node is " << start_node << endl;
	delete[] ranking;
	return start_node;

}

inline int start_node_selection_NORMAL() {

	double least_ranking = DBL_MAX;
	
	float*ranking=new float[cnt_node_query];
	memset(ranking, 0, sizeof(float) * cnt_node_query);
	int degree;
	
	

	for (int i = 0; i < cnt_node_query; i++) {
		
		if (core_number_query[i] < 2)	//root node must be selected from the core structure
			continue;

		//label = nodes_label_query[i];
		degree = query_nodes[i].size();
		for (int j = 0; j < cnt_node; j++)
		{
			
				if (nodes_label_query[i]==nodes_label[j])
				{
					ranking[i]++;
				}
				
		}
		ranking[i] = ranking[i] / degree;
	}
	int start_node=-1;
	float temp = ranking[0];
	for (int i = 1; i < cnt_node_query; i++)
	{
		if (core_number_query[i] >= 2&&ranking[i]<=temp)
		{
			temp = ranking[i];
		}
	}
	for (int i = 0; i < cnt_node_query; i++)
	{
		if (core_number_query[i] >= 2&&temp == ranking[i]) {
			start_node = i;
			break;
		}
	}
	cerr << "start node is " << start_node << endl;
	delete[] ranking;
	return start_node;
}

inline void BFS_TREE() {

	/*
	* for the normal input, this function should only consider the core structure nodes for the future top-down indexing
	* output : true_leaf_nodes, simulation_sequence_array, level_to_sequence which maps a level to a segment in the sequence
	*/

	resetTreeNodes();

	core_tree_node_child_array_index = 0;
	core_tree_node_nte_array_index = 0;
	exploreCRSequence_indx = 0;

	int * visited = visited_int_for_query;
	memset(visited, 0, sizeof(int) * cnt_node_query);
	int * queue_array = queue_array_query;
	queue_array[0] = root_node_id;//����
	int pointer_this_start = 0;
	int pointer_this_end = 1;
	int pointer_next_end = 1;
	int current_level = 1; //initial level starts at 1
	simulation_sequence_index = 0;
	level_index.clear();
	visited[root_node_id] = 1;
	BFS_level_query[root_node_id] = 1;
	BFS_parent_query[root_node_id] = -1;

	initializeTreeNode(core_query_tree[root_node_id], -1);

	while (true) {

		int start = simulation_sequence_index;

		while (pointer_this_start != pointer_this_end) { // queue not empty

			int current_node = queue_array[pointer_this_start];
			pointer_this_start++;

			int start = query_nodes_array_info[current_node];
			int end = query_nodes_array_info[current_node + 1];//����BFS��

			for (int i = start; i < end; i++) {

				int childNode = query_nodes_array[i];

				if (visited[childNode] != 0) { //this child node has been visited before,
					if (childNode != core_query_tree[current_node].parent_node)//��ͬһ����
						addNonTreeEdgeToTreeNode(core_query_tree[current_node], childNode);//�ҵ�Nte�ߣ���������S-NTE
					if (visited[childNode] > current_level)	//this is a cross level nte��C-NTE
						addCrossLevelNTEToTreeNode(core_query_tree[childNode], current_node); //record its cross level nte parent
				}
				else {					//this child node has not been visited.
					visited[childNode] = current_level + 1; //parent node's level plus one��ǰ�ȼ����Ǹ��ڵ�
					queue_array[pointer_next_end++] = childNode;
					BFS_level_query[childNode] = current_level + 1;
					BFS_parent_query[childNode] = current_node;

					/*if (core_number_query[childNode] < 2)
						continue;*/

					initializeTreeNode(core_query_tree[childNode], current_node);
					addChildToTreeNode(core_query_tree[current_node], childNode);
				}
			}

			simulation_sequence[simulation_sequence_index++] = current_node;
		}

		int end = simulation_sequence_index;
		level_index.push_back(make_pair(start, end));//������������Ԫ��

		for (int i = end - 1; i >= start; i--) {
			int node = simulation_sequence[i];
			if (core_number_query[node] < 2)
				continue;
			exploreCRSequence[exploreCRSequence_indx++] = node;//��Ҫ��������N-TREE�ߵĺ�ʵ
		}

		if (pointer_next_end == pointer_this_end) //no node has been pushed in
			break;

		pointer_this_start = pointer_this_end;
		pointer_this_end = pointer_next_end;
		current_level++;
	}
}



inline void BFS_PARENT_CHILD() {
	//resetTreeNodes();
	int * visited = visited_int_for_query;
	memset(visited, 0, sizeof(int) * cnt_node_query);
	BFS_parent.resize(cnt_node_query);
	BFS_child.resize(cnt_node_query);
	int * queue_array = queue_array_query;
	queue_array[0] = root_node_id;

	int pointer_this_start = 0;
	int pointer_this_end = 1;
	int pointer_next_end = 1;
	//int current_level = 1; //initial level starts at 1

	simulation_sequence_index = 0;
	visited[root_node_id] = 1;
	BFS_parent[root_node_id].push_back(-1);

	while (true) {

		int start = simulation_sequence_index;

		while (pointer_this_start != pointer_this_end) { // queue not empty

			int current_node = queue_array[pointer_this_start];
			pointer_this_start++;

			int start = 0;
			int end = query_nodes[current_node].size();

			for (int i = start; i <end; i++) {

				int childNode = query_nodes[current_node][i];
				int d = 0;
				if (visited[childNode] != 0) { //this child node has been visited before,
					for (int i = 0; i < BFS_parent[current_node].size(); i++)
					{
						if (BFS_parent[current_node][i]==childNode)
						{
							d++;
							break;
						}
					}
					if (d==0) {
					
					     BFS_parent[childNode].push_back(current_node);
						 BFS_child[current_node].push_back(childNode);
					
					}//�ҵ�no-tree��

				}
				else {//this child node has not been visited.

					BFS_child[current_node].push_back(childNode);
					BFS_parent[childNode].push_back(current_node);


					visited[childNode] = 1; 

					queue_array[pointer_next_end] = childNode;
					pointer_next_end++;
				}
			}
		}
		if (pointer_next_end == pointer_this_end) //no node has been pushed in
			break;
		pointer_this_start = pointer_this_end;
		pointer_this_end = pointer_next_end;
	}
}



inline void BFS_NORMAL() {

	resetTreeNodes();

	core_tree_node_child_array_index = 0;
	core_tree_node_nte_array_index = 0;

	exploreCRSequence_indx = 0;

	int * visited = visited_int_for_query;
	memset(visited, 0, sizeof(int) * cnt_node_query);

	int * queue_array = queue_array_query;

	queue_array[0] = root_node_id;

	int pointer_this_start = 0;
	int pointer_this_end = 1;
	int pointer_next_end = 1;
	//int current_level = 1; //initial level starts at 1

	simulation_sequence_index = 0;
	level_index.clear();
	visited[root_node_id] = 1;
	BFS_level_query[root_node_id] = 1;
	//BFS_parent_query[root_node_id] = -1;
	initializeTreeNode(core_query_tree[root_node_id], -1);

	while (true) {

		int start = simulation_sequence_index;

		while (pointer_this_start != pointer_this_end) { // queue not empty

			int current_node = queue_array[pointer_this_start];
			pointer_this_start++;

			int start = 0;
			int end = query_nodes[current_node].size();

			for (int i = start; i <end; i++) {

				int childNode = query_nodes[current_node][i];

				if (visited[childNode] != 0) { //this child node has been visited before,

					continue;

				}
				else {//this child node has not been visited.

					visited[childNode] = current_level + 1; //parent node's level plus one

					queue_array[pointer_next_end] = childNode;
					pointer_next_end++;

					BFS_level_query[childNode] = current_level + 1;
					//BFS_parent_query[childNode] = current_node;

					initializeTreeNode(core_query_tree[childNode], current_node);
					//addChildToTreeNode(core_query_tree[current_node], childNode);
				}
			}

			simulation_sequence[simulation_sequence_index] = current_node;
			simulation_sequence_index++;
		}

		int end = simulation_sequence_index;

		level_index.push_back(make_pair(start, end));

		if (pointer_next_end == pointer_this_end) //no node has been pushed in
			break;

		pointer_this_start = pointer_this_end;
		pointer_this_end = pointer_next_end;
		current_level++;
	}
}

//inline void TreeMat_Path(int parent, int child) {
//	int str1 = edge_direction_query[to_string(parent) + "+" + to_string(child)][0];
//	
//	
//	
//	for (int i = 0; i < is_path[parent].size(); i++)
//	{
//		for (int j = 0; j < nodes[is_path[parent][i]].size(); j++)
//		{
//			int candidate_p = is_path[parent][i];
//			int candidate_c = nodes[is_path[parent][i]][j];
//			int str2 = edge_direction[to_string(candidate_p) + "+" + to_string(candidate_c)][0];
//			
//			
//			if (str1 == str2)
//			{
//				if (str1==0)
//				{
//					int str3 = edge_label_query[to_string(parent) + "+" + to_string(child)][0];
//					int str4 = edge_label[to_string(candidate_p) + "+" + to_string(candidate_c)][0];
//					if (str3 == str4&&std::find(is_path[child].begin(), is_path[child].end(), candidate_c) == is_path[child].end()) //��Ҫ�ж����Ԫ���Ƿ����
//					{
//						is_path[child].push_back(candidate_c);
//					}
//				}
//				if (str1 == 1)
//				{
//					int str5 = edge_label_query[to_string(child) + "+" + to_string(parent)][0];
//					int str6 = edge_label[to_string(candidate_c) + "+" + to_string(candidate_p)][0];
//					if (str5 == str6&&std::find(is_path[child].begin(), is_path[child].end(), candidate_c) == is_path[child].end())
//					{
//						is_path[child].push_back(candidate_c);
//					}
//				}
//			}
//		}
//	}
//}





inline void constructMDCG() {

	int * in_degree = visited_int_for_query;
	memset(in_degree, 0, sizeof(int) * cnt_node_query);
	// BFS_NORMAL();
	for (int i = 0; i < cnt_node_query; i++)
	{
		if (i==root_node_id)
		{
			in_degree[i] = 0;
		}
		else
		{
			in_degree[i] = BFS_parent[i].size();
		}
	}
	queue<int>q;
	q.push(root_node_id);
	while (!q.empty())
	{
		int t = q.front();
		q.pop();
		ORDER1.push_back(t);
		for (int i = 0; i <BFS_child[t].size() ; i++)
		{
			in_degree[BFS_child[t][i]]--;
			if (in_degree[BFS_child[t][i]]==0)
			{
				q.push(BFS_child[t][i]);
			}
		}
	}


	/************************************************************************/
	//ORDER1.push_back(root_node_id);
	//for (int i = 2; i < current_level+1; i++)
	//{
	//	for (int j = 0; j <cnt_node_query; j++)
	//	{

	//	}
	//}

	//ORDER1.push_back(1); ORDER1.push_back(0); ORDER1.push_back(2); ORDER1.push_back(3); ORDER1.push_back(4); 

	/************************************************************************/
	for (int i = cnt_node_query-1; i >=0; i--)
	{
		
			ORDER2.push_back(ORDER1[i]);
		
	}



	//N_match.resize(cnt_node_query);
	/********************************������ʼ�������ݽṹ���Բ�ѯΪ����***********************************************************/
	CAND.resize(cnt_node_query);//���ÿһ����ѯ�ڵ�ĺ�ѡ��
	for (int  i = 0; i <cnt_node_query; i++)
	{
		for (int j = 0; j < cnt_node; j++)
		{
			if (nodes_label[j]==nodes_label_query[i])
			{
				CAND[i].push_back(j);
			}
		}
	}
	stree_or_match.resize(cnt_node_query);
	for (int i = 0; i <cnt_node_query; i++)
	{
		stree_or_match[i].resize(CAND[i].size());
		//if (BFS_child[i].size()>0)
		//{
		//	N_stree[i].resize(CAND[i].size());
		//}
		/*if (BFS_level_query[i]-1 !=0)
		{
			N_match[i].resize(CAND[i].size());
		}*/
		for (int j = 0; j < CAND[i].size(); j++)
		{
			stree_or_match[i][j].resize(query_num);
			for (int n = 0; n <query_num; n++)
			{
				stree_or_match[i][j][n].push_back(0);
				stree_or_match[i][j][n].push_back(0);//Ϊÿһ���ڵ㹹��һ��״̬��ʶ����˵���䵱ǰ��״̬. ��Ӧ�Ĳ�ѯ����������ƥ�仹��������ƥ��
			}
		}	
	}

/********************************ʹ��ORDER2������stree***********************************************************/
	//int * temp = visited_int_for_query;
	//int * temp=(int *)malloc(sizeof(int) * (cnt_node));
	int * visited1 = visited_int_for_query;
	
	int * temp = visited_int_for_query1;
	

	for (int i = 0; i < ORDER2.size(); i++)
	{
		int cur = ORDER2[i];
		if (BFS_child[cur].size()==0)
		{
			for (int j = 0; j < CAND[cur].size(); j++) {
				for (int k = 0; k < query_num; k++)
				{
					stree_or_match[cur][j][k][0] = 1;
				}
			}
		}
		else
		{
			memset(visited1, 0, sizeof(int) * query_num);
			
			for (int j = 0; j < BFS_child[cur].size(); j++)
			{
				int child = BFS_child[cur][j];
				for (int k = 0; k < query_nodes[cur].size(); k++)
				{
					if (query_nodes[cur][k]==child)
					{
						for (int m = 0; m < query_edge_id[cur][k].size(); m++)
						{
							visited1[query_edge_id[cur][k][m]]++;//�жϺ��ӽڵ��д��ڵĲ�ѯ
						}
					}
				}
			}
			for (int j = 0; j < query_num; j++)
			{
				int temp1 = 0; 
				if (visited1[j]>0)
				{
					memset(temp, 0, sizeof(int) * CAND[cur].size());
					for (int k = 0; k < BFS_child[cur].size(); k++)
					{
						if (verify_query(cur, BFS_child[cur][k],j))//���ں�ʵ������֮���Ƿ���ڶ�Ӧ�Ĳ�ѯid
						{
							temp1++;
							int  child = BFS_child[cur][k];
							for (int m = 0; m < CAND[cur].size(); m++)
							{
				
								for (int n = 0; n < CAND[child].size(); n++)
								{
									int cc = 0;
									if (stree_or_match[child][n][j][0] == 1)
									{
										for (int p = 0; p < nodes[CAND[child][n]].size(); p++)
										{
											if ( nodes[CAND[child][n]][p] == CAND[cur][m])
											{

												temp[m]++; cc = 1;
												break;
											}
										}
									}
									if (cc==1)
									{
										break;
									}
								}
							}
						}
					}
					for (int k = 0; k < CAND[cur].size(); k++)
					{
						if (temp[k]== temp1)
						{
							stree_or_match[cur][k][j][0] = 1;
						}
					}
				}
			}
		}
	}
	/********************************ʹ��ORDER1������match***********************************************************/
	for (int i = 0; i < ORDER1.size(); i++) {
		int cur = ORDER1[i];
		if (BFS_parent[cur][0]== -1)
		{
			for (int j = 0; j < CAND[cur].size(); j++)
			{
				for (int k = 0; k < query_num; k++)
				{
					if (stree_or_match[cur][j][k][0] == 1)
					{
						stree_or_match[cur][j][k][1] = 1;
					}
				}
			}
		}

		memset(visited1, 0, sizeof(int) * query_num);
		for (int j = 0; j < BFS_parent[cur].size(); j++)
		{
			int parent = BFS_parent[cur][j];
			for (int k = 0; k < query_nodes[cur].size(); k++)
			{
				if (query_nodes[cur][k] == parent)
				{
					for (int m = 0; m < query_edge_id[cur][k].size(); m++)
					{
						visited1[query_edge_id[cur][k][m]]++;//�жϸ��׽ڵ��д��ڵĲ�ѯ����ORDER2�Գ�
					}
				}
			}
			for (int j = 0; j < query_num; j++)
			{
				int temp1 = 0; 
				if (visited1[j]>0)
				{
					memset(temp, 0, sizeof(int) * CAND[cur].size());
					for (int k = 0; k < BFS_parent[cur].size(); k++)
					{
						if (verify_query(cur, BFS_parent[cur][k], j))//���ں���������֮���Ƿ���ڶ�Ӧ�Ĳ�ѯid
						{
							temp1++;
							int  parent = BFS_parent[cur][k];
							for (int m = 0; m < CAND[cur].size(); m++)
							{
								
								for (int n = 0; n < CAND[parent].size(); n++)
								{
									int cc = 0;
									if (stree_or_match[parent][n][j][1] == 1)
									{
										for (int p = 0; p < nodes[CAND[parent][n]].size(); p++)
										{
											if ( nodes[CAND[parent][n]][p] == CAND[cur][m])
											{

												temp[m]++; cc++;
												break;
											}
										}
									}
									if (cc==1)
									{
										break;
									}
								}
							}
						}
					}
					for (int k = 0; k < CAND[cur].size(); k++)
					{
						if (temp[k] == temp1&& stree_or_match[cur][k][j][0] == 1)
						{
							stree_or_match[cur][k][j][1] = 1;
						}
					}
				}
			}
		}
	}
/********************************���Ҷ����ڵ㣬�����Ӧ��match��Ϊ1**********************************************************/
	memset(visited1, 0, sizeof(int) * query_num);
	memset(temp, 0, sizeof(int) * query_num);
	for (int i = 0; i < cnt_node_query; i++)
	{
		if (BFS_child[i].size()>0)
		{
			for (int j = 0; j < BFS_parent[i].size(); j++)
			{
				int parent = BFS_parent[i][j];
				for (int k = 0; k < query_nodes[i].size(); k++)
				{
					if (query_nodes[i][k]== parent)
					{
						for (int m= 0; m < query_edge_id[i][k].size(); m++)
						{
							visited1[query_edge_id[i][k][m]]++;
						}
					}
				}
			}
			for (int j = 0; j < BFS_child[i].size(); j++)
			{
				int child = BFS_child[i][j];
				for (int k = 0; k < query_nodes[i].size(); k++)
				{
					if (query_nodes[i][k] == child)
					{
						for (int m = 0; m < query_edge_id[i][k].size(); m++)
						{
							temp[query_edge_id[i][k][m]]++;
						}
					}
				}
			}
		}
		for (int j = 0; j < query_num; j++)
		{
			if (temp[j]>0&& visited1[j]==0)//��ѯ�������뺢�ӽڵ����ӵı��У�û�г������븸�׽ڵ����ӵı���
			{
				for (int k = 0; k < CAND[i].size(); k++)
				{
					if (stree_or_match[i][k][j][0] ==1)
					{
						stree_or_match[i][k][j][1] = 1;
					}
				}
			}
		}
	}
	//delete[]visited1;
	//delete[]visited2;
}


inline void updateDOWN(int data_parent, int data_child, int query_parent, int query_child, int id) {
	int s2 = 0;
	int s3 = 0;
	for (int i = 0; i < BFS_parent[query_child].size(); i++)
	{

		if (BFS_parent[query_child][i] == query_parent)
		{
			continue;
		}
		else
		{
			int parent = BFS_parent[query_child][i];

			if (verify_query(parent, query_child, id))
			{
				s3++;
				int s1 = 0;
				for (int j = 0; j < CAND[parent].size(); j++)
				{

					if (stree_or_match[parent][j][id][1] == 1)
					{
						for (int k = 0; k < nodes[CAND[parent][j]].size(); k++)
						{
							if (nodes[CAND[parent][j]][k] == data_child)
							{
								s1++; s2++;
								break;
							}
							
						}
					}
					if (s1>0)
					{
						break;
					}
				}

			}

		}
	}
	int mm = 0;
	for (int i = 0; i < CAND[query_child].size(); i++)
	{
		if (CAND[query_child][i]== data_child)
		{
			mm = i;
			break;
		}
	}
	if (s2 == s3&& stree_or_match[query_child][mm][id][0] == 1 && stree_or_match[query_child][mm][id][1] == 0)
	{

		stree_or_match[query_child][mm][id][1] = 1;

		for (int i = 0; i < BFS_child[query_child].size(); i++)
		{
			int child = BFS_child[query_child][i];
			if (verify_query(query_child, child, id)) {
				for (int j = 0; j < CAND[child].size(); j++)
				{
					for (int k = 0; k < nodes[CAND[child][j]].size(); k++)
					{
						if (nodes[CAND[child][j]][k] == data_child&& stree_or_match[child][j][id][0] == 1 && stree_or_match[child][j][id][1] == 0)
						{
							Q_DOWN.push(make_pair(data_child, CAND[child][j]));
							Q_DOWN_location.push(make_pair(query_child, child));
							break;
						}
					}
				}


				//for (int j = 0; j < nodes[data_child].size(); j++)
				//{
				//	if (nodes_label[nodes[data_child][j]] == nodes_label_query[child])
				//	{
				//		for (int k = 0; k < CAND[child].size(); k++)
				//		{
				//			if (CAND[child][k] == nodes[data_child][j] && stree_or_match[child][k][id][0] == 1 && stree_or_match[child][k][id][1] == 0)
				//			{
				//				Q_DOWN.push(make_pair(nodes[data_child][j], data_child));
				//				Q_DOWN_location.push(make_pair(child, query_child));

				//				//updateUP(nodes[data_parent][j], data_parent, parent, query_parent, id);
				//			}
				//		}
				//	}
				//}
			}
		}
	
	}
}




inline void GetMatch(int data_parent, int data_child, int query_parent, int query_child, int id) {
	vector<int>matching_order;
	vector<vector<int>>CAND_order;
	vector<vector<int>>CAND_order1;





	CAND_order1.resize(cnt_node_query);
	CAND_order.resize(cnt_node_query);
	matching_order.push_back(query_parent);
	matching_order.push_back(query_child);
	CAND_order[query_parent].push_back(data_parent);
	CAND_order[query_child].push_back(data_child);
	CAND_order1[query_parent].push_back(data_parent);
	CAND_order1[query_child].push_back(data_child);
	int * visited = visited_int_for_query;
	int * visited3 = visited_int_for_query1;
	memset(visited, 0, sizeof(int) * cnt_node_query);
	visited[query_parent] = 1;
	visited[query_child] = 1;
	int cur_node = query_parent;
	int cur_node1;
	
	for (int i = 0; i < cnt_node_query; i++)
	{
		if (visited[i]==0)
		{
			for (int j = 0; j <CAND[i].size() ;  j++)
			{
				if (stree_or_match[i][j][id][1]==1)
				{
					CAND_order1[i].push_back(CAND[i][j]);
				}
			}
		}
	}

	//for (int i = 0; i < ORDER1.size(); i++)
	//{
	//	if (ORDER1[i]= cur_node)
	//	{
	//		cur_node1 = i + 1;
	//		break;
	//	}
	//}




	//for (int i = cur_node1; i <ORDER1.size(); i++)
	//{
	//	int temp1 = 0;
	//	int cur = ORDER1[i];
	//	
	//	memset(visited3, 0, sizeof(int) * CAND_order1[cur].size());
	//		
	//	for (int j = 0; j < BFS_parent[cur].size(); j++)
	//	{
	//		int parent = BFS_parent[cur][j];

	//		if (verify_query(cur, parent, id))
	//		{
	//			temp1++;
	//			for (int k = 0; k < CAND_order1[cur].size(); k++)
	//			{
	//				
	//					for (int m = 0; m < CAND_order1[parent].size(); m++)
	//					{
	//						for (int p = 0; p < nodes[CAND_order1[parent][m]].size(); p++)
	//						{
	//							if (nodes[CAND_order1[parent][m]][p] == CAND_order1[cur][k])
	//							{

	//								visited3[CAND_order1[cur][k]]++;
	//								break;
	//							}
	//						}
	//						if (visited3[CAND_order1[cur][k]] == temp1)
	//						{
	//							break;
	//						}
	//					}
	//				
	//			}
	//		}
	//			
	//	}
	//	

	//	if (temp1>0)
	//	{
	//		
	//		for (int j = 0; j < CAND_order1[cur].size(); j++)
	//		{
	//			if (visited3[CAND_order1[cur][j]] != temp1)
	//			{
	//				for (vector<int>::iterator it = CAND_order1[cur].begin(); it != CAND_order[cur].end(); )
	//				{
	//					if (*it == CAND_order1[cur][j])
	//					{
	//						it = CAND_order1[cur].erase(it); //����д��arr.erase(it);
	//						break;
	//					}
	//					else
	//					{
	//						++it;
	//					}
	//				}
	//			}
	//		}
	//		if (CAND_order1[cur].size()==0)
	//		{
	//			stop_update = 0;
	//			return;
	//		}
	//	}

	//}

	

	for (int i = 0; i <ORDER2.size(); i++)
	{
		int cur = ORDER2[i];
		if (BFS_child[cur].size() == 0)
		{
			continue;
		}
		int temp1 = 0;

		memset(visited3, 0, sizeof(int) * cnt_node);

		for (int j = 0; j < BFS_child[cur].size(); j++)
		{
			int child = BFS_child[cur][j];

			if (verify_query(cur, child, id))
			{
				temp1++;
				for (int k = 0; k < CAND_order1[cur].size(); k++)
				{

					for (int m = 0; m < CAND_order1[child].size(); m++)
					{
						int cc = 0;
						for (int p = 0; p < nodes[CAND_order1[child][m]].size(); p++)
						{
							if (nodes[CAND_order1[child][m]][p] == CAND_order1[cur][k])
							{

								visited3[k]++; cc = 1;
								break;
							}
						}
						if (cc==1)
						{
							break;
						}
					}

				}
			}

		}


		if (temp1>0)
		{

			for (int j = 0; j < CAND_order1[cur].size(); j++)
			{
				if (visited3[j] != temp1)
				{
					//for (vector<int>::iterator it = CAND_order1[cur].begin(); it != CAND_order[cur].end(); )
					//{
					//	if (*it == CAND_order1[cur][j])
					//	{
					//		it = CAND_order1[cur].erase(it); //����д��arr.erase(it);
					//		break;
					//	}
					//	else
					//	{
					//		++it;
					//	}
					//}
					auto iter = CAND_order1[cur].erase(CAND_order1[cur].begin() + j);
				}
			}
			if (CAND_order1[cur].size() == 0)
			{
				stop_update = 0;
				return;
			}
		}

	}



	for (int i = 0; i <ORDER1.size(); i++)
	{
		int temp1 = 0;
		int cur = ORDER1[i];

		memset(visited3, 0, sizeof(int) * CAND_order1[cur].size());

		for (int j = 0; j < BFS_parent[cur].size(); j++)
		{
			int parent = BFS_parent[cur][j];

			if (verify_query(cur, parent, id))
			{
				temp1++;
				for (int k = 0; k < CAND_order1[cur].size(); k++)
				{

					for (int m = 0; m < CAND_order1[parent].size(); m++)
					{
						int cc = 0;
						for (int p = 0; p < nodes[CAND_order1[parent][m]].size(); p++)
						{
							if (nodes[CAND_order1[parent][m]][p] == CAND_order1[cur][k])
							{

								visited3[k]++; cc = 1;
								break;
							}
						}
						if (cc==1)
						{
							break;
						}
					}

				}
			}

		}

		if (BFS_parent[cur][0] == -1)
		{
			for (int j = 0; j < CAND_order1[cur].size(); j++)
			{
				CAND_order[cur].push_back(CAND_order1[cur][j]);
			}
		}

		if (temp1>0)
		{

			for (int j = 0; j < CAND_order1[cur].size(); j++)
			{
				if (visited3[j] != temp1)
				{
					auto iter = CAND_order1[cur].erase(CAND_order1[cur].begin() + j);
					//for (vector<int>::iterator it = CAND_order1[cur].begin(); it != CAND_order[cur].end();)
					//{
					//	if (*it == CAND_order1[cur][j])
					//	{
					//		it = CAND_order1[cur].erase(it); //����д��arr.erase(it);
					//		break;
					//	}
					//	else
					//	{
					//		++it;
					//	}
					//}
				}
			}

			if (CAND_order1[cur].size() == 0)
			{
				stop_update = 0;
				return;
			}
			for (int j = 0; j < CAND_order1[cur].size(); j++)
			{
				CAND_order[cur].push_back(CAND_order1[cur][j]);
			}


		}

	}

	CAND_order1.clear();



	/**********************************************��ȡÿһ���ڵ��Ȩ��***************************************************************************/
	vector<int>query_temp;
	for (int i = 0; i < cnt_node_query; i++)
	{
		if (CAND_order[i].size()>0)//���ѯ��صĽڵ�
		{
			query_temp.push_back(i);
		}
	}
	int qqq = query_temp.size();


	float*weight = new float[cnt_node_query];
	memset(weight, 0, sizeof(float) * cnt_node_query);
	for (int i = 0; i < qqq; i++)
	{
		if (query_temp[i] == query_parent)
		{
			continue;
		}
		if (query_temp[i] == query_child)
		{
			continue;
		}
		weight[query_temp[i]] = CAND_order[query_temp[i]].size() / query_nodes[query_temp[i]].size();
	}
	memset(visited, 0, sizeof(int) * cnt_node_query);
	visited[query_parent] = 1;
	visited[query_child] = 1;


	while (matching_order.size() != qqq)
	{
		vector<int>order_choice;
		
		for (int i = 0; i < cnt_node_query; i++)
		{
			if (visited[i] == 0&& CAND_order[i].size()>0)
			{
				for (int j = 0; j <matching_order.size(); j++)
				{
					int m = 0;
					for (int k = 0; k < query_nodes[matching_order[j]].size(); k++)
					{
						if (query_nodes[matching_order[j]][k] == i&&verify_query(query_nodes[matching_order[j]][k], matching_order[j], id))
						{
							order_choice.push_back(i); m = 1;
							break;
						}
					}
					if (m == 1)
					{
						break;
					}
				}
			}
		}
		if (order_choice.size()==0)
		{
			return;
		}
		float temp = weight[order_choice[0]];
		for (int i = 1; i < order_choice.size(); i++)
		{
			if (weight[order_choice[i]] <= temp)
			{
				temp = weight[order_choice[i]];
			}
		}
		for (int i = 0; i < order_choice.size(); i++)
		{
			if (temp == weight[order_choice[i]]&& visited[order_choice[i]]==0) {
				visited[order_choice[i]] = 1;
				matching_order.push_back(order_choice[i]);
				break;
			}
		}
	}

	
/*****************************************����ƥ����*******************************************************************/
	
	
	vector<vector<int> > matches;//�洢��ѯƥ����
	vector<vector<int> > matches_candidate;//�洢��ѯƥ����
	vector < pair<int, int> > Candidate;
	int cur_order = 1;
	matches.resize(1);
	matches_candidate.resize(1);
	matches[0].push_back(data_parent);
	matches[0].push_back(data_child);
	memset(visited, 0, sizeof(int) * cnt_node_query);
	visited[query_parent] = 1;
	visited[query_child] = 1;
	int cnt;
	while (cur_order!=qqq-1)
	{
		vector<pair<int,int> >().swap(Candidate);
		cur_order++;
		int cur_node = matching_order[cur_order];
		if (cur_order%2==0)
		{
			for (int it = 0; it < CAND_order[cur_node].size(); it++)
			{
				int CC = 0;
				
				for (int i = 0; i < matches.size(); i++)
				{
					cnt = 0;
					for (int j = 0; j < query_nodes[cur_node].size(); j++)
					{
						
						if (verify_query(cur_node, query_nodes[cur_node][j],id)&&visited[query_nodes[cur_node][j]] == 1)
						{
							cnt++;
							for (int k = 0; k <matching_order.size(); k++)
							{
								int m = 0;
								if (matching_order[k] == query_nodes[cur_node][j])
								{
									for (int l = 0; l < nodes[CAND_order[cur_node][it]].size(); l++)
									{
										if (nodes[CAND_order[cur_node][it]][l] == matches[i][k])
										{
											CC++; m = 1;
											break;
										}
									}
								}
								if (m==1)
								{
									break;
								}
							}
						}
					}
					if (cnt == CC)
					{
						Candidate.push_back(make_pair(i, CAND_order[cur_node][it]));
					}
				}
			}
			vector<vector<int> >().swap(matches_candidate);
			matches_candidate.resize(Candidate.size());
			pair<int, int> pos;
			int start;
			int end;
			for (int a = 0;  a< Candidate.size(); a++)
			{
				pos = Candidate[a];
				start = pos.first;
				end = pos.second;
				for (int b = 0; b < matches[start].size(); b++)
				{
					matches_candidate[a].push_back(matches[start][b]);
				}
				matches_candidate[a].push_back(end);
			}
		}
		else
		{
			for (int it = 0; it < CAND_order[cur_node].size(); it++)
			{
				int CC = 0;
				for (int i = 0; i < matches_candidate.size(); i++)
				{
					cnt = 0;
					for (int j = 0; j < query_nodes[cur_node].size(); j++)
					{
						
						if (verify_query(cur_node, query_nodes[cur_node][j], id)&&visited[query_nodes[cur_node][j]] == 1)
						{
							cnt++;
							for (int k = 0; k <matching_order.size(); k++)
							{
								int m = 0;
								if (matching_order[k] == query_nodes[cur_node][j])
								{
									for (int l = 0; l < nodes[CAND_order[cur_node][it]].size(); l++)
									{
										if (nodes[CAND_order[cur_node][it]][l] == matches_candidate[i][k])
										{
											CC++; m = 1;
											break;
										}
									}
								}
								if (m == 1)
								{
									break;
								}
							}
						}
					}
					if (cnt == CC)
					{
						Candidate.push_back(make_pair(i, CAND_order[cur_node][it]));
					}
				}
			}
			vector<vector<int> >().swap(matches);
			matches.resize(Candidate.size());
			pair<int, int> pos;
			int start;
			int end;
			for (int a = 0; a< Candidate.size(); a++)
			{
				pos = Candidate[a];
				start = pos.first;
				end = pos.second;
				for (int b = 0; b < matches_candidate[start].size(); b++)
				{
					matches[a].push_back(matches_candidate[start][b]);
				}
				matches[a].push_back(end);
			}
		}
	}

	CAND_order.clear();


	if ((qqq - 1) % 2 == 0)
	{
		match_cur = matches_candidate.size();
	}
	else
	{
		match_cur = matches.size();
	}


	if (output)
	{
		cout << "Edge insertion and matched query edge: " << endl;


		cout << "(" << data_parent << ", " << data_child << ") " << "and " << "(" << query_parent << ", " << query_child << ") " << "and " << id;
		cout << endl;
		cout << "Match order: " << endl;
		for (int i = 0; i < qqq; i++)
		{
			cout << matching_order[i] << " ";
		}
		cout << endl;

		if ((qqq - 1) % 2 == 0)
		{
			match_result = matches_candidate.size();
			total_result = total_result + match_result;
			cout << "Match result: " << match_result << endl;
			/*************���ƥ����************************/
			for (int i = 0; i < match_result; i++)
			{
				cout << "MatchResult " << i << ": ";
				for (int j = 0; j <matches_candidate[i].size(); j++)
				{
					cout << matches_candidate[i][j] << " ";
				}
				cout << endl;
			}
		}
		else
		{
			match_result = matches.size();
			total_result = total_result + match_result;
			cout << "Match result: " << match_result << endl;
			/*************���ƥ����************************/
			for (int i = 0; i < match_result; i++)
			{
				cout << "MatchResult " << i << ": ";
				for (int j = 0; j <matches[i].size(); j++)
				{
					cout << matches[i][j] << " ";
				}
				cout << endl;
			}
		}
	}
}






inline void updateUP(int data_parent, int data_child, int query_parent, int query_child, int id) {
	int s2 = 0;
	int s3 = 0;
	for (int i = 0; i < BFS_child[query_parent].size(); i++)
	{
		
		if (BFS_child[query_parent][i] ==query_child)
		{
			continue;
		}
		else
		{
			int child = BFS_child[query_parent][i];
			
			if (verify_query(child, query_parent, id))
			{
				s3++;
				int s1 = 0;
				for (int j = 0; j < CAND[child].size(); j++)
				{
					
					if (stree_or_match[child][j][id][0]==1)
					{
						for (int k = 0; k < nodes[CAND[child][j]].size(); k++)
						{
							if (nodes[CAND[child][j]][k]== data_parent)
							{
								s1++; s2++;
								break;
							}
							
						}
					}
					if (s1>0)
					{
						break;
					}
				}

			}

		}
	}
	if (s2== s3)
	{
		int mm = 0;
		for (int i = 0; i < CAND[query_parent].size(); i++)
		{
			if (CAND[query_parent][i] == data_parent)
			{
				mm = i;
				break;
			}
		}
		stree_or_match[query_parent][mm][id][0] = 1;

			for (int i = 0; i < BFS_parent[query_parent].size(); i++)
			{
				int parent = BFS_parent[query_parent][i];

				if (verify_query(parent, query_parent, id)) {
					for (int j = 0; j < CAND[parent].size(); j++)
					{
						for (int k = 0; k < nodes[CAND[parent][j]].size(); k++)
						{
							if (nodes[CAND[parent][j]][k]==data_parent&& stree_or_match[parent][j][id][0] == 0)
							{
								Q_UP.push(make_pair(CAND[parent][j], data_parent));
								Q_UP_location.push(make_pair(parent, query_parent));
								break;
							}
						}
					}
					//for (int j = 0; j < nodes[data_parent].size(); j++)
					//{
					//	if (nodes_label[nodes[data_parent][j]] == nodes_label_query[parent])
					//	{
					//		for (int k = 0; k < CAND[parent].size(); k++)
					//		{
					//			if (CAND[parent][k] == nodes[data_parent][j] && stree_or_match[parent][k][id][0] == 0)
					//			{
					//				Q_UP.push(make_pair(nodes[data_parent][j], data_parent));
					//				Q_UP_location.push(make_pair(parent, query_parent));

					//				//updateUP(nodes[data_parent][j], data_parent, parent, query_parent, id);
					//			}
					//		}
					//	}
					//}
				}
			}
		//if (BFS_parent[query_parent][0] == -1)
		//{
		//	stree_or_match[query_parent][mm][id][1] = 1;

		//}
		if (query_parent_verify(query_parent,id) )//�ж��Ƿ�Ϊ��ʼ�ڵ�
		{
			stree_or_match[query_parent][mm][id][1] = 1;
			int s = 0; int sss = 0;
			for (int i = 0; i < BFS_child[query_parent].size(); i++)
			{
				int child = BFS_child[query_parent][i];
				if (verify_query(child, query_parent, id)) {
					for (int j = 0; j < CAND[child].size(); j++)
					{
						for (int k = 0; k < nodes[CAND[child][j]].size(); k++)
						{
							if (nodes[CAND[child][j]][k] == data_parent&& stree_or_match[child][j][id][0] == 1 && stree_or_match[child][j][id][1] == 0)
							{
								Q_DOWN.push(make_pair(data_parent, CAND[child][j]));
								Q_DOWN_location.push(make_pair(query_parent, child));
								break;
							}
						}
					}
				}
			}
		}
		if (query_parent_verify(query_parent, id)==false)
		{
			    int s = 0; int sss = 0;
				for (int i = 0; i < BFS_parent[query_parent].size(); i++)
				{
					int parent = BFS_parent[query_parent][i];
					if (verify_query(parent, query_parent, id)) {
						sss++;
						int ss = 0;
						for (int j = 0; j < CAND[parent].size(); j++)
						{
							if (stree_or_match[parent][j][id][1] == 1)
							{
								for (int k = 0; k < nodes[CAND[parent][j]].size(); k++)
								{
									if (nodes[CAND[parent][j]][k] == data_parent)
									{
										s++; ss++;
										break;
									}
								}
							}
							if (ss > 0)
							{
								break;
							}
						}
					}
				}
					if (s == sss)
					{
						int mm = 0;
						for (int i = 0; i < CAND[query_parent].size(); i++)
						{
							if (CAND[query_parent][i] == data_parent)
							{
								mm = i;
								break;
							}
						}
						stree_or_match[query_parent][mm][id][1] = 1;
						for (int i = 0; i < BFS_child[query_parent].size(); i++)
						{
							int child = BFS_child[query_parent][i];
							if (verify_query(child, query_parent, id)) {
								for (int j = 0; j < CAND[child].size(); j++)
								{
									for (int k = 0; k < nodes[CAND[child][j]].size(); k++)
									{
										if (nodes[CAND[child][j]][k] == data_parent&& stree_or_match[child][j][id][0] == 1 && stree_or_match[child][j][id][1] == 0)
										{
											Q_DOWN.push(make_pair(data_parent, CAND[child][j]));
											Q_DOWN_location.push(make_pair(query_parent, child));
											break;
										}
									}
								}
							}
						}
					}
				
			}
				

		//else
		//{
		//	int mm = 0;
		//	for (int i = 0; i < CAND[query_parent].size(); i++)
		//	{
		//		if (CAND[query_parent][i] == data_parent)
		//		{
		//			mm = i;
		//			break;
		//		}
		//	}
		//	stree_or_match[query_parent][mm][id][1] = 1;
		//	
		//	for (int i = 0; i < BFS_child[query_parent].size(); i++)
		//	{
		//		int child = BFS_child[query_parent][i];
		//		if (verify_query(child, query_parent, id)) {
		//			for (int j = 0; j < CAND[child].size(); j++)
		//			{
		//				for (int k = 0; k < nodes[CAND[child][j]].size(); k++)
		//				{
		//					if (nodes[CAND[child][j]][k] == data_parent&& stree_or_match[child][j][id][0] == 1 && stree_or_match[child][j][id][1] == 0)
		//					{
		//						Q_DOWN.push(make_pair(nodes[CAND[child][j]][k], data_parent));
		//						Q_DOWN_location.push(make_pair(child, query_parent));
		//						break;
		//					}
		//				}
		//			}


		//			//for (int j = 0; j < nodes[data_parent].size(); j++)
		//			//{
		//			//	if (nodes_label[nodes[data_parent][j]] == nodes_label_query[child])
		//			//	{
		//			//		for (int k = 0; k < CAND[child].size(); k++)
		//			//		{
		//			//			if (CAND[child][k] == nodes[data_parent][j] && stree_or_match[child][k][id][0] == 1 && stree_or_match[child][k][id][1] == 0)
		//			//			{
		//			//				Q_DOWN.push(make_pair(nodes[data_parent][j], data_parent));
		//			//				Q_DOWN_location.push(make_pair(child, query_parent));

		//			//				//updateUP(nodes[data_parent][j], data_parent, parent, query_parent, id);
		//			//			}
		//			//		}
		//			//	}
		//			//}
		//		}
		//	}
		//}
	}
}





 inline void update_MDCG(int data_parent, int data_child, int query_parent, int query_child) {
	
	vector<int>corres_query;

	for (int i = 0; i < query_num; i++)
	{
		if (verify_query(query_parent, query_child, i))
		{
			corres_query.push_back(i);
		}
	}




	for (int i = 0; i < corres_query.size(); i++)
	{
		int query1 = corres_query[i];
		parent_stree = 0; parent_match = 0;
		child_stree = 0;  child_match = 0;
		for (int j = 0; j < CAND[query_parent].size(); j++)
		{
			if (CAND[query_parent][j] == data_parent)
			{
				parent_stree = stree_or_match[query_parent][j][query1][0];
				parent_match = stree_or_match[query_parent][j][query1][1];
				break;
			}
		}
		for (int j = 0; j < CAND[query_child].size(); j++)
		{
			if (CAND[query_child][j] == data_child)
			{
				child_stree = stree_or_match[query_child][j][query1][0];
				child_match = stree_or_match[query_child][j][query1][1];
				break;
			}
		}
/*********************************************************Case 1: ���������MDCG���м������Ӱ��********************************************************************/
		if (child_stree == 0)
		{
			continue;
		}
		if (child_stree == 1 && parent_stree == 1 && parent_match == 0)
		{
			continue;
		}
		if (parent_match == 1 && child_match == 1)
		{
			GetMatch(data_parent, data_child, query_parent, query_child, query1);
			return;
		}
		if (child_stree == 1 && parent_match == 1&& BFS_parent[query_child].size()>1)
		{
			int s1 = 0;
			for (int k = 0; k < BFS_parent[query_child].size(); k++)
			{
				
				if (BFS_parent[query_child][k]== query_parent)
				{
					continue;
				}
				else
				{
					if (verify_query(query_child, BFS_parent[query_child][k], query1))
					{
						int  parent = BFS_parent[query_child][k];
						int s2 = 0;
						for (int m = 0; m < CAND[parent].size(); m++)
						{
							if (stree_or_match[parent][m][query1][1] == 1)
							{
								for (int p = 0; p < nodes[CAND[parent][m]].size(); p++) {
									if (nodes[CAND[parent][m]][p] == data_child)
									{
										s2++;
										s1++;
										break;
									}

								}
							}
							if (s2>0)
							{
								break;
							}
						}
					}
				}
			}
			if (s1!= BFS_parent[query_child].size()-1)
			{
				continue;
			}
		}
/*********************************************************Case 2: ����stree*******************************************************************************/
		if (child_stree == 1 && parent_stree == 0) {
			updateUP(data_parent, data_child, query_parent, query_child, query1);
		}

/*********************************************************Case 3: ����match*******************************************************************************/
		if (parent_match == 1 && child_stree == 1 && child_match == 0)
		{
			
			updateDOWN(data_parent, data_child, query_parent, query_child, query1);
		}
		pair<int, int> pos;
		pair<int, int> pos1;
		while (Q_UP.empty() != true)
		{
			pos = Q_UP.front();
			Q_UP.pop();
			pos1 = Q_UP_location.front();
			Q_UP_location.pop();
			updateUP(pos.first, pos.second, pos1.first, pos1.second, query1);
		}
		while (Q_DOWN.empty() != true)
		{
			pos = Q_DOWN.front();
			Q_DOWN.pop();
			pos1 = Q_DOWN_location.front();
			Q_DOWN_location.pop();
			updateDOWN(pos.first, pos.second, pos1.first, pos1.second, query1);
		}
		

		for (int j = 0; j < CAND[query_parent].size(); j++)
		{
			if (CAND[query_parent][j] == data_parent)
			{
				parent_match = stree_or_match[query_parent][j][query1][1];
				break;
			}
		}
		for (int j = 0; j < CAND[query_child].size(); j++)
		{
			if (CAND[query_child][j] == data_child)
			{
				child_match = stree_or_match[query_child][j][query1][1];
				break;
			}
		}
		if (child_match == 1 && parent_match == 1)
		{
			GetMatch(data_parent, data_child, query_parent, query_child, query1);
			if (stop_update==0)
			{
				stop_update = -1;
				continue;
			}
			
		}
	}
}



















inline void update_processing(string insertionFile) {

	ifstream fin(insertionFile);
	string line;
	vector<string> v;
	///edge_label.clear();
	//int insertion_num = 0;
	int num = 0;
	while (getline(fin, line))
	{
		match_num = 0;
		match_cur = 0;
		//insertion_num++;
		if (line.at(0) == 'e')
		{
			split(line, v, ' ');
			int left_insertion_node = atoi(v[1].c_str());
			int right_insertion_node = atoi(v[2].c_str());
			if (left_insertion_node != right_insertion_node) {//put the nodes into the adjacent list�����ݱ������ڽӱ�
				nodes[left_insertion_node].push_back(right_insertion_node);
				nodes[right_insertion_node].push_back(left_insertion_node);
			}
			else
			{
				continue;
			}
			for (int i = 0; i < cnt_node_query; i++)
			{
				if (nodes_label_query[i] == nodes_label[left_insertion_node])
				{
					for (int j = 0; j < query_nodes[i].size(); j++)
					{
						if (nodes_label_query[query_nodes[i][j]] == nodes_label[right_insertion_node])
						{
							int cp = 0;
							for (int k = 0; k < BFS_parent[query_nodes[i][j]].size(); k++)
							{
								if (BFS_parent[query_nodes[i][j]][k]==i)
								{
									cp++;
									update_MDCG(left_insertion_node, right_insertion_node, i, query_nodes[i][j]);
								}
							
							}
							if (cp==0)
							{
								update_MDCG(right_insertion_node, left_insertion_node, query_nodes[i][j], i);

							}
						}
					}
				}
			}
			if (!output&&match_num>0)
			{
				cout << "Edge:  " << "(" << left_insertion_node << ", " << right_insertion_node << ") " << "causes  " << match_num << "  results" << endl;
			}
		}
	}
	//if (num==0)
	//{
	//	cout << "There exists no new caused matching results !!!"  << endl;
	//}
	fin.close();
}

int main(int argc, char *argv[]) {
	//int a = 0;
	std::string datagraphFile, querygraphFile, insertionFile;
	std::chrono::high_resolution_clock::time_point start, start_index;

	for (int i = 1; i < argc; i++) {
		//cout << "change datasets path" << endl;
		if (string(argv[i]) == "-q")
			querygraphFile = argv[i + 1];
		else if (string(argv[i]) == "-d")
			datagraphFile = argv[i + 1];
		else if (string(argv[i]) == "-u")
			insertionFile = argv[i + 1];
		//else if (string(argv[i]) == "-c")  // ��ʾѡȡ������ǰ��Ķ����У�Ĭ��������
			//cnum = atoi(argv[i + 1]);
	}

	/*datagraphFile = argv[1]; 
	querygraphFile = argv[2];
	insertionFile = argv[3]; */
	//insertion_num_need = argv[4];
 	//string option = argv[4];

	/*if (option == "-f")
		output = false;
	else if (option == "-fo")
		output = true;
	else {
		cerr << "Please enter the correct parameter!" << endl;
		return 0;
	}*/
	readDataGraph(datagraphFile);
	//readQueryGraph(querygraphFile);
	// int bg_curr_pid = GetCurrentPid();
	// float bg_memory_usage = GetMemoryUsage(bg_curr_pid);
	constructAQG(querygraphFile);
	labelAQG(querygraphFile);
	coreDecomposition_query();
	isTree = true;
	// int ed_cur_pid = GetCurrentPid();
	// float ed_memory_usage = GetMemoryUsage(ed_cur_pid);
	//stringstream ss;
	//ss << insertion_num_need;
	//ss >> number;
	//key_index.resize(7000000);
	for (int i = 0; i < cnt_node_query; i++)
		if (core_number_query[i] >= 2) {
			isTree = false;
			break;
		}

	//clock_t time = clock();
	if (isTree)
	{
		root_node_id = start_node_selection_Tree();
		BFS_TREE();
		constructMDCG();
		update_processing(insertionFile);
	}
	else
	{
		root_node_id = start_node_selection_NORMAL();
		BFS_PARENT_CHILD();
		constructMDCG();
		update_processing(insertionFile);
	}
	//cerr << "total_match_result: " << total_result << endl;
	//HANDLE handle = GetCurrentProcess();
	//PROCESS_MEMORY_COUNTERS pmc;
	//GetProcessMemoryInfo(handle, &pmc, sizeof(pmc));
	//printf("%d\r\n", pmc.WorkingSetSize);  //������浥λ��B�����Գ���1000����Ϊkb��ʽ
	//return 0;
}