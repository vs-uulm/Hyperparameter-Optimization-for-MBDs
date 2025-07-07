## VeReMi Extension dataset related to the paper "Hyperparameter Optimization-Based Trust Quantification for Misbehavior Detection Systems"

### Overview

This directory provides the VeReMi extension dataset related to the paper **Hyperparameter Optimization-Based Trust Quantification for Misbehavior Detection Systems**. The artifacts can be categorised in four parts:

- The used training dataset
- The used test dataset
- The source code used to split the VeReMi extension dataset into a training and a test dataset
- The source code used to serialize the training and test dataset

---

### Summary of Artifacts

#### Training and Test Dataset

The training dataset is located in the directory **train** and the test dataset is located in the directory **test**. These datasets were created based on the VeReMi Extension datsaset provided by Kamel et al.[1] and were split into two parts.
We have extended this dataset with an additional attribute called **dist**, which specifies the distance of the provided position to the closest road-segment. This attribute is necessary for the *position plausibililty detector*.

#### Source Code for Splitting the VeReMi Extension Dataset

The source code for splitting the VeReMi extension dataset into a training and a test dataset is located in the directory **Source_Code_Splitting**. In the source code, the path of of the VeReMi extension dataset, the path of the correspoding ground truth files of the VeReMi extension dataset and the ratio of the percentage of the dataset to be assigned to the trainding dataset and the percentage to be assigned to the test dataset must be specified. The source code checks 30,000 different splits of the dataset, condcuts a Kolmogorow-Smirnow-Test and selects the split where the distributsions auf the metrics avg. reiving rate, number of malicious message and number of total messages are as similar as possible.

### Source Code of the Generation of the Bin-Files

We have created the source code for the serialization of the training and test dataset into bin files. This allows the training and test dataset to be dererialized much faster in the training and test phase of the evaluation compared to reading all json files of the VeReMi extension dataset.
The source code for this serialization of the training and test dataset can be found in the directory **Source_Code_Bin_Generation**.

---

### Sources

[1] J. Kamel, M. Wolf, R. W. van der Heijden, A. Kaiser, P. Urien and F. Kargl, "VeReMi Extension: A Dataset for Comparable Evaluation of Misbehavior Detection in VANETs