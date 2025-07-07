## Evaluation results related to the paper "Hyperparameter Optimization-Based Trust Quantification for Misbehavior Detection Systems"


### Overview
This directory provides the evaluation results related to the paper **Hyperparameter Optimization-Based Trust Quantification for Misbehavior Detection Systems**. The artifacts can be categorised into two parts:
- The evaluation results of the comparision of the MBD variants
- The evaluation results of the comparision of different fusion operators of the SL MBD



***

### Summary of Articacts

#### Evaluation Results of the MBD Variants
The results of the evaluation of the binary MBD, the float MBD and the SL MBD are provided in the directory **MBD_Comparision**. The results include the MBDs with and without using the hyperparameter optimization approach.
For each MBD variant the results are provided in a separate directory. This directory includes several json-files.
For each attack, there exists a separate file that contains the performance for this attack. In addition, there exists a file that contains the overall performance for all three analyzed attacks. In addition, for each json-files, also a corresonding visualization is provided.
The json-files include the parameter set used for the MBD variant and the corresponding performance that results when these parameters are used. For each parameter set, the true positives (TP), true negatives (TN), false positives (FP) and false negatives (FN) are provided. In addition to that, also the accuracy, precision, recall and f1-score are provided.



#### Evalaution Results of the Comparision of different Fusion Operators
For the SL MBD, we analyzed several fusion operators. A fusion operator fuses several subjective logic trust opinions into a final trust opinion. Based on this trust opinion, it is decided whether a message is malicious. We have analyzed four different fusion operator provided by Josang et al.[1].
In the directroy **Fusion_Operator_Comparision**, the performance results for each fusion operator is provided. For each fusion operator the results are provided in a separate sub-directory.




---

### Sources
[1] A. Josang, "Subjective Logic: A formalism for reasoning under uncertainty." 