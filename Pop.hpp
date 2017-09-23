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
  OrgVec forest;
  OrgVec ScoreDexv; // for sorting
  typedef struct ScorePair { double Score[2]; };
  std::vector<ScorePair> ScoreBuf;// for recording scores even after some creatures are dead

  double avgnumwinners = 0.0;
  TesterPtr tester;// crucible
  uint32_t GenCnt;
  const double MutRate=0.3;//0.8
  /* ********************************************************************** */
  Pop() : Pop(popmax) {
  }
  /* ********************************************************************** */
  Pop(int popsize) {
    this->Init(popsize);
  }
  /* ********************************************************************** */
  ~Pop() {
    this->Clear();
  }
  /* ********************************************************************** */
  Init(int popsize) {// is it really necessary to be able to re-init without just deleting the population?
    Org *org;
    int pcnt;
    this->popsz = popsize;
    forest.resize(popsize);
    ScoreDexv.resize(popsize);
    ScoreBuf.resize(popsize);
    for (pcnt=0; pcnt<popsize; pcnt++) {
      org = Org::Abiogenate();
      ScoreDexv.at(pcnt) = org;
    }
    tester=new TesterMx(Org::DefaultWdt, Org::DefaultHgt);
    this->GenCnt=0;
  }
  /* ********************************************************************** */
  Clear() {// is it really necessary to be able to clear without just deleting the population?
    size_t siz, pcnt;
    siz = ScoreDexv.size();
    for (pcnt=0; pcnt<siz; pcnt++) {
      delete ScoreDexv.at(pcnt);
    }
    delete tester;
  }
  /* ********************************************************************** */
  void Gen() { // each generation
    this->Gen_No_Mutate();
    this->Mutate(MutRate, MutRate);
  }
  /* ********************************************************************** */
  void Gen_No_Mutate() { // call this by itself to 'coast', reproduce and winnow generations without mutation.
    double SurvivalRate=0.5;
    uint32_t popsize = this->ScoreDexv.size();
    OrgPtr candidate;
    for (uint32_t pcnt=0; pcnt<popsize; pcnt++) {
      candidate = ScoreDexv[pcnt];
      tester->Test(candidate);
    }
    Sort();
    OrgPtr TopOrg = ScoreDexv[0];
    double TopScore = TopOrg->Score[0];
    Birth_And_Death(SurvivalRate);
    printf("GenCnt:%i, TopScore:%f\n", this->GenCnt, TopScore);
    this->GenCnt++;
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
      doomed = ScoreDexv[cnt]; doomed->Doomed = true;
      delete doomed;
      child = ScoreDexv[topcnt]->Spawn();// Whenever one dies, replace it with the child of another.
      ScoreDexv[cnt] = child;
      if (++topcnt>=NumSurvivors) {topcnt=0;}
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

