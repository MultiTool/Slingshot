#ifndef ORG_HPP_INCLUDED
#define ORG_HPP_INCLUDED

#include "Matrix.hpp"

/* ********************************************************************** */
class Org;// forward
typedef Org *OrgPtr;
typedef std::vector<OrgPtr> OrgVec;
class Org: public Matrix {
public:
  static const int NumScores=2;
  double Score[NumScores];
  static OrgPtr Abiogenate() {return nullptr;}
  OrgPtr Spawn() {return nullptr;}
  void Rand_Init(){}
  /* ********************************************************************** */
  int Compare_Score(OrgPtr other) {
    int cnt = 0;
    double *ScoreMe, *ScoreYou;
    ScoreMe=this->Score; ScoreYou=other->Score;
    while (cnt<NumScores) {
      if (ScoreMe[cnt]<ScoreYou[cnt]) {return 1;}
      if (ScoreMe[cnt]>ScoreYou[cnt]) {return -1;}
      cnt++;
    }
    return 0;
  }
};

#endif // ORG_HPP_INCLUDED
