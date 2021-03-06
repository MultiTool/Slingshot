#ifndef SYNAPSE_HPP_INCLUDED
#define SYNAPSE_HPP_INCLUDED

#include "Matrix.hpp"
#include "Org.hpp"

#define WeightAmp 2.0;

class Node; // forward
/* ********************************************************************** */
class Synapse;// forward
typedef Synapse *SynapsePtr;
typedef std::vector<SynapsePtr> SynVec;
class Synapse {
public:
  Node *USNode, *DSNode;
  double FireVal, Corrector;// do we need separate //, FireSq
  double WeightDef, Weight;
  MatrixPtr genome;
  int numins=Org::DefaultWdt;//, Org::DefaultHgt
  VectPtr state;
  // These are pies I put on my windowsill for Node to eat and leave a tip.
  double DSCorrVal, USCorrVal, DSFireVal, USFireVal, SumFireSq;
  /* ********************************************************************** */
  Synapse() {
    state = new Vect(this->numins);
  }
  /* ********************************************************************** */
  ~Synapse() {
    delete state;
  }
  /* ********************************************************************** */
  inline double GetWeightedFire() {
    return this->FireVal*this->Weight;
  }
  /* ********************************************************************** */
  inline double GetWeightedCorrector() {
    return this->Corrector*this->Weight;
  }
  /* ********************************************************************** */
  inline double GetGenCorrector() {
    return this->USCorrVal;
  }
  /* ********************************************************************** */
  inline void Assign_Fire(double Fire) {
    this->FireVal=Fire; //this->FireSq=Fire*Fire;
  }
  /* ********************************************************************** */
  void Randomize_Weight() {
    this->WeightDef = (frand()-0.5) * WeightAmp;// to do: do this with a distribution change
    this->Weight = this->WeightDef;// to do: do this with a distribution change
  }
  /* ********************************************************************** */
  void Reset_Weights() {// reset network back to original random values
    this->Weight = this->WeightDef;
    this->DeState();
  }
  /* ********************************************************************** */
  void DeState() {// remove all stateful information from org/matrix, but keep weight
    this->FireVal=0.0; this->Corrector=0.0;
    DSCorrVal=0.0; USCorrVal=0.0; DSFireVal=0.0; USFireVal=0.0; SumFireSq=0.0;
    state->Fill(0.0);
  }
  /* ********************************************************************** */
  void Attach_Genome(MatrixPtr genome0) {
    this->genome=genome0;
  }
  /* ********************************************************************** */
  void Run_Process() {
    // confusion. work out order of events.

    double Factor = 1.0;//0.2;// to squeeze more range with the sigmoid function
    int dex=0;
    // first load inputs
    state->ray[dex++] = this->DSFireVal; state->ray[dex++] = this->USFireVal;// upfire, downfire
    state->ray[dex++] = this->DSCorrVal; state->ray[dex++] = this->Weight;
    state->ray[dex++] = this->SumFireSq;
    for (int cnt=0; cnt<dex; cnt++) { state->ray[cnt]*=Factor; }// squeeze

    genome->Iterate(state, 3, state);// can we use the same vector for input and output?

    dex=0;

    for (int cnt=0; cnt<2; cnt++) { state->ray[cnt]/=Factor; }// un-squeeze
    // then write outputs
    //this->DSFireVal = state->ray[dex++];// wrong
    this->USCorrVal = state->ray[dex++];
    this->Weight += state->ray[dex++];
    /*
    every creature:
    input upfire, downfire
    output to downfire SumFireSq, input from downfire SumFireSq, for single-layer weight change
    output to self, input from self
    output to weight change
    input from downfire corrector
    input from current weight

    downfire sum exists to create normal of all input fires to one node. used for weight adjustment.

    */
  }
};


#endif // SYNAPSE_HPP_INCLUDED
