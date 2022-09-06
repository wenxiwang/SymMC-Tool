#include "MCSampler.h"

#include <sys/resource.h>


size_t AlloySolNum = 0;
size_t model_indx = 0;
unordered_map<size_t, int> model_indx_map;
vector<int> parent;
vector<int> rank_;
vector<double> mu;

size_t timeout = 5000; //(in seconds)
int is_sampling = 0;
bool hasIdentical = false;
bool isNonIsom = true;
bool minisatRun;
bool sampSizeUpdated = false;

mpf_class EXP_COMB_SR = 0.5;
mpf_class MIX_PARAPERM_NUM= 2000; // user set
mpf_class MIN_COMB_NUM = 2000; // user set
mpf_class MAX_COMB_NUM = 100000; // user set
mpf_class MAX_CAPACITY = 5000000; 
mpf_class ADD_SR = 0.15;
size_t sampSize = 0;
size_t init_sampSize = 0;
size_t sampSize_sofar = 0;

mpf_class real_comb_sr;

mpf_class cntFSB = 0;
mpf_class cntNSB = 0;

// non-isomorphic solution set;
unordered_set<size_t> nonIsomSolSets;

// isomorphic solution count;
mpf_class isomSolCnt = 0;
mpf_class sampledPermNum = 0;
mpf_class ratioNSB;
mpf_class ratioFSB;
pthread_mutex_t lock4batchvec;
bool noMoreTask = false;
int batch;
vector<string> batchvec;
vector<PermPair> ppList;
vector<PermPair> samp_ppList;
mpf_class total_perm_num;
list<vector<int>> numList;
list<BasicPermPair> basicPPList;
mpf_class global_mean;
mpf_class dev_first;


unordered_set<size_t> sampIdxSet;
unordered_set<size_t> comb_hash_set;

vector<int> shuffle_indx_vector;
unordered_set<size_t> sampIdxSet_sofar;
unordered_set<size_t> comb_hash_set_sofar;


int find_op(vector<int>& parent, int x) {
   if(x != parent[x]) {
      parent[x] = find_op(parent, parent[x]);
   }

   return parent[x];
}

void union_op(vector<int>& parent, vector<int>& rank_, int x, int y) {
   x = find_op(parent, x);
   y = find_op(parent, y);
   if(rank_[x] > rank_[y]) {
      parent[y] = x;
   } else {
      parent[x] = y;
      if(rank_[x] == rank_[y]) {
         rank_[y] += 1;
      }
   }
}


void GenOnePermSol(string& org, size_t org_hash, PermPair& pp) {
    auto orgSol = org;
    bool changed = false;
    auto & first = pp.first;
    auto & second = pp.second;

    assert(first.size() == second.size());
    for (auto pair1 = first.begin(), pair2 = second.begin(); pair1 != first.end(); pair1++, pair2++) {
        assert((*pair1)->size() == (*pair2)->size());
        for (auto idx1 = (*pair1)->begin(), idx2 = (*pair2)->begin(); idx1 != (*pair1)->end(); idx1++, idx2++) {
            int indx1 = *idx1 - 1;
            int indx2 = *idx2 - 1;

            char c1 = orgSol[indx1];
            char c2 = orgSol[indx2];
            //if c1!=c2, then swap the two bits
            if (c1 != c2) {
                changed = true;
                //cout << "changed" << endl;
                orgSol[indx2] = c1;
                orgSol[indx1] = c2;
            }
        }
    }

    //cout << "perm: " << orgSol << endl;
    if (changed) {
        hash<string> str_hash;
        size_t perm_hash = str_hash(orgSol);
        
        if(model_indx_map.find(perm_hash) != model_indx_map.end()) {
	    	// the permuted model is one of the model under partial symmetry breaking,
	    	// thus the model is the node in the graph; 
		int from_node = model_indx_map[org_hash];
		int to_node = model_indx_map[perm_hash];
		if(find_op(parent, from_node) != find_op(parent, to_node)) {
         		union_op(parent, rank_, from_node, to_node);
        	}
	}
    }
}

void GenOnePermSol(string& org, size_t org_hash, PermPair& pp, unordered_set<size_t>& oneSolset) {
    auto orgSol = org;
    bool changed = false;
    auto & first = pp.first;
    auto & second = pp.second;

    assert(first.size() == second.size());
    for (auto pair1 = first.begin(), pair2 = second.begin(); pair1 != first.end(); pair1++, pair2++) {
        assert((*pair1)->size() == (*pair2)->size());
        for (auto idx1 = (*pair1)->begin(), idx2 = (*pair2)->begin(); idx1 != (*pair1)->end(); idx1++, idx2++) {
            int indx1 = *idx1 - 1;
            int indx2 = *idx2 - 1;

            char c1 = orgSol[indx1];
            char c2 = orgSol[indx2];
            //if c1!=c2, then swap the two bits
            if (c1 != c2) {
                changed = true;
                //cout << "changed" << endl;
                orgSol[indx2] = c1;
                orgSol[indx1] = c2;
            }
        }
    }

    //cout << "perm: " << orgSol << endl;
    if (changed) {
        hash<string> str_hash;
        size_t perm_hash = str_hash(orgSol);
        
        if(model_indx_map.find(perm_hash) != model_indx_map.end()) {
	    	// the permuted model is one of the model under partial symmetry breaking,
	    	// thus the model is the node in the graph; 
		int from_node = model_indx_map[org_hash];
		int to_node = model_indx_map[perm_hash];
		if(find_op(parent, from_node) != find_op(parent, to_node)) {
         		union_op(parent, rank_, from_node, to_node);
        	}
	}
	
	oneSolset.insert(perm_hash);
    }
    else 
    	oneSolset.insert(org_hash);

}

double GenPermSols_CN(string& org, size_t hash_value) {
    unordered_set<size_t> oneSolset;
    //oneSolset.insert(hash_value);
    
    if(is_sampling == 0) { 
        for (int i = 0; i < ppList.size(); i++) {
            PermPair& pp = ppList[i];
            auto orgSol = org;
            GenOnePermSol(orgSol, hash_value, pp, oneSolset);
        }
    }
    else if(is_sampling == 1){               
        for (int i = sampSize_sofar; i< sampSize_sofar + sampSize; i++) {
            int ppIdx = shuffle_indx_vector[i];           
            PermPair& pp = ppList[ppIdx];
            GenOnePermSol(org, hash_value, pp, oneSolset);
        }      
    }
    else { // is_sampling == 2
        for(int i=0; i< samp_ppList.size(); i++ ) {
            PermPair& pp = samp_ppList[i];
            GenOnePermSol(org, hash_value, pp, oneSolset);         
        }
    }
    
    int one_num = oneSolset.size();
    double mu = 0.0;
    if(is_sampling == 0 ) 
    	mu = (double) one_num / (double) total_perm_num.get_ui(); 
    else
        mu = (double) one_num / (double) sampSize;
    
    //cout << " mu: " << mu << endl;
    
    return mu;
}

void GenPermSols_CF(string& org, size_t hash_value) {
        
    if(is_sampling == 0) {   	
        for (int i = 0; i < ppList.size(); i++) {
            PermPair& pp = ppList[i];
            auto orgSol = org;
            GenOnePermSol(orgSol, hash_value, pp);
        }        
    }
    else if(is_sampling == 1){
        for (int i = sampSize_sofar; i< sampSize_sofar + sampSize; i++) {
            int ppIdx = shuffle_indx_vector[i];           
            PermPair& pp = ppList[ppIdx];
            GenOnePermSol(org, hash_value, pp);
        }    
    }
    else { // is_sampling == 2
        for(int i=0; i< samp_ppList.size(); i++ ) {
            PermPair& pp = samp_ppList[i];
            GenOnePermSol(org, hash_value, pp);         
        }
    }     

}


void genPermList(L2P& transposList, list<BasicPermPair>& basicPPList) {

    int i = 0;
    for (auto & tpairList : transposList) {
        PermPair perm;

        for (auto & pair : tpairList) {
            fillPermPair(pair.first, pair.second, basicPPList, perm);
        }

        ppList.emplace_back(std::move(perm));
        i++;
    }
    transposList.clear();
}

void genSampPermList(L2P& transposList, list<BasicPermPair>& basicPPList, vector<PermPair>& samp_ppList) {
    int i = 0;
    for (auto & tpairList : transposList) {
        PermPair perm;

        for (auto & pair : tpairList) {
            fillPermPair(pair.first, pair.second, basicPPList, perm);
        }

        samp_ppList.emplace_back(std::move(perm));
        i++;
    }
    transposList.clear();
}

void fillPermPair(int prev, int cur, list<BasicPermPair>& basicPPList, PermPair& perm) {

    for (auto & bpp : basicPPList) {
        list<int>& hp = bpp.getHeadPair();

        int prevIndex = 0;
        for (auto & tmp : hp) {
            if (prev == tmp) {
                break;
            }
            prevIndex++;
        }

        if(prevIndex >= hp.size()) {
            prevIndex = -1;
        }

        int curIndex = 0;
        for (auto & tmp : hp) {
            if (cur == tmp) {
                break;
            }
            curIndex++;
        }

        if(curIndex >= hp.size()) {
            curIndex = -1;
        }

        if (prevIndex != -1 && curIndex != -1) {
            perm.addFirst(&bpp.first);
            perm.addSecond(&bpp.second);
            break;
        }
    }
}

L2P getTransPosList(list<list<list<list<int>>>>& comboCycleList) {
    L2P transposList;
    for (auto & cl : comboCycleList) {
        //every p is the cycle notation of only one permutation of one number group
        LP tpairList;
        for (auto & p : cl) {
            // every pp is each cycle;
            for (auto & pp : p) {

                auto i = pp.rbegin();
                auto lastElm = *i;
                i++;

                for (; i != pp.rend(); i++) {
                    pair<int, int> tpair;

                    tpair.first = lastElm;

                    tpair.second = std::move(*i);
                    tpairList.push_back(std::move(tpair));
                }
            }
        }

        transposList.push_back(std::move(tpairList));
    }

    return std::move(transposList);
}

void getTotalPermNum(list<vector<int>>& numList) {
    total_perm_num = 1;
    for (auto & nums: numList) {
        // cout << nums.size() << endl;
        total_perm_num *= factorial(nums.size());
        //cout << "nums" << endl;
    }
   // cout << "end total perm num" << endl;
    //return total_perm_num;
}
list<list<list<list<int>>>> getComboCycleList(list<vector<int>>& numList, L4& l4perms) {

    // compute total num. of permutation
    list<list<list<list<int>>>> ccl;

    // generate all perms for each symmetry
    for (auto & nums: numList) {
    //cout << "here 111" << endl;
        L3 perms4nums = GenAllPerms(nums, false); // disable sampling
        l4perms.push_back(std::move(perms4nums));
    }
    ccl = GenAllCombo(l4perms);

    /*if(total_perm_num <= MIN_COMB_NUM) {
        // generate all perms for each symmetry
        for (auto & nums: numList) {
            //cout << "here 111" << endl;
            L3 perms4nums = GenAllPerms(nums, false); // disable sampling
            l4perms.push_back(std::move(perms4nums));
        }
        ccl = GenAllCombo(l4perms);
    } else {
        // sampling perms for each symmetry

        // Yang: update to bigfloat root in future
        is_sampling = true;
        ccl = SampPerms(numList, total_perm_num);
    }*/
    ccl = GenAllCombo(l4perms);
    mpf_class ccl_size = ccl.size();
    sampledPermNum = ccl_size;
    real_comb_sr = ccl_size / total_perm_num;
    return std::move(ccl);
}

unordered_map<size_t, mpf_class> fmap;
mpf_class factorial(size_t m) {
    if (fmap.find(m) == fmap.end()) {
        mpf_class f = 1;
        for(size_t i = 1; i <= m; i++) {
            mpf_class i_big = i;
            f *= i_big;
        }
        fmap[m] = f;
        return f;
    }

    return fmap[m];
}

// sampling alg.:
// http://bastian.rieck.me/blog/posts/2017/selection_sampling/
unordered_set<size_t> sampling(size_t N, size_t n) {
    //cout << "N:" << N << " n:" << n << endl;
    assert(N > n && n > 0);

    unordered_set<size_t> selected;

    random_device rd;
    default_random_engine e(rd());

    uniform_real_distribution<> U( 0, std::nextafter(1.0, std::numeric_limits<double>::max() ) );

    selected.reserve(n);
    for(size_t t = 0; t < N; t++ ) {
        if((N-t) * U( e ) < n - selected.size())
            selected.insert(t);

        if(selected.size() == n)
            break;
    }

    assert(selected.size() == n);
    return std::move(selected);
}

list<list<int>> genc4Oneperm(vector<int>& r, vector<int>& num) {
    list<list<int>> c4Oneperm;
    list<int> visited;

    for (auto i = num.begin(), ri = r.begin(); i != num.end(); i++, ri++) {
        list<int> cc;
        int select = *i;

        bool contains = false;
        for (auto &tmp : visited) {
            if (tmp == select) {
                contains = true;
                break;
            }
        }

        if (!contains) {
            int corespnd = *ri; //r.get(i);
            cc.emplace_back(select);
            visited.emplace_back(select);
            while (corespnd != select) {
                cc.emplace_back(corespnd);
                visited.emplace_back(corespnd);

                for (auto tmp1 = r.begin(), tmp2 = num.begin(); tmp2 != num.end(); tmp1++, tmp2++) {
                    if (*tmp2 == corespnd) {
                        corespnd = *tmp1;
                        break;
                    }
                }
            }
            if (cc.size() > 1)
                c4Oneperm.push_back(std::move(cc));
        }
    }

    return std::move(c4Oneperm);
}

L3 GenAllPerms(vector<int>& num, bool do_sampling) {
    //cout << "GenAllPerms" << endl;
    L3 cycles;
    mpf_class ori_perm_num = factorial(num.size());

        vector<int> r = num;
        do {
            list<list<int>> c4Oneperm = genc4Oneperm(r, num);
            cycles.push_back(std::move(c4Oneperm));

        } while (std::next_permutation(r.begin(), r.end()));
    return std::move(cycles);
}

inline void numlist_shuffle(list<vector<int>>& numList) {
    for(auto & num: numList) {
        random_shuffle(num.begin(), num.end());
    }
}

inline size_t numlist_hash(list<vector<int>>& numList) {
    hash<string> str_hash;

    vector<int> f;
    for(auto & num : numList) {
        for(auto & ele : num)
            f.push_back(ele);
    }

    std::stringstream r_ss;
    std::copy(f.begin(), f.end(), std::ostream_iterator<int>(r_ss, " "));

    return str_hash(r_ss.str());
}

list<list<list<list<int>>>> SampPerms() {
    //cout << "GenAllPerms" << endl;
    list<list<list<list<int>>>> ccl;
    std::srand(unsigned(std::time(0)));
    
    hash<string> str_hash;

    auto numListCopy = numList;
    while (comb_hash_set.size() < sampSize) {
        size_t hash_value = 0;
        do {
                numlist_shuffle(numListCopy);
                hash_value = numlist_hash(numListCopy);
            //cout << hash_value << endl;
        } while (comb_hash_set.find(hash_value) != comb_hash_set.end() && comb_hash_set_sofar.find(hash_value) != comb_hash_set_sofar.end() );

        comb_hash_set.insert(hash_value);
        L3 cycles;
        for(auto numShuffle = numListCopy.begin(), num = numList.begin();
        numShuffle != numListCopy.end(); numShuffle++, num++) {
            list<list<int>> c4Oneperm = genc4Oneperm(*numShuffle, *num);
            cycles.push_back(std::move(c4Oneperm));
        }

        ccl.push_back(std::move(cycles));

    }

    return std::move(ccl);
}

list<list<list<list<int>>>> GenAllCombo(L4& l4perms) {
    list<list<list<list<int>>>> combinations;
    auto element0 = l4perms.begin();

    for (auto & i: *element0) {
        list<list<list<int>>> newList;
        newList.emplace_back(i);
        combinations.push_back(std::move(newList));
    }

    auto nextList = ++l4perms.begin();
    for (; nextList != l4perms.end(); nextList++) {
        list<list<list<list<int>>>> newCombinations;

        for (list<list<list<int>>>& first : combinations) {
            //for(list<list<list<int>>> first: combinations) {
            for (auto & second : *nextList) {
                //for(list<list<int>> second: nextList) {
                list<list<list<int>>> newList;

                // combine
                /*for (auto & tmp : first) {
                    newList.emplace_back(tmp);
                }*/
                newList = first;

                newList.emplace_back(second);
                newCombinations.push_back(std::move(newList));
            }
            first.clear();
        }
        combinations = std::move(newCombinations);
       // nextList->clear();

    }

    return std::move(combinations);
}


void parseBasicPerms(string& permF, list<vector<int>>& numList, list<BasicPermPair>& basicPPList) {
    ifstream permFile(permF);
    string line;
    while (getline(permFile, line)) {
        //cout << line << endl;
        if (!line.empty() && line[0] == '*') {

            istringstream iss (line.substr(1));

            vector<int> nums;
            while (iss.rdbuf()->in_avail() > 0) {
                int ipt = 1;
                char comma = -1;
                iss >> ipt >> comma;
                nums.emplace_back(ipt);
            }
            numList.push_back(std::move(nums));
        } else {
            istringstream iss (line);
            BasicPermPair bpp;
            while(iss.rdbuf()->in_avail() > 0) {
                int first = -1;
                char comma = -1;
                int second = -1;

                iss >> first >> comma >> second;
                if(bpp.headPair.empty()) {
                    bpp.headPair.emplace_back(first);
                    bpp.headPair.emplace_back(second);
                }
                else {
                    if(comma == ',') {
                        bpp.first.emplace_back(first);
                        bpp.second.emplace_back(second);
                    }
                }
            }

            basicPPList.push_back(std::move(bpp));
        }
    }
}

