#include <iostream>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include "liveViz.h"
#include "pup_stl.h"
#include "Neuron.h"
#include "neuralNetwork.decl.h"

#define ITERATION (2000)
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
std::string testImageFile;
std::string testLabelFile;

/*readonly*/

using namespace std;

class Main: public CBase_Main {

  public:
    int iteration;
    std::string imageFile;
    std::string labelFile;
    int phase;

    Main(CkArgMsg* m) {

      phase = 0 ; //Traning first

      mainProxy = thisProxy;
      inputNeurons        = atoi(m->argv[1]);
      nMiddleLayers       = atoi(m->argv[2]);
      middleLayersNeurons = atoi(m->argv[3]);
      outputNeurons       = atoi(m->argv[4]);
      neuronsPerChare     = atoi(m->argv[5]);

      if(m->argc == 10) { 
      	trainImageFile = m->argv[6];
      	trainLabelFile = m->argv[7];
        testImageFile = m->argv[8];
        testLabelFile = m->argv[9];
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
      std::ifstream fp(trainLabelFile.c_str());
      std::string line;
      while(std::getline(fp, line)) { 
	      oracle.push_back(atof(line.c_str()));
      }

      //When inactive, after sending and receiving, call end
      //CkStartQD(CkCallback(CkReductionTarget(Main, done), mainProxy));
    }

    void readFromFiles() {

      std::ifstream fp(imageFile.c_str());
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

      CkPrintf("Image File: %s \n", imageFile.c_str() );
      CkPrintf("Images: %d - dim: %d\n", fullInputVector.size(), (int)sqrt(fullInputVector.at(0).size()) );

      //Inizialize oracle array
      //initOracle();

      layerProxies.at(0).setInputVector(fullInputVector);
    }

    void initOracle(){

      std::fstream file(labelFile.c_str());
      oracle.clear();

      int aux;

      while (1) {
        
          file >> aux;
          if(file.eof())
            break;
          oracle.push_back(aux);
      }

      CkPrintf("Oracle File: %s \n", labelFile.c_str() );
      CkPrintf("Oracle Inizialized with %d values. \n", oracle.size());    
    }

    void initializationDone() { 
	    CkPrintf("Input vectors assigned. Starting run ...\n");    
	    for (int i = 0; i < layerProxies.size(); ++i)
	    {
		    layerProxies.at(i).run(); //MAYBE SYNC PROBLEM WITH ALL THE LAYERS.
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

        imageFile = trainImageFile;
        labelFile = trainLabelFile; 

        readFromFiles();
      } 
    }

    void iterationsCompleted(){

      static unsigned int counter = 0;
      ++counter;

      if (counter == totalLayers)
      {
        CkPrintf("All iterations completed ...\n");
        // Set inputVector for Layer 0
        //Load Input Image Vector for Layer 0

        /*if (phase == 0){
            imageFile = testImageFile;
             //labelFile = testLabelFile;  //Maybe no need
             readFromFiles();
             CkPrintf("*** Traning Done *** \n");
             phase = 1;
             counter = 0;
        }
        else*/{
            CkPrintf("*** Done *** \n");
            CkExit();
        }
      }
    }

    void setIteration(unsigned int iter){

      iteration = iter;
      CkPrintf("Iteration done in main: %d \n", iteration);
    }

    void forwardComplete(){
        
        //CkPrintf("Forward complete! Iteration: %d \n", iteration);

        //Go to backward face every BACKWARD_FREQ iterations
        for (int i = 0; i < layerProxies.size(); ++i)
        {
          //layerProxies.at(i).runBackward(); //MAYBE SYNC PROBLEM WITH ALL THE LAYERS.
        } 
    }

    void backwardComplete(){

        //CkPrintf("Backward complete ! \n");
        
        //Go to backward face every BACKWARD_FREQ iterations
        for (int i = 0; i < layerProxies.size(); ++i)
        {
          //layerProxies.at(i).run(); //MAYBE SYNC PROBLEM WITH ALL THE LAYERS.
        } 
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
    int j_ci;
    int nIndex_ci;
    int blockT;

    unsigned int layerIndex;
    int phase;
    int maxIterations;
    vector<Neuron> neurons; //Maybe not needed
    vector<double> incomingAj; //Incoming values
    vector<double> values;
    vector<double> errors;
    vector< vector<double> > incomingErrs;
    vector< vector<double> > inputVectors;
    vector<double> outgoingErrs; //weighted errors are sent back per neuron
 
    NeuronGroup(int layer, int nNeurons):
                  layerIndex(layer)
    {
      __sdag_init();
      iteration = 0;
      usesAtSync=CmiTrue;
      phase = 0;
      maxIterations = ITERATION;

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

      for(int j = 0; j < nNeurons; j++) {
	      std::vector<double> vec;
	      for (int i = 0; i < neuronsNextLayer; ++i)
	      {
		      vec.push_back(0.0f); //Just allocate the space, the value does not matter.
	      }
        incomingErrs.push_back(vec);
	//CkPrintf("Layer %d, Neuron %d, ASSIGNMENT Error Vec Size: %d\n", layerIndex, thisIndex, incomingErrs[j].size());
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
      
      inputVectors.clear();

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
        //CkPrintf("Value: %f \n", values.at(i) );
      }
    }

    void collectValues (int image_id) {
	    for (int i = 0; i < neurons.size(); ++i)
	    {
        neurons.at(i).x = inputVectors[image_id][i];
		    values.at(i)    = neurons.at(i).x;
	    }
    }

    void updateWeight()
    {
      for (int i=0; i < neurons.size(); ++i)
      {
        if (layerIndex == totalLayers - 1)
          neurons[i].updateWeight(incomingAj); //only for output layer
        else
          neurons[i].updateWeight(incomingAj, incomingErrs[i]);
	//CkPrintf("!!!Error Computed for Neuron %d: %lf\n", thisIndex, neurons[i].error);
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
	//CkPrintf("ThisIndex: %d, Activation for Neuron_%d: %lf, Error: %lf\n", thisIndex, i, neurons[i].x, neurons[i].error);
      }
    }

    void printValues()
    {  
      for (int i=0; i < neurons.size(); i++){
        CkPrintf("Group: %d - Neuron: %d  - Value: %f \n", thisIndex, i, neurons[i].x);
      }
    }

    void collectErrors(int neuronIndex)
    {
	    outgoingErrs.clear();
	    outgoingErrs.resize(0);
	    for(int i = 0; i < mapLayerToNeurons[layerIndex-1]; i++) 
	    { 
		    outgoingErrs.push_back(neurons[neuronIndex].collectError(i)); 
	    }
    }

    void printResult() {
      //Final phase
      collectValues();
      stringstream result;
      double max; 
      int number;
      for (int i = 0; i < values.size(); ++i)
      {
        if (i == 0){
          max = values.at(i);
          number = 0;
        }
        else{
          if (values.at(i) > max)
          {
            max = values.at(i);
            number = i;
          }
        }
        result << values.at(i) << " "; //(int)round(values.at(i)) << " ";
        //result << (int)round(values.at(i)) << " ";
      }
      //std::string aux = result.str();
      //CkPrintf("Output Layer for iteration %3d : %s \n", iteration, aux.c_str());
      CkPrintf("Output iteration %04d : %d - %f - O: %d - %d \n", iteration, number, max,(int) oracle[iteration], number == (int) oracle[iteration]? 1:0);
    }
  private:

};

#include "neuralNetwork.def.h"
