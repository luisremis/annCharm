#include <stdlib.h>
#include <vector>
#include <math.h>
#include <map>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include "liveViz.h"
#include "pup_stl.h"
#include "Neuron.h"
#include "neuralNetwork.decl.h"

#define ITERATION (1)
#define LB_FREQ   (10)
#define BACKWARD_FREQ (1)

using namespace std;

/*readonly*/
CProxy_Main  mainProxy;
std::vector<CProxy_NeuronGroup> layerProxies;
std::vector<double> oracle;
std::map<unsigned int, unsigned int> mapLayerToNeurons;

unsigned int inputNeurons;
unsigned int nMiddleLayers;
unsigned int middleLayersNeurons;
unsigned int outputNeurons;
unsigned int neuronsPerChare;
unsigned int totalLayers;
std::string trainImageFile;
std::string trainLabelFile;

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

      if(m->argc == 8) { 
	trainImageFile = m->argv[6];
	trainLabelFile = m->argv[7];
      }

      //Deallocate space for message
      delete m;
 
      totalLayers = nMiddleLayers + 2; //MiddleLayers, input and ouput

      //This map mas be ready when neurons gruops are created!!!
      mapLayerToNeurons[0] = inputNeurons;
      mapLayerToNeurons[totalLayers - 1] = outputNeurons;
      for (int i = 0; i < nMiddleLayers; ++i)
      { //Add middle layers
        mapLayerToNeurons[i + 1] = middleLayersNeurons;
      }

      //Input Layer
      layerProxies.push_back(CProxy_NeuronGroup::ckNew(0, neuronsPerChare, ceil(inputNeurons/neuronsPerChare)) );

      for (int i = 0; i < nMiddleLayers; ++i)
      { //Add middle layers
        layerProxies.push_back(CProxy_NeuronGroup::ckNew(i + 1,neuronsPerChare, ceil(middleLayersNeurons/neuronsPerChare)) );
      }

      //Output Layer
      layerProxies.push_back(CProxy_NeuronGroup::ckNew(totalLayers - 1,neuronsPerChare, ceil(outputNeurons/neuronsPerChare)) );

      //Inizialize oracle array
      for (int i = 0; i < ITERATION; ++i)
      {
        //std::vector<double> vec;
        //for (int i = 0; i < outputNeurons; ++i)
        //{
        //  vec.push_back(1.0f); // TODO ! LOAD REAL VALUES FORM ADITYA'S VECTOR
        //}
        oracle.push_back(0.0f);// TODO ! LOAD REAL VALUES FORM ADITYA'S VECTOR
      }

      //When inactive, after sending and receiving, call end
      CkStartQD(CkCallback(CkReductionTarget(Main, done), mainProxy));

      // std::vector<double> v(0.0f, inputNeurons);
      // layerProxies.at(0).activate(v);
    }

    void initializationDone() { 
	    CkPrintf("Input vectors assigned. Starting runForward ...\n");    
	    for (int i = 0; i < layerProxies.size(); ++i)
	    {
		    layerProxies.at(i).runForward(); //MAYBE SYNC PROBLEM WITH ALL THE LAYERS.
	    }
    }

    void creationDone(){

      static unsigned int counter = 0;
      ++counter;

      if (counter == totalLayers)
      {
        CkPrintf("Creation complete! Assigning input vectors ...\n");
        // Set inputVector for Layer 0
        //Load Input Image Vector for Layer 0

        std::ifstream fp(trainImageFile.c_str());
        std::string line;
        vector< vector<double> > fullInputVector;
        while(std::getline(fp, line)) {
          vector<double> currVec;
          istringstream iss(line);
          do { 
            std::string currNum;
            iss >> currNum;
            if(currNum == "") break;
            currVec.push_back(atof(currNum.c_str()));	
          } while(iss);
          fullInputVector.push_back(currVec);
        }

        layerProxies.at(0).setInputVector(fullInputVector);
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
    vector< vector<double> > inputVectors;

    NeuronGroup(int layer, int nNeurons):
                  layerIndex(layer)
    {
      __sdag_init();
      iteration = 0;
      usesAtSync=CmiTrue;

      int neuronsPreviousLayer, neuronsNextLayer;

      if (layerIndex == 0 )
        neuronsPreviousLayer = 0;//mapLayerToNeurons[0];
      else
        neuronsPreviousLayer = mapLayerToNeurons[layerIndex - 1];

      if (layerIndex == totalLayers-1 || layerIndex == 0)
        neuronsNextLayer = 0;
      else
        neuronsNextLayer = mapLayerToNeurons [layerIndex + 1];

      for (int i = 0; i < neuronsPreviousLayer; ++i)
      {
        incomingAj.push_back(0.0f); //Just allocate the space, the value does not matter. 
      }

      for (int i = 0; i < neuronsNextLayer; ++i)
      {
        incomingErrs.push_back(0.0f); //Just allocate the space, the value does not matter.
      }

      for (int i = 0; i < nNeurons; ++i)
      {
        neurons.push_back(Neuron(0, neuronsPreviousLayer));
        values.push_back(0.0f); //Just allocate the space, the value does not matter.
        errors.push_back(0.0f); //Just allocate the space, the value does not matter.
      }
      
      //CkPrintf("Layer %2d NeuronGroup %04d has %04d Neurons \n", layerIndex, thisIndex, neurons.size());
      
      CkCallback cb(CkReductionTarget(Main, creationDone), mainProxy);
      contribute(cb);
    }

    NeuronGroup(CkMigrateMessage* m) {}

    void pup(PUP::er &p){
      CBase_NeuronGroup::pup(p);
      __sdag_pup(p);
      p|iteration;
    }

    void setInputVector( vector< vector<double> > fullVector) {
      for(int i = 0; i < fullVector.size(); i++) { 
        vector<double> currVec;
        for(int j = thisIndex*neuronsPerChare; j<(thisIndex+1)*neuronsPerChare; j++) {
          currVec.push_back(fullVector[i][j]);
        }
        inputVectors.push_back(currVec);
      }
      CkCallback cb(CkReductionTarget(Main, initializationDone), mainProxy);
      contribute(cb);
    }

    void activate(){
      for (int i = 0; i < neurons.size(); ++i)
      {
        neurons.at(i).activate(incomingAj);
      }
    }

    void collectValues () {
      for (int i = 0; i < neurons.size(); ++i)
      {
        values.at(i) = neurons.at(i).x;
      }
    }

    void collectValues (int image_id) {
	    for (int i = 0; i < neurons.size(); ++i)
	    {
		    values.at(i) = inputVectors[image_id][i];
	    }
    }

    void calculateErrors()
    {
      for (int i=0; i < neurons.size(); ++i)
      {
        neurons[i].calculateError(incomingErrs);
      }
    }

    void calculateOutputError()
    {
      for (int i=0; i < neurons.size(); ++i)
      {
        if (oracle[iteration] == neurons.size()*thisIndex + i)
          neurons[i].calculateOutputError(1.0f);
        else
          neurons[i].calculateOutputError(0.0f);
      }
    }

    void printValues()
    {  
      for (int i=0; i < neurons.size(); i++){
        CkPrintf("Group: %d - Neuron: %d  - Value: %f \n", thisIndex, i, neurons[i].x);
      }
    }

    void collectErrors()
    {
      for(int i=0; i < neurons.size(); i++) {
        errors[i] = neurons[i].collectError();
      }
    }

    void exportErrors(vector<vector<double> > &errorBuffer) {
      //errorBuffer.push_back(this->errors);
    }
  private:

};

#include "neuralNetwork.def.h"
