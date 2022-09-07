# SymMC-Tool

### 1. Overview

- SymMC is an automated tool implemented to solve three Alloy related problems:  
  - SymMC is the first approach that automatically approximates non-isomorphic model enumeration/counting for Alloy specifications. 
  - SymMC also provides the first automatic quantification measurement on the solution space pruning ability of Kodkod PaSB. 
  - SymMC provides a competitive isomorphic counting approach for Alloy specifications compared to the state-of-the-art model counters.
  
- ```
  ![SymMC overview](/images/overview.pdf)
  ```

  The enumerator and estimator part of SymMC is built upon MiniSat, which is a classic CDCL SAT solver. 

