TOBM: Two-sided online bipartite matching in spatial data: experiments and analysis
========================================================================

This repository stores the source code of the solutions to the problem called TOBM in the following papers.
If you use our source code or dataset, please consider citing our papers.

[1] **Two-sided online bipartite matching in spatial data: experiments and analysis.**
*Yiming Li, Jingzhi Fang, Yuxiang Zeng, Balz Maag, Yongxin Tong, Lingyu Zhang.* GeoInformatica 24(1): 175-198 (2020). [link](https://doi.org/10.1007/s10707-019-00359-w)

[2] **Flexible Online Task Assignment in Real-Time Spatial Data.**
*Yongxin Tong, Libin Wang, Zimu Zhou, Bolin Ding, Lei Chen, Jieping Ye, Ke Xu.* PVLDB 10(11): 1334-1345 (2017). [link](http://www.vldb.org/pvldb/vol10/p1334-tong.pdf)

If you find our work helpful in your research, please consider citing our papers and the bibtex are listed below:
```  
@article{DBLP:journals/geoinformatica/LiFZMTZ20,
  author    = {Yiming Li and
               Jingzhi Fang and
               Yuxiang Zeng and
               Balz Maag and
               Yongxin Tong and
               Lingyu Zhang},
  title     = {Two-sided online bipartite matching in spatial data: experiments and analysis},
  journal   = {GeoInformatica},
  volume    = {24},
  number    = {1},
  pages     = {175--198},
  year      = {2020},
} 
@article{DBLP:journals/pvldb/TongWZDCYX17,
  author    = {Yongxin Tong and
               Libin Wang and
               Zimu Zhou and
               Bolin Ding and
               Lei Chen and
               Jieping Ye and
               Ke Xu},
  title     = {Flexible Online Task Assignment in Real-Time Spatial Data},
  journal   = {{PVLDB}},
  volume    = {10},
  number    = {11},
  pages     = {1334--1345},
  year      = {2017},
}
``` 

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

The directory **dataset/synthetic** includes

data.txt: the information of tasks and workers

entropy.txt: the information of entropy for Batch-LLEP

predict.txt: the prediction for POLAR-OP

guide.txt: the guide for POLAR-OP

### Real dataset

The directory **dataset/real** includes

data.txt: the information of tasks and workers

entropy.txt: the information of entropy for Batch-LLEP

predict.txt: the prediction for POLAR-OP

guide.txt: the guide for POLAR-OP


Related resources
------------------------
We have maintained a paper list of the studies on spatial crowdsourcing. [link](https://github.com/BUAA-BDA/SpatialCrowdsourcing-Survey)



Contact
------------
- Yuxiang Zeng: yzengal@cse.ust.hk
- Yongxin Tong: yxtong@buaa.edu.cn
- Yiming Li: yliix@connect.ust.hk
