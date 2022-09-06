/*****************************************************************************************[Main.cc]
Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
Copyright (c) 2007,      Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/
  
#include <errno.h>
#include <zlib.h>
#include <signal.h>
#include "minisat/utils/System.h"
#include "minisat/utils/ParseUtils.h"
#include "minisat/utils/Options.h"
#include "minisat/core/Dimacs.h"
#include "minisat/simp/SimpSolver.h"
#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <pthread.h>
#include <iomanip>
#include <unistd.h>
#include <sys/resource.h>

#include "minisat/perm/MCSampler.h"  

using namespace Minisat;

//=================================================================================================


static Solver* solver;
// Terminate by notifying the solver and back out gracefully. This is mainly to have a test-case
// for this feature of the Solver as it may take longer than an immediate call to '_exit()'.
static void SIGINT_interrupt(int) { solver->interrupt(); }
int primary_var;

// Note that '_exit()' rather than 'exit()' has to be used. The reason is that 'exit()' calls
// destructors and may cause deadlocks if a malloc/free function happens to be running (these
// functions are guarded by locks for multithreaded use).
static void SIGINT_exit(int) {
    printf("\n"); printf("*** INTERRUPTED ***\n");
    if (solver->verbosity > 0){
        solver->printStats();
        printf("\n"); printf("*** INTERRUPTED ***\n"); }
    _exit(1); }

void printStats(Solver& solver)
{
    double cpu_time = cpuTime();
    double mem_used = memUsedPeak();
    printf("restarts              : %"PRIu64"\n", solver.starts);
    printf("conflicts             : %-12"PRIu64"   (%.0f /sec)\n", solver.conflicts   , solver.conflicts   /cpu_time);
    printf("decisions             : %-12"PRIu64"   (%4.2f %% random) (%.0f /sec)\n", solver.decisions, (float)solver.rnd_decisions*100 / (float)solver.decisions, solver.decisions   /cpu_time);
    printf("propagations          : %-12"PRIu64"   (%.0f /sec)\n", solver.propagations, solver.propagations/cpu_time);
    printf("conflict literals     : %-12"PRIu64"   (%4.2f %% deleted)\n", solver.tot_literals, (solver.max_literals - solver.tot_literals)*100 / (double)solver.max_literals);
    if (mem_used != 0) printf("Memory used           : %.2f MB\n", mem_used);
    printf("CPU time              : %g s\n", cpu_time);
}


//=================================================================================================
void genSol(vec<Lit>&  blocking_clause) {
    blocking_clause.clear();
    std::string line = "";
    for (int i = 0; i < primary_var; i++) {
        blocking_clause.push(mkLit(i, solver->modelValue(i) == l_True));
        if(solver->modelValue(i) == l_True) {
            line += "1";
        }
        else {
            line += "0";
        }
    }

    batchvec.emplace_back(line);
    solver->addClause_(blocking_clause);
}



void genSolsRealSingle() {

    vec<Lit> blocking_clause;
    while(solver->solve()) {
        blocking_clause.clear();
        std::string line = "";
        for (int i = 0; i < primary_var; i++) {
            blocking_clause.push(mkLit(i, solver->modelValue(i) == l_True));
            if(solver->modelValue(i) == l_True) {
                line += "1";
            }
            else {
                line += "0";
            }
        }

        AlloySolNum++;

        solver->addClause_(blocking_clause);
    }
}

bool genBatchSols() {
    //cout << "genBatchSols" << endl;
    vec<Lit>  blocking_clause;
    //batchvec.clear();
    int cnt=0;
    while (cnt < batch) {
        if(solver->solve()) {
            genSol(blocking_clause);
            cnt++;
        }
        else
            return true;
    }
    return false;
}

void* generatePerms(char* permFile) {

    string permF(permFile);
    parseBasicPerms(permF, numList, basicPPList);
    // get the total permutation number "total_perm_num"
    getTotalPermNum(numList);
    
    // decide the sampling mode based on MIN_COMB_NUM and MAX_CAPACITY
    //cout << "total_perm_num: " << total_perm_num << endl;

    if(total_perm_num <= MIN_COMB_NUM) {
        is_sampling = 0;
    }
    else {
        if(total_perm_num <= MAX_CAPACITY) {
            is_sampling = 1;
        }
        else {
            is_sampling = 2;
        }
        mpf_class sampSizeMPF = EXP_COMB_SR * total_perm_num;
        if (sampSizeMPF < MIN_COMB_NUM)
            sampSizeMPF = MIN_COMB_NUM;
        if (sampSizeMPF > MAX_COMB_NUM)
            sampSizeMPF = MAX_COMB_NUM;
        sampSize = sampSizeMPF.get_ui();
        init_sampSize = sampSize;
    }

    if(is_sampling !=2 ) {
        L4 l4perms;
        list<list<list<list<int>>>> comboCycleList =  getComboCycleList(numList, l4perms);
        numList.clear();

        L2P transposList = getTransPosList(comboCycleList);
        comboCycleList.clear();
        l4perms.clear();
        
        genPermList(transposList, basicPPList);    
    }
    
    return NULL;
}

chrono::high_resolution_clock::time_point startTime;

void symMC_CF(void* input) {
    //cout << "gensolssignle" << endl;
    char** argv = (char**) input;
    generatePerms(argv[2]);

    // generate the models under partial symmetry breaking with blocking method
    vec<Lit> blocking_clause;
    while(solver->solve()) {
        blocking_clause.clear();
        std::string line = "";
        for (int i = 0; i < primary_var; i++) {
            blocking_clause.push(mkLit(i, solver->modelValue(i) == l_True));
            if(solver->modelValue(i) == l_True) {
                line += "1";
            }
            else {
                line += "0";
            }
        }
        AlloySolNum++;
        batchvec.emplace_back(line);
        solver->addClause_(blocking_clause);
        //cout << " AlloySolNum: " << AlloySolNum << endl;
    }
    
    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> time_span = endTime - startTime;
    cout << "AlloySolNum: " << AlloySolNum << ",time:" << time_span.count()/1000.0 << endl;
    
    parent.resize(AlloySolNum);
    rank_.resize(AlloySolNum);
    
    int model_id = 0;
    for(string model : batchvec) {
        // get the model under partial symmetry breaking from the pool
        std::hash<string> str_hash;
        size_t hash_value = str_hash(model);
	model_indx_map[hash_value] = model_id;	
	parent[model_id] = model_id;
	rank_[model_id] = 0;
	model_id++;	    
    }
    
    mpf_class diff_cntFSB = 1;
    mpf_class prev_cntFSB = AlloySolNum;
    //cout << "cntFSB:" << cntFSB << ",is_sampling:" << is_sampling << ",sampSize:" << sampSize << endl;
    
    if(is_sampling == 0) {
    	for(string model: batchvec) {
            std::hash<string> str_hash;
    	    GenPermSols_CF(model, str_hash(model));
    	}
    	cntFSB = 0;
    	for(int i = 0; i < AlloySolNum; i++) 
       	    if(parent[i] == i) 
                cntFSB += 1;             
    }
    else if(is_sampling == 1) {
    
    	for(int i= 0; i< total_perm_num; i++) {
            shuffle_indx_vector.push_back(i);
        }
        random_shuffle(shuffle_indx_vector.begin(), shuffle_indx_vector.end());
        
        auto endTime = chrono::high_resolution_clock::now();
    	    chrono::duration<double, std::milli> time_span = endTime - startTime; 
            cout << "cntFSB:" << cntFSB << ",diff_cntFSB:" << diff_cntFSB << ",is_sampling:" << is_sampling << ",sampSize:"             
            << sampSize << ",total_perm_num:" << total_perm_num <<  ",time:" << time_span.count()/1000.0 << endl;
            
        do {     
	    for(string model: batchvec) {
                std::hash<string> str_hash;
    	        GenPermSols_CF(model, str_hash(model));
    	    }	
    
            cntFSB = 0;
    	    for(int i = 0; i < AlloySolNum; i++)
       	        if(parent[i] == i) 
                     cntFSB += 1;
        
            diff_cntFSB = prev_cntFSB - cntFSB;
            prev_cntFSB = cntFSB; 
        
            sampSize_sofar += sampSize;    
            
            mpf_class sampSizeMPF = ADD_SR * total_perm_num;
            if (sampSizeMPF < MIN_COMB_NUM)
                sampSizeMPF = MIN_COMB_NUM;
            if (sampSizeMPF > MAX_COMB_NUM)
                sampSizeMPF = MAX_COMB_NUM;
            sampSize = sampSizeMPF.get_ui();
            
            if(sampSize_sofar + sampSize > total_perm_num)
 	    	sampSize = total_perm_num.get_ui() - sampSize_sofar;
            
            auto endTime = chrono::high_resolution_clock::now();
    	    chrono::duration<double, std::milli> time_span = endTime - startTime; 
            cout << "cntFSB:" << cntFSB << ",diff_cntFSB:" << diff_cntFSB << ",is_sampling:" << is_sampling << ",sampSize:"             
            << sampSize << ",total_perm_num:" << total_perm_num <<  ",time:" << time_span.count()/1000.0 << endl;
        
    	} while(diff_cntFSB != 0 && sampSize > 0) ;
    }
    else { // is_sampling ==2   
        auto endTime = chrono::high_resolution_clock::now();
    	    chrono::duration<double, std::milli> time_span = endTime - startTime; 
            cout << "cntFSB:" << cntFSB << ",diff_cntFSB:" << diff_cntFSB << ",is_sampling:" << is_sampling << ",sampSize:"             
            << sampSize << ",total_perm_num:" << total_perm_num <<  ",time:" << time_span.count()/1000.0 << endl; 	
    	do {    	    
    	    samp_ppList.clear();
            comb_hash_set.clear();   	    
    	    list<list<list<list<int>>>> ccl = SampPerms();
    	    comb_hash_set_sofar.insert(comb_hash_set.begin(), comb_hash_set.end());
            L2P transposList = getTransPosList(ccl);           
            genSampPermList(transposList, basicPPList, samp_ppList);
            
    	    for(string model: batchvec) {
                std::hash<string> str_hash;
    	        GenPermSols_CF(model, str_hash(model));
    	    }	   
    	    cntFSB = 0;
    	    for(int i = 0; i < AlloySolNum; i++) 
       	        if(parent[i] == i) 
                     cntFSB += 1;

            diff_cntFSB = prev_cntFSB - cntFSB;
            prev_cntFSB = cntFSB; 
        
            sampSize_sofar += sampSize;    
            
            mpf_class sampSizeMPF = ADD_SR * total_perm_num;
            if (sampSizeMPF < MIN_COMB_NUM)
                sampSizeMPF = MIN_COMB_NUM;
            if (sampSizeMPF > MAX_COMB_NUM)
                sampSizeMPF = MAX_COMB_NUM;
            sampSize = sampSizeMPF.get_ui();
            
            if(sampSize_sofar + sampSize > total_perm_num)
 	    	sampSize = total_perm_num.get_ui() - sampSize_sofar;
            
            auto endTime = chrono::high_resolution_clock::now();
    	    chrono::duration<double, std::milli> time_span = endTime - startTime; 
            cout << "cntFSB:" << cntFSB << ",diff_cntFSB:" << diff_cntFSB << ",is_sampling:" << is_sampling << ",sampSize:"             
            << sampSize << ",total_perm_num:" << total_perm_num <<  ",time:" << time_span.count()/1000.0 << endl;
        
    	} while(diff_cntFSB != 0 && sampSize > 0) ;
    	
    	cout << "end sampling" << endl;
    	
    }    
}

void symMC_CN(void* input) {
     symMC_CF(input);  
     
     std::hash<string> str_hash;
     double sum_mu = 0.0;
     sampSize = sampSize_sofar;
     sampSize_sofar = 0;
     for(int i = 0; i < AlloySolNum; i++) 
       	    if(parent[i] == i) 
                 sum_mu += GenPermSols_CN(batchvec[i], str_hash(batchvec[i])); 
     
     cntNSB = total_perm_num * sum_mu;  
     sampSize_sofar = sampSize;  
     cout << "sum_mu:" << sum_mu << ",cntNSB: " << cntNSB << endl;
}


void symMC_CN_prev(void* input) {
    //cout << "gensolssignle" << endl;
    char** argv = (char**) input;
    generatePerms(argv[2]);

    // generate the models under partial symmetry breaking with blocking method
    vec<Lit> blocking_clause;
    while(solver->solve()) {
        blocking_clause.clear();
        std::string line = "";
        for (int i = 0; i < primary_var; i++) {
            blocking_clause.push(mkLit(i, solver->modelValue(i) == l_True));
            if(solver->modelValue(i) == l_True) {
                line += "1";
            }
            else {
                line += "0";
            }
        }
        AlloySolNum++;
        batchvec.emplace_back(line);
        solver->addClause_(blocking_clause);
        //cout << " AlloySolNum: " << AlloySolNum << endl;
    }
    
    auto endTime = chrono::high_resolution_clock::now();
    chrono::duration<double, std::milli> time_span = endTime - startTime;
    cout << "AlloySolNum: " << AlloySolNum << ",time:" << time_span.count()/1000.0 << endl;
    
    parent.resize(AlloySolNum);
    rank_.resize(AlloySolNum);
    mu.resize(AlloySolNum);
    
    int model_id = 0;
    std::hash<string> str_hash;
    for(string model : batchvec) {
        // get the model under partial symmetry breaking from the pool       
        size_t hash_value = str_hash(model);
	model_indx_map[hash_value] = model_id;	
	parent[model_id] = model_id;
	rank_[model_id] = 0;
	model_id++;	    
    }
    
    mpf_class diff_cntFSB = 1;
    mpf_class prev_cntFSB = AlloySolNum;
    //cout << "cntFSB:" << cntFSB << ",is_sampling:" << is_sampling << ",sampSize:" << sampSize << endl;
    
    int round = 0;
    if(is_sampling == 0) {
        int model_id = 0;
    	for(string model: batchvec) {          
    	    mu[model_id] = GenPermSols_CN(model, str_hash(model));
    	    model_id++;
    	}
    	cntFSB = 0;
    	for(int i = 0; i < AlloySolNum; i++) 
       	    if(parent[i] == i) 
                cntFSB += 1;             
    }
    else if(is_sampling == 1) {
    
    	for(int i= 0; i< total_perm_num; i++) {
            shuffle_indx_vector.push_back(i);
        }
        random_shuffle(shuffle_indx_vector.begin(), shuffle_indx_vector.end());
        
        auto endTime = chrono::high_resolution_clock::now();
    	    chrono::duration<double, std::milli> time_span = endTime - startTime; 
            cout << "cntFSB:" << cntFSB << ",diff_cntFSB:" << diff_cntFSB << ",is_sampling:" << is_sampling << ",sampSize:"             
            << sampSize << ",total_perm_num:" << total_perm_num <<  ",time:" << time_span.count()/1000.0 << endl;
            
        do {
            if(round == 0) {
            	int model_id = 0;
	    	for(string model: batchvec) {                	
    	        	mu[model_id] = GenPermSols_CN(model, str_hash(model));
    	        	model_id++;
    	    	}
    	    }
    	    else {
		if(sampSize >= MAX_COMB_NUM) {
		    cout << "sampling 1 += mu" << endl;
		    int model_id = 0;
    	    	    for(string model: batchvec) {                	
    	        	mu[model_id] += GenPermSols_CN(model, str_hash(model));
    	        	model_id++;
    	    	    }
    	    	}
    	    	else
    	            for(string model: batchvec) {                	
    	        	GenPermSols_CF(model, str_hash(model));
    	    	    }
    	    }	
    
            cntFSB = 0;
    	    for(int i = 0; i < AlloySolNum; i++)
       	        if(parent[i] == i) 
                     cntFSB += 1;
            
        
            diff_cntFSB = prev_cntFSB - cntFSB;
            prev_cntFSB = cntFSB; 
        
            sampSize_sofar += sampSize;   
            round++; 
            
            mpf_class sampSizeMPF = ADD_SR * total_perm_num;
            if (sampSizeMPF < MIN_COMB_NUM)
                sampSizeMPF = MIN_COMB_NUM;
            if (sampSizeMPF > MAX_COMB_NUM) 
                sampSizeMPF = MAX_COMB_NUM * pow(2,round);
            sampSize = sampSizeMPF.get_ui();
            
 	    if(sampSize_sofar + sampSize > total_perm_num)
 	    	sampSize = total_perm_num.get_ui() - sampSize_sofar;
            
            auto endTime = chrono::high_resolution_clock::now();
    	    chrono::duration<double, std::milli> time_span = endTime - startTime; 
            cout << "cntFSB:" << cntFSB << ",diff_cntFSB:" << diff_cntFSB << ",is_sampling:" << is_sampling << ",sampSize:"             
            << sampSize << ",total_perm_num:" << total_perm_num <<  ",time:" << time_span.count()/1000.0 << endl;

    	} while(diff_cntFSB != 0 && sampSize > 0) ;
    }
    else { // is_sampling ==2   
        auto endTime = chrono::high_resolution_clock::now();
    	    chrono::duration<double, std::milli> time_span = endTime - startTime; 
            cout << "cntFSB:" << cntFSB << ",diff_cntFSB:" << diff_cntFSB << ",is_sampling:" << is_sampling << ",sampSize:"             
            << sampSize << ",total_perm_num:" << total_perm_num <<  ",time:" << time_span.count()/1000.0 << endl; 

    	do {    	    
    	    samp_ppList.clear();
            comb_hash_set.clear();   	    
    	    list<list<list<list<int>>>> ccl = SampPerms();
    	    comb_hash_set_sofar.insert(comb_hash_set.begin(), comb_hash_set.end());
            L2P transposList = getTransPosList(ccl);           
            genSampPermList(transposList, basicPPList, samp_ppList);
            if(round == 0) {
            	int model_id = 0;
	    	for(string model: batchvec) {               	
    	        	mu[model_id] = GenPermSols_CN(model, str_hash(model));
    	        	model_id++;
    	    	}
    	    }
    	    else {
    	        if(sampSize >= MAX_COMB_NUM) {
    	            cout << "sampling 2 += mu" << endl;
    	            int model_id = 0;
    	    	    for(string model: batchvec) {                	
    	        	mu[model_id] += GenPermSols_CN(model, str_hash(model));
    	        	model_id++;
    	    	    }
    	    	}
    	    	else
    	            for(string model: batchvec) {                	
    	        	GenPermSols_CF(model, str_hash(model));
    	    	    }
    	    }   	   
    	    cntFSB = 0;
    	    for(int i = 0; i < AlloySolNum; i++) 
       	        if(parent[i] == i) 
                     cntFSB += 1;
            
            

            diff_cntFSB = prev_cntFSB - cntFSB;
            prev_cntFSB = cntFSB; 
        
            sampSize_sofar += sampSize;    
            
            round++;
            
            mpf_class sampSizeMPF = ADD_SR * total_perm_num;
            if (sampSizeMPF < MIN_COMB_NUM)
                sampSizeMPF = MIN_COMB_NUM;
            if (sampSizeMPF > MAX_COMB_NUM) {
                sampSizeMPF = MAX_COMB_NUM * pow(2,round);
            }
            sampSize = sampSizeMPF.get_ui();
            
            if(sampSize_sofar + sampSize > total_perm_num)
 	    	sampSize = total_perm_num.get_ui() - sampSize_sofar;
            
            auto endTime = chrono::high_resolution_clock::now();
    	    chrono::duration<double, std::milli> time_span = endTime - startTime; 
            cout << "cntFSB:" << cntFSB << ",diff_cntFSB:" << diff_cntFSB << ",is_sampling:" << is_sampling << ",sampSize:"             
            << sampSize << ",total_perm_num:" << total_perm_num <<  ",time:" << time_span.count()/1000.0 << endl;
            
            
        
    	} while(diff_cntFSB != 0 && sampSize > 0) ;

    	cout << "end sampling2" << endl;
    	
    }

    double sum_mu = 0.0;
    if(is_sampling == 1 && round > 1 && cntFSB == 1) {
        sampSize = sampSize_sofar; 
        sampSize_sofar = 0;        
        sum_mu = GenPermSols_CN(batchvec[0], str_hash(batchvec[0]));    
        //cout << "sum_mu: " << sum_mu << " sampSize: " << sampSize << " sampSize_sofar: " << sampSize_sofar;         
    }
    else if(sampSize >= MAX_COMB_NUM) {
        for(int i = 0; i < AlloySolNum; i++) 
            if(parent[i] == i) 
                sum_mu += (mu[i]/round);
    }
    else {
        for(int i = 0; i < AlloySolNum; i++) 
            if(parent[i] == i) 
                sum_mu += mu[i]; 
            
    }  
    
    cntNSB = total_perm_num * sum_mu;
    
    cout << "sum_mu:" << sum_mu << ",cntNSB: " << cntNSB << endl;
      	    
}



int main(int argc, char** argv)
{
    try {
	
        setUsageHelp("USAGE: %s [options] <input-file> <result-output-file>\n\n  where input may be either in plain or gzipped DIMACS.\n");
        
#if defined(__linux__) && defined(_FPU_EXTENDED) && defined(_FPU_DOUBLE) && defined(_FPU_GETCW)
        fpu_control_t oldcw, newcw;
        _FPU_GETCW(oldcw); newcw = (oldcw & ~_FPU_EXTENDED) | _FPU_DOUBLE; _FPU_SETCW(newcw);
        //printf("WARNING: for repeatability, setting FPU to use double precision\n");
#endif
        // Extra options:
        //
        IntOption    verb   ("MAIN", "verb",   "Verbosity level (0=silent, 1=some, 2=more).", 0, IntRange(0, 2));
        IntOption    cpu_lim("MAIN", "cpu-lim","Limit on CPU time allowed in seconds.\n", INT32_MAX, IntRange(0, INT32_MAX));
        IntOption    mem_lim("MAIN", "mem-lim","Limit on memory usage in megabytes.\n", INT32_MAX, IntRange(0, INT32_MAX));
        BoolOption   cnt_all("MAIN", "all",    "Count all models.", false);
        BoolOption   use_pol("MAIN", "pol",    "Set preferred polarity for minimal model search.", true);
	
        parseOptions(argc, argv, true);

        Solver S;
        startTime = chrono::high_resolution_clock::now();
        //double initial_time = cpuTime();

        solver = &S;
        // Use signal handlers that forcibly quit:
        signal(SIGINT, SIGINT_exit);
        signal(SIGXCPU,SIGINT_exit);

        if (argc == 1)
            printf("Reading from standard input... Use '--help' for help.\n");

       // printf("argv[1]: %s\n", argv[1]);
        
        gzFile in = (argc == 1) ? gzdopen(0, "rb") : gzopen(argv[1], "rb");
        if (in == NULL)
            printf("ERROR! Could not open file: %s\n", argc == 1 ? "<stdin>" : argv[1]), exit(1);
        
        if (verb > 0){
            printf("============================[ Problem Statistics ]=============================\n");
            printf("|                                                                             |\n"); }
        
        primary_var = parse_DIMACS(in, S);

        //cout << "primary_var: " << primary_var << endl;
        gzclose(in);
        
        if (verb > 0){
            printf("|  Number of variables:  %12d                                         |\n", S.nVars());
            printf("|  Number of clauses:    %12d                                         |\n", S.nClauses()); }

    	struct timespec stoptime;
    	if (clock_gettime(CLOCK_REALTIME, &stoptime) == -1) {
        	cerr << "Failed to get curr time" << endl;
        	return 0;
    	}
   	    stoptime.tv_sec += timeout;

    	//symMC_CF(argv);
    	symMC_CN(argv);


        auto endTime = chrono::high_resolution_clock::now();
    	chrono::duration<double, std::milli> time_span = endTime - startTime;
        
        cout << "cntFSB:" << cntFSB << ",cntNSB:" << cntNSB << ",is_sampling:" << is_sampling << ",sampSizesofar:" << sampSize_sofar << ",total_perm_num:" << total_perm_num << ",time:" << time_span.count()/1000.0 << endl;
        
    } catch (OutOfMemoryException&){
        printf("===============================================================================\n");
        printf("INDETERMINATE\n");
        exit(0);
    }
}
