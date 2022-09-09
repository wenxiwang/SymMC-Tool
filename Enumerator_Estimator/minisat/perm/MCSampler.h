//
// Created by huyang on 1/9/20.
//

#ifndef CPP_MCSAMPLER_H
#define CPP_MCSAMPLER_H

#include "dep.h"
#include "type.h"
#include "PermPair.h"
#include "BasicPermPair.h"

extern size_t model_indx;
extern unordered_map<size_t, int> model_indx_map;
extern vector<int> parent;
extern vector<int> rank_;
extern vector<double> mu;

extern size_t timeout; //(in seconds)

extern mpf_class MIX_PARAPERM_NUM; // user set
extern mpf_class MIN_COMB_NUM; // user set
extern mpf_class MAX_COMB_NUM; // user set
extern mpf_class MAX_CAPACITY; // 10!
extern mpf_class ADD_SR;
extern mpf_class EXP_COMB_SR;

extern mpf_class real_comb_sr;

extern size_t min_perm_num;
extern size_t max_perm_num;
extern double exp_perm_sr;

extern unordered_set<size_t> nonIsomSolSets;
extern mpf_class sampledPermNum;

extern mpf_class mcPSB; // model count under partial symmetry breaking
extern mpf_class mcFSB; // model count under full symmetry breaking
extern mpf_class mcNSB; // model count under no symmetry breaking

extern int is_sampling;
extern bool isNonIsom;
extern mpf_class ratioNSB;
extern mpf_class ratioFSB;
extern mpf_class isomSolCnt;
extern pthread_mutex_t lock4batchvec;

extern bool noMoreTask;
extern int batch;
extern vector<string> batchvec;
extern vector<PermPair> ppList;
extern vector<PermPair> samp_ppList;
extern bool minisatRun;
extern mpf_class total_perm_num;
extern mpf_class global_mean;
extern mpf_class local_var_first;

//extern set<list<list<int>>> cycleList;
mpf_class factorial(size_t);
extern size_t sampSize;
extern size_t init_sampSize;
extern size_t sampSize_sofar;
extern list<vector<int>> numList;
extern list<BasicPermPair> basicPPList;

void parseBasicPerms(string&, list<vector<int>>&, list<BasicPermPair>&);
list<list<list<list<int>>>> getComboCycleList(list<vector<int>>&, L4&);
list<list<list<list<int>>>> GenAllCombo(L4&);
L3 GenAllPerms(vector<int>&, bool);
list<list<list<list<int>>>> SampPerms();
list<list<int>> genc4Oneperm(vector<int>&, vector<int>&);
L2P getTransPosList(list<list<list<list<int>>>>&);
void genPermList(L2P&, list<BasicPermPair>&);
void genSampPermList(L2P&, list<BasicPermPair>&, vector<PermPair>&);
void fillPermPair(int, int, list<BasicPermPair>&, PermPair&);
void genSolSets(string&);
void GenPermSols_CF(string& , size_t);
double GenPermSols_CN(string& org, size_t hash_value);
void GenPermSols_CF_sample(string& , size_t);

void checkSolSets();
bool clearAsstList();
void genPermList();
void numlist_shuffle(list<vector<int>>&);
size_t numlist_hash(list<vector<int>>&);
void getTotalPermNum(list<vector<int>>& numList);
void GenOnePermSol(string& org, PermPair& pp);
void GenOnePermSol(string& org, size_t org_hash, PermPair& pp, unordered_set<size_t>& oneSolset);
list<string> split(string c, string k);
extern unordered_set<size_t> sampIdxSet;
extern unordered_set<size_t> comb_hash_set;

extern unordered_set<size_t> sampIdxSet_sofar;
extern unordered_set<size_t> comb_hash_set_sofar;

extern vector<int> shuffle_indx_vector;

#endif //CPP_MCSAMPLER_H
