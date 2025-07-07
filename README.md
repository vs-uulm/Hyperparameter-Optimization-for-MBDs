## Research Data related to the paper "Hyperparameter Optimization-Based Trust Quantification for Misbehavior Detection Systems"

### Overview

The repository provides all artifacts related to the paper **Hyperparameter Optimization-Based Trust Quantification for Misbehavior Detection Systems**. The artifacts can be categorized into three parts:

- The VeReMi extension dataset, which was extended with a further attribute necessary for the *Position Plausibility Detector*
- The evaluation results of all evaluations conducted in the paper
- The source code to reproduce the evalaution results (including the code for the MBD variants and the optimization approach)

---

### Summary of Artifacts

#### VeReMi Extension Dataset

The VeReMi extension dataset is a benchmark dataset to evaluate misbehavior detection systems. The dataset was create by Kamel et al.[1] and was extended within our work by an additional attribute. For each position included in the dataset, the distance to the closes road was determined and added to the dataset. This enables the *position plausibility detector* to provide an output. The corresponding dataset is located [here](https://cloudstore.uni-ulm.de/s/9JKabACB5932MTf?dir=/VeReMi_Extension_Dataset).

#### Evaluation Results

Within our work two different kinds of evaluations were coducted. In the first step, the implemented SL MBD was evaluated with different subjective logic fusion operators. To determine the appropriate operator, the performance of four different fusion operators was tested. In the second step, the performance of three variants of an MBD was evaluated with and without integrating the optimization approach. The results of all evaluations are located [here](https://cloudstore.uni-ulm.de/s/9JKabACB5932MTf?dir=/Evaluation_Results).

### Source Code

In our work, we evaluated three different MBD variants: a binary MBD, a float MBD and a subjective logic MBD. The binary MBD and the float MBD were created my Kamel et al.[2]. The subjective logic MBD was implemented based on the detectors used in the float MBD and the proposed trust quantification methodology. We applied a hyperparameter optimization approach to all three MBD variants to optimize the corresponding parameters for the MBDs to achieve the best performance. The code of the three MDBs and the code of the hyperparameter optimization approach are located [here](https://cloudstore.uni-ulm.de/s/9JKabACB5932MTf?dir=/Source_Code_Evaluation).

---

### Sources

[1] J. Kamel, M. Wolf, R. W. van der Hei, A. Kaiser, P. Urien and F. Kargl, "VeReMi Extension: A Dataset for Comparable Evaluation of Misbehavior Detection in VANETs"

[2] J. Kamel, A. Kaiser, I. ben Jemaa, P. Cincilla and P. Urien, "CaTch: A Confidence Range Tolerant Misbehavior Detection Approach"