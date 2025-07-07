## Source code related to the paper "Hyperparameter Optimization-Based Trust Quantification for Misbehavior Detection Systems"

### Overview

The directory provides the source code related to the paper **Hyperparameter Optimization-Based Trust Quantification for Misbehavior Detection Systems**. The artifacts can be categorised into three parts:

- The source code for the three misbehavior detection system variants (binary MBD, float MBD, SL MBD)
- The source code for the hyperparameter optimization approach that we used in the paper
- The source code of the pipeline, that coordinates which software component should be exectued in which order and with which papameters

---

### Summary of Source Code Artifacts

#### Source Code of the Misbehavior Detection Systems

The source code of the misbeahvior detection systems (MBDs) can be found in the directory **MBD_Code**. It includes the source code of the binary MBD, the float MDB and the subjective logic (SL) MBD. To start the MBD, several parameters have to be specified. Therefore, we recommend just to exectue the pipeline provided in the directory **pipeline**. The source code of all three MBD variants is based on the work from Kamel et al.\[1\]. The code of the MBDs is written‚ in C++. Before the execution, the code needs to be complied. You can use **cmake** to compile the code with the following commands:

- *cmake .*
- *cmake --build . -- -j 30*

#### Source Code of the Hyperparameter-Optimization Approach

In this work we used hyperparameter optimization to optimize the parameters necessary for the MBDs. For this puropse, we used the Optuna framework. The corresponding source code is include in the **pipeline** folder in the **train.py** file of the single MBD variants. For exmaple, the source code for the optimization of the SL MBD can be found [here](https://cloudstore.uni-ulm.de/s/9JKabACB5932MTf?dir=/Source_Code_Evaluation/pipeline/SL_MBD_optimized&openfile=true).

### Pipelines

For each MBD variant we created a shell script called **evaluation.sh**. This shell script exectues the training and the test phase of the corresponding MBD variant, conducting 100,000 iterations. To reproduce our evalaution results, you just have to exectue the corresponding shell script. For example, for the SL MBD, the corresponding shell script can be found [here](https://cloudstore.uni-ulm.de/s/9JKabACB5932MTf?dir=/Source_Code_Evaluation/pipeline/SL_MBD_optimized&openfile=true).

---

### Sources

\[1\] J. Kamel, A. Kaiser, I. ben Jemaa, P. Cincilla and P. Urien, "CaTch: A Confidence Range Tolerant Misbehavior Detection Approach,