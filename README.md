# SymMC-Tool

### 1. About SymMC

SymMC is an automated tool implemented to solve three Alloy related problems:  

1) SymMC is the first approach that automatically approximates non-isomorphic model enumeration/counting for Alloy specifications. 

2) SymMC provides a competitive isomorphic counting approach for Alloy specifications compared to the state-of-the-art model counters.

3) SymMC also provides the first automatic quantification measurement on the solution space pruning ability of Kodkod PaSB. 

The input of SymMC is an arbitrary Alloy specification. SymMC has three key functionalities which correspond to three outputs: 

1) the non-isomorphic models/count of the specification;
2) the isomorphic model count of the specification;
3) the quantification metric in evaluating the pruning ability of the applied Kodkod partial SBP.

### 2. SymMC architecture overview:

![SymMC overview](./images/overview.jpg)

SymMC consists of three modules: 

1) **enhanced Kodkod**: encodes the Alloy specification under partial symmetry breaking into a SAT formula, and extracts the symmetry info of the specification; 
2) **all-satisfiable model enumerator** : generates all the satisfying models projected over primary variables under PaSB; 
3) **estimator module**: including the two estimators for approximating the non-isomorphic model set/count and the isomorphic model count, respectively; the quantification metric is the by-product of the non-isomorphic estimator;

### 3. Repo structures:

The enhanced Kodkod part corresponds to the folder named `Enhanced_Kodkod`. 

The input Alloy specification is stored in the `Files/specs` folder; the output sat formula by enhanced Kodkod module is stored in the `File/cnfs` folder; and the output symmetry information by enhanced Kodkod module is stored in the `Files/syms` folder.

The folder named `Enumerator_Estimator` is for the All-SAT model enumerator and the estimator parts.

### 

