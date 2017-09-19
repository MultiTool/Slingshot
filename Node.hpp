#ifndef NODE_HPP_INCLUDED
#define NODE_HPP_INCLUDED

#include "Synapse.hpp"

/* ********************************************************************** */
class Node;// forward
typedef Node *NodePtr;
typedef std::vector<NodePtr> NodeVec;
class Node {
public:
  double FireVal, SumFireSq, Corrector, CorrSum;
  SynVec Working_Ins, Working_Outs;
  /* ********************************************************************** */
  Node() {
  }
  /* ********************************************************************** */
  ~Node() {
    for (int cnt=0; cnt<this->Working_Ins.size(); cnt++) {
      delete this->Working_Ins.at(cnt);
    }
    this->Working_Ins.clear();// clearing probably not necessary
    this->Working_Outs.clear();
  }
  /* ********************************************************************** */
  void AddCorr(double val) { this->CorrSum+=val; }
  /* ********************************************************************** */
  void Attach_Genome(MatrixPtr genome0) {
    SynapsePtr ups;
    size_t siz = this->Working_Ins.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ups = this->Working_Ins.at(cnt);
      ups->Attach_Genome(genome0);
    }
  }
  /* ********************************************************************** */
  void ConnectIn(NodePtr other) {// attach upstream node to me
    SynapsePtr ln = new Synapse();
    ConnectIn(other, ln);
  }
  /* ********************************************************************** */
  void ConnectIn(NodePtr other, SynapsePtr ln) {// attach upstream node to me
    this->Working_Ins.push_back(ln);// this approach uses less memory, fewer allocations/frees and is probably faster.
    other->Working_Outs.push_back(ln);
    ln->USNode = other; ln->DSNode = this;
  }
  /* ********************************************************************** */
  void Randomize_Weights() {
    SynapsePtr ups;
    size_t siz = this->Working_Ins.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ups = this->Working_Ins.at(cnt);
      ups->Randomize_Weight();
    }
  }
  /* ********************************************************************** */
  void Collect_And_Fire() {
    SynapsePtr ups;
    double Fire, Sum=0, SumSq=0.0;
    size_t siz = this->Working_Ins.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ups = this->Working_Ins.at(cnt);
      Fire=ups->GetWeightedFire();
      Sum+=Fire;
      SumSq+=ups->FireVal*ups->FireVal;
    }
    this->SumFireSq = SumSq;
    this->FireVal = ActFun(Sum);
  }
  /* ********************************************************************** */
  void Push_Fire() {
    SynapsePtr downs;
    double MyFire=this->FireVal;
    size_t siz = this->Working_Outs.size();
    for (int cnt=0; cnt<siz; cnt++) {
      downs = this->Working_Outs.at(cnt);
      downs->Assign_Fire(MyFire);
    }
  }
  /* ********************************************************************** */
  void Push_Correctors_Backward() {
    SynapsePtr ups;
    size_t siz = this->Working_Ins.size();
    for (int cnt=0; cnt<siz; cnt++) {
      ups = this->Working_Ins.at(cnt);
      //ups->Corrector = this->Corrector;
      ups->DSCorrVal = this->Corrector;
    }
  }
  /* ********************************************************************** */
  void Pull_Correctors() {
    double ClipRad = 20.0;
    SynapsePtr downs;
    double CorrSum;
    double SurfParams[2];
    double MyFire=this->FireVal;
    double Fire_Deriv = 1.0;
    CorrSum = 0.0;
    size_t siz = this->Working_Outs.size();
    for (int cnt=0; cnt<siz; cnt++) {
      downs = this->Working_Outs.at(cnt);
      //CorrSum += downs->GetWeightedCorrector();
      CorrSum += downs->GetGenCorrector();
    }
    this->Corrector = CorrSum;
    //Fire_Deriv = sigmoid_deriv_postfire(MyFire);// wrong. instead just what?
    //this->Corrector = Fire_Deriv * CorrSum;
  }
  /* ********************************************************************** */
  void Apply_Corrector(double CorrDelta) {// wrong? this may be 100% handled by synapse matrix
    SynapsePtr ups;
    double num, fire;
    double ModCorrDelta = CorrDelta;// * this->LRate;
    size_t siz = this->Working_Ins.size();

    double SumSq=0.0;// wrong. want to replace this with matrix decision
    SumSq = this->SumFireSq;
    if (true) {
      for (int cnt=0; cnt<siz; cnt++) {// then divide all upstream fires by their sum-of-squares.  then mult that vector by corrector value and add to weight.
        ups = this->Working_Ins.at(cnt);
        ups->Run_Process();// this applies weight changes, and creates the passback value.
      }
      return;
    }
    /*
    first we get the whole input vector and unitize it (sum of squares?). that becomes the recognition vector.
    then mult the recog vector by the corrector (and by lrate) and add it to my input weights.
    */
    if (false) {
      for (int cnt=0; cnt<siz; cnt++) {// first get sum of squares of upstream fires
        fire = this->Working_Ins.at(cnt)->FireVal;
        SumSq += fire * fire;
      }
    }
    SumSq = this->SumFireSq;
    for (int cnt=0; cnt<siz; cnt++) {// then divide all upstream fires by their sum-of-squares.  then mult that vector by corrector value and add to weight.
      ups = this->Working_Ins.at(cnt);
      num = ups->FireVal / SumSq;
      ups->Weight += num * ModCorrDelta;
    }
  }
  /* ********************************************************************** */
  void ReadSynapseUS(Synapse& syn) {
    //this->Corrector=syn.DSCorrVal; this->FireVal=syn.DSFireVal;// wrong
  }
  /* ********************************************************************** */
  void ReadSynapseDS(Synapse& syn) {
    this->Corrector=syn.USCorrVal; //this->FireVal=syn.USFireVal;// wrong
  }
  /* ********************************************************************** */
  void WriteSynapseUS(Synapse& syn) {
    syn.DSCorrVal=this->Corrector; syn.DSFireVal=this->FireVal;
    syn.SumFireSq=this->SumFireSq;// only pass this upstream, unless synapse has a separate ds variable for it.
  }
  /* ********************************************************************** */
  void WriteSynapseDS(Synapse& syn) {
    syn.USCorrVal=this->Corrector; syn.USFireVal=this->FireVal;
  }
};

/* ********************************************************************** */
class Layer {
public:
  Matrix *weights;
};

#endif // NODE_HPP_INCLUDED
