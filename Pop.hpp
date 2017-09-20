#ifndef POP_HPP_INCLUDED
#define POP_HPP_INCLUDED

#include "Org.hpp"
#include "Tester.hpp"

//#define popmax 1000
#define popmax 100
//#define popmax 10

/* ********************************************************************** */
class Pop;
typedef Pop *PopPtr;
class Pop {
public:
  uint32_t popsz;
  OrgVec ScoreDexv; // for sorting
  typedef struct ScorePair { double Score[2]; };
  std::vector<ScorePair> ScoreBuf;// for recording scores even after some creatures are dead

  StackPtr BPNet;// crucible
  uint32_t MaxNeuroGens = 2000;
  uint32_t DoneThresh = 32;//64; //32; //64;// 128;//16;
  double avgnumwinners = 0.0;
  Tester *tester;
  /* ********************************************************************** */
  Pop() : Pop(popmax) {
  }
  /* ********************************************************************** */
  Pop(int popsize) {
    BPNet = new Stack();
    Org *org;
    int pcnt;
    BPNet->Create_Any_Depth();
    this->popsz = popsize;
    ScoreDexv.resize(popsize);
    ScoreBuf.resize(popsize);
    for (pcnt=0; pcnt<popsize; pcnt++) {
      org = Org::Abiogenate();
      ScoreDexv.at(pcnt) = org;
    }
  }
  /* ********************************************************************** */
  ~Pop() {
    size_t siz, pcnt;
    siz = ScoreDexv.size();
    for (pcnt=0; pcnt<siz; pcnt++) {
      delete ScoreDexv.at(pcnt);
    }
    delete BPNet;
  }
  /* ********************************************************************** */
  void Gen(uint32_t evogens, uint32_t gencnt) { // each generation
  }
  /* ********************************************************************** */
  double AvgBeast() {
    size_t siz = ScoreDexv.size();
    double sum = 0.0;
    for (int cnt=0; cnt<siz; cnt++) {
      sum += ScoreDexv[cnt]->Score[0];
    }
    sum /= (double)siz;
    return sum;
  }
  /* ********************************************************************** */
  static bool AscendingScore(OrgPtr b0, OrgPtr b1) {
    return b0->Compare_Score(b1) > 0;
  }
  static bool DescendingScore(OrgPtr b0, OrgPtr b1) {
    return b1->Compare_Score(b0) > 0;
  }
  void Sort() {
    std::random_shuffle(ScoreDexv.begin(), ScoreDexv.end());
    std::sort (ScoreDexv.begin(), ScoreDexv.end(), DescendingScore);
  }
  /* ********************************************************************** */
  void Birth_And_Death(double SurvivalRate) {
    size_t siz = ScoreDexv.size();
    size_t NumSurvivors = siz * SurvivalRate;
    size_t topcnt, cnt;
    OrgPtr doomed, child;
    topcnt = 0;
    for (cnt=NumSurvivors; cnt<siz; cnt++) {
      doomed = ScoreDexv[cnt];
      delete doomed;
      child = ScoreDexv[topcnt]->Spawn();
      ScoreDexv[cnt] = child;
      topcnt++;
      if (topcnt>=NumSurvivors) {topcnt=0;}
    }
  }
  /* ********************************************************************** */
  void Mutate_Sorted(double Pop_MRate, double Org_MRate) {
    size_t siz = this->ScoreDexv.size();
    for (int cnt=16; cnt<siz; cnt++) {
      if (frand()<Pop_MRate) {
        OrgPtr org = this->ScoreDexv[cnt];
        org->Mutate_Me(Org_MRate);
      }
    }
  }
  /* ********************************************************************** */
  void Mutate(double Pop_MRate, double Org_MRate) {
    OrgPtr org;
    size_t LastOrg;
    size_t siz = this->ScoreDexv.size(); LastOrg = siz-1;
    for (int cnt=0; cnt<LastOrg; cnt++) {
      if (frand()<Pop_MRate) {
        org = this->ScoreDexv[cnt];
        org->Mutate_Me(Org_MRate);
      }
    }
    org = this->ScoreDexv[LastOrg];// very last mutant is 100% randomized, to introduce 'new blood'
    org->Rand_Init();
  }
};

#endif // POP_HPP_INCLUDED

