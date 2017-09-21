#ifndef TESTER_HPP_INCLUDED
#define TESTER_HPP_INCLUDED

#include "Org.hpp"
#include "Cluster.hpp"
#include "Stack.hpp"

/* ********************************************************************** */
class Tester;// forward
typedef Tester *TesterPtr;
typedef std::vector<TesterPtr> TesterVec;
class Tester : public Stack {// to do: make this a wrapper around the network instead of its subclass.
public:
  OrgPtr org;
  uint32_t MaxNeuroGens;
  void Test() {
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
      if (goal*fire>0) { WinCnt++; }
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
    if (temp<0.0) {temp=0.0;}
    FSurf->Score[1] *= temp;//oneify
#endif // false
  }
};

#endif // TESTER_HPP_INCLUDED