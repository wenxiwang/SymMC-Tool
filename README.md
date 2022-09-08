# SymMC-Tool

### 

### 1. SymMC is an automated tool implemented to solve three Alloy related problems:  

- SymMC is the first approach that automatically approximates non-isomorphic model enumeration/counting for Alloy specifications. 
- SymMC also provides the first automatic quantification measurement on the solution space pruning ability of Kodkod PaSB. 
- SymMC provides a competitive isomorphic counting approach for Alloy specifications compared to the state-of-the-art model counters.

### 2. Here is the overview of SymMC:

![SymMC overview](./images/overview.jpg)

- The enhanced Kodkod (1) part of SymMC adapts Alloy analyzer which takes in Alloy specification as input and translate the specification into a SAT formula and the extracted Symmetry information. This part corresponds to the folder named `Enhanced_Kodkod`. The input Alloy specification is stored in the `Files/specs` folder; the output cnf file is stored in the `File/cnfs` folder; and the symmetry information is stored in the `Files/syms` folder.
- The All-SAT model enumerator (2) and the estimator (3) part of SymMC is built upon MiniSat, which is a classic CDCL SAT solver. This part is in the folder named`Enumerator_Estimator`.

