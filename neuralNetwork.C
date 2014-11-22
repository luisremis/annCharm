#include <stdlib.h>
#include <vector>
#include <math.h>
#include <map>
#include "liveViz.h"
#include "pup_stl.h"
#include "Neuron.h"
#include "neuralNetwork.decl.h"

#define ITERATION (1)
#define LB_FREQ   (10)

using namespace std;

/*readonly*/
CProxy_Main  mainProxy;
std::vector<CProxy_NeuronGroup> layerProxies;
std::map<unsigned int, unsigned int> mapLayerToNeurons;

unsigned int inputNeurons;
unsigned int nMiddleLayers;
unsigned int middleLayersNeurons;
unsigned int outputNeurons;
unsigned int neuronsPerChare;
unsigned int totalLayers;
/*readonly*/

using namespace std;

class Main: public CBase_Main {

  public:
    int iteration;
    //unsigned int neuronsPerChare;
    //unsigned int inputNeurons, nMiddleLayers, middleLayersNeurons, outputNeurons;
    //unsigned int totalLayers;

    Main(CkArgMsg* m) {

      mainProxy = thisProxy;
      inputNeurons        = atoi(m->argv[1]);
      nMiddleLayers       = atoi(m->argv[2]);
      middleLayersNeurons = atoi(m->argv[3]);
      outputNeurons       = atoi(m->argv[4]);
      neuronsPerChare     = atoi(m->argv[5]);

      totalLayers = nMiddleLayers + 2; //MiddleLayers, input and ouput

      //Input Layer
      layerProxies.push_back(CProxy_NeuronGroup::ckNew(0, neuronsPerChare, ceil(inputNeurons/neuronsPerChare)) );
      mapLayerToNeurons[0] = inputNeurons;

      for (int i = 0; i < nMiddleLayers; ++i)
      { //Add middle layers
        layerProxies.push_back(CProxy_NeuronGroup::ckNew(i + 1,neuronsPerChare, ceil(middleLayersNeurons/neuronsPerChare)) );
        mapLayerToNeurons[i + 1] = middleLayersNeurons;
      }

      //Output Layer
      layerProxies.push_back(CProxy_NeuronGroup::ckNew(totalLayers - 1,neuronsPerChare, ceil(outputNeurons/neuronsPerChare)) );
      mapLayerToNeurons[totalLayers - 1] = outputNeurons;

      //When inactive, after sending and receiving, call end
      CkStartQD(CkCallback(CkReductionTarget(Main, done), mainProxy));

      // std::vector<double> v(0.0f, inputNeurons);
      // layerProxies.at(0).activate(v);
    }

    void creationDone(){

      static unsigned int counter = 0;
      ++counter;

      if (counter == totalLayers)
      {
          CkPrintf("Creation complete ! \n");

          for (int i = 0; i < layerProxies.size(); ++i)
          {
            layerProxies.at(i).runForward(); //MAYBE SYNC PROBLEM WITH ALL THE LAYERS.
          }
      }

    }

    void forwardComplete(){
      
    }

    void totalNeurons(int total){

      static unsigned int counter = 0;
      ++counter;
      CkPrintf("Total neurons created %2d Counter: %d \n", total, counter);
    }

    void done(){
      CkPrintf("EXIT \n");
      CkExit();
    }

};

// This class represent the cells of the simulation.
/// Each cell contains a vector of particle.
// On each time step, the cell perturbs the particles and moves them to neighboring cells as necessary.
class NeuronGroup : public CBase_NeuronGroup {
  NeuronGroup_SDAG_CODE

  public:
    int iteration;
    int i;
    unsigned int layerIndex;
    vector<Neuron> neurons; //Maybe not needed
    vector<double> incomingAj; //Incoming values
    vector<double> values;
    vector<double> errors;
    vector<double> incomingErrs;

    NeuronGroup(int layer, int nNeurons):
                  layerIndex(layer)
    {
      __sdag_init();
      iteration = 0;
      usesAtSync=CmiTrue;

      int neuronsPreviousLayer;

      if (layerIndex == 0 )
        neuronsPreviousLayer = mapLayerToNeurons[0];
      else
        neuronsPreviousLayer = mapLayerToNeurons[layerIndex - 1];

      for (int i = 0; i < neuronsPreviousLayer; ++i)
      {
        incomingAj.push_back(0.0f); //Just allocate the space, the value does not matter. 
      }

      for (int i = 0; i < nNeurons; ++i)
      {
        neurons.push_back(Neuron(0, neuronsPreviousLayer));
        values.push_back(0.0f); //Just allocate the space, the value does not matter.
      }

      CkCallback cb(CkReductionTarget(Main, creationDone), mainProxy);
      contribute(cb);

      // int size = neurons.size();
      //CkCallback cb(CkReductionTarget(Main, totalNeurons), mainProxy);
      //contribute(sizeof(int), &size, CkReduction::sum_int, cb);

      CkPrintf("Layer %2d NeuronGroup %04d has %04d Neurons \n", layerIndex, thisIndex, neurons.size());
    }

    NeuronGroup(CkMigrateMessage* m) {}

    void pup(PUP::er &p){
      CBase_NeuronGroup::pup(p);
      __sdag_pup(p);
      p|iteration;
    }

    void activate(){

      for (int i = 0; i < neuronsPerChare; ++i)
      {
        neurons.at(i).activate(incomingAj);
      }
    }

    void sayHello(){

    }

    void collectValues (){

      for (int i = 0; i < neurons.size(); ++i)
      {
        values.at(i) = neurons.at(i).x;
      }
    }

    vector<float> readTarget(){

    }

    void calculateErrors(bool isHidden){
      for (int i=0; i < neuronsPerChare; i++){
        neurons[i].calculateError(incomingErrs, isHidden);
      }
    }

    void printValues(){
      
      for (int i=0; i < neurons.size(); i++){
        CkPrintf("Group: %d - Neuron: %d  - Value: %f \n", thisIndex, i, neurons[i].x);
      }

    }

    void collectErrors (){
      for(int i=0; i < neurons.size(); i++) {
        errors[i] = neurons[i].error;
      }
    }
    /*
    void updateWeight(){
      for (int i = 0; i<neurons.size(): i++){
        for (int j = 0; j<neurons[i].weight.size(); j++) {
          neurons[i].weight[j] += errors[j]*values[i];
        }
      }
    }
    */
    void exportErrors(vector<vector<double> > &errorBuffer) {
      errorBuffer.push_back(this->errors);
    }
  private:

};

#include "neuralNetwork.def.h"
