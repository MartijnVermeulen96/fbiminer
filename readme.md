# Cleaning Data with Forbidden Itemsets

> Based on the conference paper "Cleaning Data with Forbidden Itemsets", by J. Rammelaere, F. Geerts, and B. Goethals, published in the International Conference on Data Engineering (ICDE 2017). 

Since the ICDE publication, a journal extension of this work has been accepted by IEEE Transactions on Knowledge and Data Engineering (TKDE). The code associated with this extended version, which has enhanced repairing functionality, is available on the CodeOcean platform: https://codeocean.com/capsule/9082604/

## Quick Start Guide
The algorithm takes 2 mandatory arguments as input:
1. A dataset in csv format
2. The maximum lift threshold referred to as tau in the paper. Must be between 0 and 0.75 not included.

## Available Datasets
The Zoo dataset is used for example purposes in the paper. Beware that repairing this dataset might take a long time. 
The other 6 datasets in the datasets/ folder are the ones used in the experimental section, taken from the UCI Machine Learning Repository (http://archive.ics.uci.edu/ml/). 

Statistics of the datasets:

Dataset | Nr. Tuples | Nr. Items | Nr. Attributes
:--- | :--- | :--- | :---
Adult | 48842 | 202 | 11
CensusIncome | 199524 | 235 | 12
CreditCard | 30000 | 216 | 12
Ipums | 70187 | 364 | 32
LetterRecognition | 20000 | 282 | 17
Mushroom | 8124 | 119 | 15

## Problem Statement
Given a dataset D and a maximum lift threshold tau:
* The FBI Discovery Problem is to discover all itemsets I with lift(I) <= tau
* The FBI Repairing Problem is to modify D such that no itemsets I occur in D with lift(I) <= tau

## Paper Abstract
Methods  for  cleaning  dirty  data  typically  rely  on additional information about the data, such as user-specified constraints  that  specify  when  a  database  is  dirty.  These  constraints often involve domain restrictions and illegal value combinations. Traditionally, a database is considered clean if all constraints are satisfied. However, many real-world scenarios only have a dirty database available. In such a context, we adopt a dynamic notion of  data  quality,  in  which  the  data  is  clean  if  an  error  discovery algorithm   does   not   find   any   errors.   We   introduce forbidden itemsets which capture unlikely value co-occurrences in dirty data, and we derive properties of the lift measure to provide an efficient algorithm  for  mining  low  lift  forbidden  itemsets.  We  further introduce  a  repair  method  which  guarantees  that  the  repaired database  does  not  contain  any  low  lift  forbidden  itemsets.  The algorithm  uses  nearest  neighbor  imputation  to  suggest  possible repairs. Optional user interaction can easily be integrated into the proposed cleaning method. Evaluation on real-world data shows that errors are typically discovered with high precision, while the suggested  repairs  are  of  good  quality  and  do  not  introduce  new forbidden  itemsets,  as  desired.
