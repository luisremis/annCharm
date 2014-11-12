#ifndef NEURON_H
#define NEURON_H

/*
*Neuron
*/

#define NEURON_TYPE_INPUT  0
#define NEURON_TYPE_OUTPUT 1
#define NEURON_TYPE_HIDDEN 2

#define DIRECTION_FOWARD   0
#define DIRECTION_BACKWARD 1

class Neuron  {
public:
    double x;
    std::vector<double> weight;

    Neuron(int type, unsigned int neuronsPreviousLayer) {

      x = 0.0f; //TODO

      for (int i = 0; i < neuronsPreviousLayer; ++i)
      {
        weight.push_back(drand48());
      }
    }

    void pup(PUP::er &p){
      p|x;
    }

    void activate(std::vector<double> & aj){
      
      if (aj.size() != weight.size())
      {
        CkPrintf("FATAL ERROR IN NEURON WEIGHT OPERATION: aj,weight %04d, %04d \n", aj.size(), weight.size());

      }

      x = 0.0f;
      for (int i = 0; i < weight.size(); ++i)
      {
        x += weight.at(i) * aj.at(i);
      }

      x = neuronFunction(x);
    }

    double neuronFunction(double x){

      //Function to be defined
      return x + 0.0f ; 
    }

};

#endif
