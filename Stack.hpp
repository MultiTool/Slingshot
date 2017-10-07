#ifndef STACK_HPP_INCLUDED
#define STACK_HPP_INCLUDED

#include <functional>

#include "Cluster.hpp"

/* ********************************************************************** */
class Stack;// forward
typedef Stack *StackPtr;
class Stack {//for layered 1-way networks
public:
  ClusterVec Layers;
  ClusterPtr InLayer, OutLayer;
  /* ********************************************************************** */
  Stack() {
  }
  /* ********************************************************************** */
  ~Stack() {
    for (size_t lcnt=0; lcnt<Layers.size(); lcnt++) {
      delete Layers.at(lcnt);
    }
  }
  /* ********************************************************************** */
  void Attach_Genome(MatrixPtr org) {
    size_t cnt;
    ClusterPtr cluster;
    size_t siz = this->Layers.size();
    for (cnt=0; cnt<siz; cnt++) {
      cluster = this->Layers.at(cnt);
      cluster->Attach_Genome(org);
    }
  }
  /* ********************************************************************** */
  void Create_Any_Depth() {
    ClusterPtr clprev, clnow;
    InLayer = clnow = new Cluster(3); Layers.push_back(clnow);
    clprev = clnow;
    int largo =  10;
    for (int lcnt=0; lcnt<largo; lcnt++) {
      clnow = new Cluster(3); Layers.push_back(clnow);
      clnow->Connect_Other_Cluster(clprev);
      clprev = clnow;
    }
    OutLayer = clnow = new Cluster(1); Layers.push_back(clnow);
    clnow->Connect_Other_Cluster(clprev);
  }
  /* ********************************************************************** */
  void Load_Inputs(double in0, double in1, double in2) {
    InLayer->NodeList.at(0)->FireVal = in0;
    InLayer->NodeList.at(1)->FireVal = in1;
    InLayer->NodeList.at(2)->FireVal = in2;
  }
  /* ********************************************************************** */
  void Load_Inputs(std::vector<double> *invec) {
    InLayer->Load_Inputs(invec);
  }
  /* ********************************************************************** */
  void Fire_Gen() {
    size_t lcnt;

    // experiment with templates
    this->Aaagh<std::function<void(SynapsePtr)>>(DeStateFn);
    this->Aaagh<double>(0.1);

    ClusterPtr clnow;
    clnow = Layers.at(0);
    for (lcnt=1; lcnt<Layers.size(); lcnt++) {
      clnow->Push_Fire();// emit
      clnow = Layers.at(lcnt);
      clnow->Collect_And_Fire();// absorb
    }
  }
  /* ********************************************************************** */
  void Backprop(std::vector<double> *goalvec) {
    int lcnt;
    ClusterPtr clnow;

    OutLayer->Load_Correctors(goalvec);

    int LastLayer = Layers.size()-1;
    clnow = Layers.at(LastLayer);
    for (lcnt=LastLayer-1; lcnt>=0; lcnt--) {
      clnow->Push_Correctors_Backward();
      clnow = Layers.at(lcnt);
      clnow->Pull_Correctors();// absorb
    }

    for (lcnt=LastLayer; lcnt>=0; lcnt--) {
      clnow = Layers.at(lcnt);
      clnow->Apply_Correctors();
    }
  }
  /* ********************************************************************** */
  void Randomize_Weights() {
    ClusterPtr clnow;
    size_t siz = this->Layers.size();
    for (size_t lcnt=0; lcnt<siz; lcnt++) {
      clnow = Layers.at(lcnt);
      clnow->Randomize_Weights();
    }
  }
  /* ********************************************************************** */
  void Reset_Weights() {// reset network back to original random values
    ClusterPtr clnow;
    size_t siz = this->Layers.size();
    for (size_t lcnt=0; lcnt<siz; lcnt++) {
      clnow = Layers.at(lcnt);
      clnow->Reset_Weights();
    }
  }
  /* ********************************************************************** */
  template <class AaaghType> void Aaagh(AaaghType fred) {
    ClusterPtr clnow;
    size_t siz = this->Layers.size();
    for (size_t lcnt=0; lcnt<siz; lcnt++) {
      clnow = Layers.at(lcnt);
      clnow->Aaagh<AaaghType>(fred);
    }
  }
  /* ********************************************************************** */
  static void DeStateFn(SynapsePtr Syn) {
    Syn->DeState();
  }
  /* ********************************************************************** */
  void DeState() {
    this->Synapse_Apply(DeStateFn);
  }
  /* ********************************************************************** */
  void Synapse_Apply(std::function<void(SynapsePtr)> SynFunc) {// traverse the whole tree of synapses with whatever
    ClusterPtr clnow;
    size_t siz = this->Layers.size();
    for (size_t lcnt=0; lcnt<siz; lcnt++) {
      clnow = Layers.at(lcnt);
      clnow->Synapse_Apply(SynFunc);
    }
  }
  /* ********************************************************************** */
  void Print_Specs() {
    ClusterPtr cluster;
    size_t siz = this->Layers.size();
    printf(" Stack depth:%li, ", siz);
    cluster = this->Layers.at(1);
    size_t width = cluster->NodeList.size();
    printf(" Mid Width:%li ", width);
    printf(" \n");
  }
  /* ********************************************************************** */
  void Print_Me() {
    size_t cnt;
    ClusterPtr cluster;
    printf(" Stack this:%p, ", this);
    size_t siz = this->Layers.size();
    printf(" num Clusters:%li\n", siz);
    for (cnt=0; cnt<siz; cnt++) {
      cluster = this->Layers.at(cnt);
      cluster->Print_Me(cnt);
    }
  }
};

#endif // STACK_HPP_INCLUDED
