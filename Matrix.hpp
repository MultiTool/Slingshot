#ifndef MATRIX_HPP_INCLUDED
#define MATRIX_HPP_INCLUDED

#include <iostream>
#include <cstdint>
#include <cstring>
#include <vector>

#include "Base.hpp"

/* ********************************************************************** */
class Vect;// forward
typedef Vect *VectPtr;
class Vect {
public:
  int wdt;
  double* ray;
  /* ********************************************************************** */
  Vect(int wdt0) {
    this->wdt = wdt0;
    this->ray = allocsafe(double, wdt);
    for (int cnt=0; cnt<this->wdt; cnt++) {
      this->ray[cnt]=0.01;
    }
  }
  /* ********************************************************************** */
  //Vect():Vect(4) { }
  /* ********************************************************************** */
  ~Vect() {
    freesafe(this->ray);
  }
  /* ********************************************************************** */
  void Copy_From(Vect* source) {
    int ln = std::min(this->wdt,source->wdt);
    std::memcpy(this->ray, source->ray, ln*sizeof(double));
  }
  /* ********************************************************************** */
  double Mult(Vect* other) {
    int ln = std::min(this->wdt, other->wdt);
    double sum=0.0;
    for (int cnt=0; cnt<ln; cnt++) {
      sum+=this->ray[cnt]*other->ray[cnt];
    }
    return sum;
  }
  /* ********************************************************************** */
  double MultFire(Vect* other) {
    double Sum = this->Mult(other);
    return this->ActFun(Sum);
  }
  /* ********************************************************************** */
  double ActFun(double xin) {
    double OutVal;
    OutVal = xin / sqrt(1.0 + xin * xin);/* symmetrical sigmoid function in range -1.0 to 1.0. */
    return OutVal;
    /* General formula: double power = 2.0; OutVal = xin / Math.pow(1 + Math.abs(Math.pow(xin, power)), 1.0 / power); */
  }
  /* ********************************************************************** */
  void Fill(double val) {
    int ln = this->wdt;
    for (int cnt=0; cnt<ln; cnt++) {
      this->ray[cnt] = val;
    }
  }
  /* ********************************************************************** */
  void Mutate(double amp) {
    int ln = this->wdt;
    double val;//, amp = 0.3;
    for (int cnt=0; cnt<ln; cnt++) {
      val = this->ray[cnt];
      val += (frand()*2.0-1.0)*amp;
      this->ray[cnt] = val;
    }
  }
  /* ********************************************************************** */
  void Print_Me() {
    int ln = this->wdt;
    double val=0.0;
    for (int cnt=0; cnt<ln; cnt++) {
      val = this->ray[cnt];
      printf("%f, ",val);
    }
    printf("\n");
  }
};

/* ********************************************************************** */
class Matrix;// forward
typedef Matrix *MatrixPtr;
class Matrix {
public:
  int wdt,hgt;
  VectPtr* ray;
  VectPtr midvec;// persistent fire state
  /* ********************************************************************** */
  Matrix(int wdt0, int hgt0) {
    this->wdt = wdt0; this->hgt = hgt0;
    this->ray = allocsafe(VectPtr, wdt*hgt);
    for (int cnt=0; cnt<this->hgt; cnt++) {
      this->ray[cnt] = new Vect(this->wdt);
    }
    midvec = new Vect(this->wdt);
  }
  /* ********************************************************************** */
  Matrix():Matrix(4,4) {
  }
  /* ********************************************************************** */
  ~Matrix() {
    for (int cnt=0; cnt<this->hgt; cnt++) {
      delete this->ray[cnt];
    }
    freesafe(this->ray);
    delete this->midvec;
  }
  /* ********************************************************************** */
  void Init_Identity() {// create identity matrix
    VectPtr vec;
    for (int cnt=0; cnt<this->hgt; cnt++) {
      vec=this->ray[cnt];
      vec->Fill(0.0);
      vec->ray[cnt]=1.0;
    }
  }
  /* ********************************************************************** */
  void MultFire(Vect* invec, Vect* outvec) {
    VectPtr vec;
    double Fire;
    for (int cnt=0; cnt<this->hgt; cnt++) {
      vec=this->ray[cnt];
      Fire = vec->MultFire(invec);
      outvec->ray[cnt]=Fire;
    }
  }
  /* ********************************************************************** */
  void Iterate_Persistent(Vect* invec, Vect* outvec) {// recurrent firing
    this->midvec->Copy_From(invec);// invec is shorter than midvec, so some recurrent info carries over from last call
    for (int cnt=0; cnt<3; cnt++) {// pass through 3 times total
      this->MultFire(this->midvec, outvec);
      this->midvec->Copy_From(outvec);
    }
  }
  /* ********************************************************************** */
  void Iterate_old(Vect* invec, Vect* outvec) {// recurrent firing
    Vect tempvec(this->hgt);
    this->MultFire(invec, outvec);// not safe
    for (int cnt=1; cnt<3; cnt++) {// pass through 3 times total
      tempvec.Copy_From(outvec);
      this->MultFire(&tempvec, outvec);
    }
  }
  /* ********************************************************************** */
  void Iterate_old2(Vect* invec, Vect* outvec) {// recurrent firing
    Vect tempvec(this->hgt);
    this->MultFire(invec, &tempvec);// safe, more efficient
    for (int cnt=1; cnt<3; cnt++) {// pass through 3 times total
      this->MultFire(&tempvec, outvec);
      tempvec.Copy_From(outvec);
    }
  }
  /* ********************************************************************** */
  void Iterate(Vect* invec, Vect* outvec) {// recurrent firing
    Vect tempvec(this->hgt);
    tempvec.Copy_From(invec);// safe, more flexible
    for (int cnt=0; cnt<3; cnt++) {// pass through 3 times total
      this->MultFire(&tempvec, outvec);
      tempvec.Copy_From(outvec);
    }
  }
  /* ********************************************************************** */
  void Mutate() {
    double amp = 0.3;
    for (int cnt=0; cnt<this->hgt; cnt++) {
      this->ray[cnt]->Mutate(amp);
    }
  }
  /* ********************************************************************** */
  void Print_Me() {
    for (int cnt=0; cnt<this->hgt; cnt++) {
      this->ray[cnt]->Print_Me();
    }
  }
};

/* ********************************************************************** */
class Node;// forward
typedef Node *NodePtr;
class Node {
public:
  double FireVal, CorrVal, CorrSum;
  void AddCorr(double val){ this->CorrSum+=val; }
  //void AddCorr(double val){ this->CorrSum+=val; }
};

/* ********************************************************************** */
class Synapse;// forward
typedef Synapse *SynapsePtr;
class Synapse {
public:
  NodePtr USNode, DSNode;
  MatrixPtr genome;
  VectPtr state;
  double Weight;
  int numins=5;
  /* ********************************************************************** */
  Synapse() {
    state = new Vect(this->numins);
  }
  /* ********************************************************************** */
  ~Synapse() {
    delete state;
  }
  /* ********************************************************************** */
  void Attach_Genome(MatrixPtr genome0) {
    this->genome=genome0;
  }
  /* ********************************************************************** */
  void Run_Process() {
    // confusion. work out order of events.

    int dex=0;
    // first load inputs
    state->ray[dex++] = this->DSNode->FireVal; state->ray[dex++] = this->USNode->FireVal;// upfire, downfire
    state->ray[dex++] = this->DSNode->CorrVal;

    genome->Iterate(state, state);// can we use the same vector for input and output?

    dex=0;
    // then unload outputs
    this->DSNode->FireVal = state->ray[dex++];
    this->USNode->AddCorr(state->ray[dex++]);
    this->Weight += state->ray[dex++];
    /*
every creature:
input upfire, downfire
output to downfire sum, input from downfire sum, for single-layer weight change
output to self, input from self
output to weight change
3 inputs, 3 outputs, more if we want more feedback.

downfire sum exists to create normal of all input fires to one node. used for weight adjustment.

    */
  }
};

/* ********************************************************************** */
class Layer {
public:
  Matrix *weights;
};

#if 0
class Matrix {
public:
  int wdt,hgt;
  double* ray;
  Matrix(int wdt0, int hgt0) {
    this->wdt = wdt0; this->hgt = hgt0;
    this->ray = allocsafe(double, wdt*hgt);
  }
  Matrix():Matrix(4,4) {
  }
  ~Matrix() {
    freesafe(this->ray);
  }
};
#endif // 0

#endif // MATRIX_HPP_INCLUDED

#if 0
/*
so can all the behavior be done with just one matrix?  or do we need a stack of 3 matrices?

so next vectors and matrices can mutate.  then maybe cross.

each network will have X outputs that are also inputs (recurrent). maybe 2? 3?
each network will read the fire values of both upstream and downstream, and know the past through recurrence.
each network will output a value that will be added to the weight of the link it controls.
that it?
but really each creature is not altering weights, it is pushing correctors backward.
it receives a corrector, and pushes it upstream. the upstreamer must add all the corrs together.

next step:
be able to run it on 2 nodes
therefore define node, and link.
creature lives inside the link.
link needs a way to load its values into the creature,
and to get the new values out.
creature should not be blanked between fires?  carry over of prev fires allows delta/hebb stuff.
if not blanked then invec and outvec should be smaller than matrix wdt, hgt.
we would need a persistent tempvec for iterate. midvec. and the midvec must belong to the creature/link, not the matrix genome.

since we will be blanking at first, prev fires will be ignored. that may not be workable.

*/
#endif // 0