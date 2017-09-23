#ifndef TESTER_HPP_INCLUDED
#define TESTER_HPP_INCLUDED

#include "Org.hpp"
#include "Cluster.hpp"
#include "Stack.hpp"

/* ********************************************************************** */
class Tester;// forward
typedef Tester *TesterPtr;
typedef std::vector<TesterPtr> TesterVec;
class Tester {// base class, does not do anything by itself
public:
  OrgPtr org;
  /* ********************************************************************** */
  Tester(){
  }
  /* ********************************************************************** */
  ~Tester(){
  }
  /* ********************************************************************** */
  virtual void Test() {
  }
  /* ********************************************************************** */
  virtual void Test(OrgPtr candidate) {
  }
  /* ********************************************************************** */
  virtual double Dry_Run_Test() {
    return 0;
  }
};

/* ********************************************************************** */
class TesterMx;// forward
typedef TesterMx *TesterMxPtr;
typedef std::vector<TesterMxPtr> TesterMxVec;
class TesterMx : public Tester {// evolve to match an existing matrix
public:
  MatrixPtr model;// alternate crucible
  VectPtr invec, outvec0, outvec1;
  int MxWdt, MxHgt;
  /* ********************************************************************** */
  TesterMx(int MxWdt0, int MxHgt0){
    this->MxWdt=MxHgt0; this->MxHgt=MxHgt0;
    this->model = new Matrix(MxWdt0, MxHgt0);
    this->model->Mutate_Me(1.0);// mutate 100%
    this->invec = new Vect(MxWdt0);
    this->invec->Mutate_Me(1.0);// mutate 100%
    this->outvec0 = new Vect(MxHgt0);
    this->outvec1 = new Vect(MxHgt0);
  }
  /* ********************************************************************** */
  ~TesterMx(){
    delete this->outvec1;
    delete this->outvec0;
    delete this->invec;
    delete this->model;
  }
  /* ********************************************************************** */
  void Test() override {
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    // Run the candidate and the model and compare their outputs.
    int Iterations=1;
    double val0, val1, diff;
    double range = 2.0;
    double score = 1.0;
    this->invec->Mutate_Me(1.0);// mutate 100%
    model->Iterate(invec, Iterations, outvec0);
    candidate->Iterate(invec, Iterations, outvec1);
    for (int cnt=0;cnt<this->MxHgt;cnt++){
      val0 = outvec0->ray[cnt];
      val1 = outvec1->ray[cnt];
      diff=std::fabs(val0-val1);
      score*=(range-diff)/range;
    }
    candidate->Score[0]=score;
  }
};

/* ********************************************************************** */
class TesterNet;// forward
typedef TesterNet *TesterNetPtr;
typedef std::vector<TesterNetPtr> TesterNetVec;
class TesterNet : public Tester {// evolve to create a backpropagation learning rule
public:
  StackPtr BPNet;// crucible
  uint32_t MaxNeuroGens = 2000;
  uint32_t DoneThresh = 32;//64; //32; //64;// 128;//16;
  /* ********************************************************************** */
  TesterNet(){
    BPNet = new Stack();
    BPNet->Create_Any_Depth();
  }
  /* ********************************************************************** */
  ~TesterNet(){
    delete BPNet;
  }
  /* ********************************************************************** */
  void Test() override {
  }
  /* ********************************************************************** */
  void Test(OrgPtr candidate) override {
    this->BPNet->Attach_Genome(candidate);
    // to do: run the BPNet, judge how well it has learned and assign the score to the candidate.
    // or alternatively, run the candidate and the model and compare their outputs.
    candidate->Score[0]=1;//dummy assignment
  }
  /* ********************************************************************** */
  double Dry_Run_Test() {
#if false
    uint32_t GenCnt;
    double goal;
    double WinCnt;
    IOPairPtr Pair;
    WinCnt=0.0;
    for (GenCnt=0; GenCnt<MaxNeuroGens; GenCnt++) {
      Pair = TSet->at(GenCnt%TSet->size());
      goal = Pair->goalvec.at(0);
      // BPNet->Load_Inputs(Pair->invec.at(0), Pair->invec.at(1), 1.0);
      BPNet->Load_Inputs(&(Pair->invec));
      BPNet->Fire_Gen();
      double fire = BPNet->OutLayer->NodeList.at(0)->FireVal;
      if (goal*fire>0) {
        WinCnt++;
      }
    }
    return WinCnt/((double)MaxNeuroGens);
#endif // false
    return 0;
  }
  /* ********************************************************************** */
  void Run_Test() {
#if false
    uint32_t FinalFail = 0;
    uint32_t GenCnt;
    double goal;
    double ScoreBefore;
    double WinCnt;
    IOPairPtr Pair;
    do {
      BPNet->Randomize_Weights();
      ScoreBefore = Dry_Run_Test(16, TSet);
    } while (ScoreBefore==1.0);
    BPNet->Attach_Genome(FSurf);
    WinCnt=0.0;
    //MaxNeuroGens/=TSet->size();
    for (GenCnt=0; GenCnt<MaxNeuroGens; GenCnt++) {
      Pair = TSet->at(GenCnt%TSet->size());

      goal = Pair->goalvec.at(0);
      BPNet->Load_Inputs(&(Pair->invec));
      BPNet->Fire_Gen();
      double fire = BPNet->OutLayer->NodeList.at(0)->FireVal;
      if (goal*fire>0) {
        WinCnt++;
      } else {
        FinalFail = GenCnt;
      }
      if ((GenCnt-FinalFail)>DoneThresh) {
        break;
      }
      // BPNet->Backprop(goal);
      BPNet->Backprop(&(Pair->goalvec));
      //}
    }
    double PrimaryScore = 0;
    if (FinalFail>=(MaxNeuroGens-DoneThresh)) {
      PrimaryScore = 0.0;
    } else {
      PrimaryScore = 1.0 - ( ((double)FinalFail)/(double)(MaxNeuroGens-DoneThresh) );// oneify
    }
    FSurf->FinalFail = FinalFail;
    FSurf->Score[0] *= PrimaryScore;//1.0 - ( ((double)FinalFail)/(double)MaxNeuroGens );// oneify
    double Remainder = MaxNeuroGens-GenCnt;// if nobody won *earlier*, then score by average goodness of output
    double temp = ( (WinCnt+Remainder)/((double)MaxNeuroGens) ) - ScoreBefore;
    temp = (temp+1.0)/2.0;
    if (temp<0.0) {
      temp=0.0;
    }
    FSurf->Score[1] *= temp;//oneify
#endif // false
  }
};

#endif // TESTER_HPP_INCLUDED
