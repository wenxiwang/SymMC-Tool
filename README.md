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

```
|-Enhanced_Kodkod
|-Enumerator_Estimator
|-Files
	|-specs
	|-cnfs
	|-syms
```



1. The folder named `Enhanced_Kodkod`is for the enhanced Kodkod (part 1).
2. The folder named `Enumerator_Estimator` is for the All-SAT model enumerator (part 2) and the estimator (part 3).
3. The input Alloy specification is stored in the `Files/specs` folder; the sat formula output by `Enhanced_Kodkod` (which is also one input of the `Enumerator_Estimator` ) is stored in the `File/cnfs` folder; and the symmetry information output by `Enhanced_Kodkod` (which is also one input of the  `Enumerator_Estimator`) is stored in the `Files/syms` folder. 

   

### 4. Building SymMC

- 1. Requirements:

- 2. Instructions:

     Step 1: Build the `Enhanced_Kodkod` module:

     go to `Enhanced_Kodkod` folder and run the command:

     ``````
     tbd
     ``````
     
     Step 2: Build the `Enumerator_Estimator` module: 
     
     go to `Enumerator_Estimator` folder and run the following command. You will get the `cmake-build-release` folder, if the build succeeds.
     
     ``````
     ./build.sh
     ``````

​			   

### 5. Using SymMC

- Step 1: Use enhanced Kodkod module to translate the input Alloy specification into a SAT formula and also extract the symmetry information. We denote the path of the generated SAT formula as `path_to_sat_file`; and the path of the extracted symmetry information as `path_to_sym_file`.

  ```
  tbd

- Step 2: Take the translated SAT formula and the extracted symmetry information, use the enumerator and estimator modules to get three possible outputs. Go to the `Enumerator_Estimator` folder, and:

  1. to get the non-isomorphic models/count of the input Alloy specification, run the command:

     ``````
     ./cmake-build-release/minisat -getmcFSB $path_to_sat_file $path_to_sym_file
  
  2. to get the isomorphic count of the input Alloy specification, run the command:
  
     ``````
     ./cmake-build-release/minisat -getmcNSB $path_to_sat_file $path_to_sym_file

  3. to get the quantification metric in evaluating the pruning ability of the applied Kodkod partial SBP, run the command:
  
     ``````
     ./cmake-build-release/minisat -kodkodpruning $path_to_sat_file $path_to_sym_file
     ``````

### 6. Reference:

For detailed descriptions about SymMC, please refer to our ESEC/FSE 2022 paper:

```
@inproceedings{wang2022symmc,
  title={SymMC: Approximate Model Enumeration and Counting Using Symmetry Information for Alloy Specifications},
  author={Wang, Wenxi and Hu, Yang and Kenneth, McMillan and Khurshid, Sarfraz},
  booktitle={Proceedings of the 30th ACM Joint Meeting on European Software Engineering Conference and Symposium on the Foundations of Software Engineering},
  pages={},
  year={2022}
}
```

