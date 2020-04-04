TOBM: Two-sided online bipartite matching in spatial data: experiments and analysis
========================================================================

This repository stores the source code of the solutions to the problem called TOBM in the following paper.
If you use our source code or dataset, please consider citing our paper.

[1] **Two-sided online bipartite matching in spatial data: experiments and analysis.**
*Yiming Li, Jingzhi Fang, Yuxiang Zeng, Balz Maag, Yongxin Tong, Lingyu Zhang.* GeoInformatica 24(1): 175-198 (2020). [link](https://doi.org/10.1007/s10707-019-00359-w)


Usage of the algorithms
---------------

### Environment

gcc/g++ version: 7.4.0 

OS: Ubuntu

### Compile the algorithms

cd algorithm && make all


### Run the algorithms

./Greedy ../dataset/synthetic/data.txt

./Random ../dataset/synthetic/data.txt

./ext-Ranking ../dataset/synthetic/data.txt

./Batch-GR ../dataset/synthetic/data.txt

./Batch-LLEP ../dataset/synthetic/data.txt ../dataset/synthetic/entropy.txt

./Batch-NNP ../dataset/synthetic/data.txt

./Guide ../dataset/synthetic/data.txt ../dataset/synthetic/guide.txt

./POLAR-OP ../dataset/synthetic/data.txt ../dataset/synthetic/guide.txt

Description of the datasets
---------------

### Synthetic dataset

dataset/synthetic:

data.txt: the information of tasks and workers

entropy.txt: the information of entropy for Batch-LLEP

predict.txt: the prediction for POLAR-OP

guide.txt: the guide for POLAR-OP

### Real dataset

data.txt: the information of tasks and workers

entropy.txt: the information of entropy for Batch-LLEP

predict.txt: the prediction for POLAR-OP

guide.txt: the guide for POLAR-OP


Contact
------------
- Yuxiang Zeng: yzengal@cse.ust.hk
- Yongxin Tong: yxtong@buaa.edu.cn
- Yiming Li: yliix@connect.ust.hk