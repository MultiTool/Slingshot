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
  bool Doomed = false;
  static const int DefaultWdt = 5, DefaultHgt = 5;
  /* ********************************************************************** */
  Org(int wdt0, int hgt0) : Matrix(wdt0, hgt0) {
  }
  //MatrixPtr matrix;
  /* ********************************************************************** */
  static OrgPtr Abiogenate() {
    OrgPtr child = new Org(DefaultWdt, DefaultHgt);
    child->Rand_Init();
    return child;
  }
  /* ********************************************************************** */
  OrgPtr Spawn() {
    OrgPtr child = new Org(this->wdt, this->hgt);
    child->Copy_From(this);
    return child;
  }
  /* ********************************************************************** */
  void Rand_Init(){
    this->Mutate_Me(1.0);// mutate 100%
  }
  /* ********************************************************************** */
//  void Mutate_Me(double MRate) {// use this if matrix is a property of org
//    this->Mutate_Me(MRate);
//  }
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
