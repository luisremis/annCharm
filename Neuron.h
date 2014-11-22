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
    double error;
    double target;
    std::vector<double> weight;

    Neuron(int type, unsigned int neuronsPreviousLayer) {

      x = 1.0f; //TODO
      error = 0.0f; //TODO
      //target = 0.0f; //TODO FOR RECODING OUTPUT

      for (int i = 0; i < neuronsPreviousLayer; ++i)
      {
        weight.push_back(1.0f);//drand48());
      }
    }
/*
    Neuron(int type, unsigned int neuronsPreviousLayer, double target) {
      //Constructor for output layer neuron
      x = 1.0f; //TODO
      error = 0.0f; //TODO
      //this->target = target; //TODO FOR RECODING OUTPUT

      for (int i = 0; i < neuronsPreviousLayer; ++i)
      {
        weight.push_back(1.0f);//drand48());
      }
    }
*/
    void pup(PUP::er &p){
      p|x;
    }

    void activate(std::vector<double> & aj){

      if (aj.size() != weight.size())
      {
        CkPrintf("FATAL ERROR IN NEURON WEIGHT OPERATION: aj,weight %04d, %04d \n", aj.size(), weight.size());
      }

      float sum = 0.0f;
      for (int i = 0; i < weight.size(); ++i)
      {
        sum += weight.at(i) * aj.at(i);
        //CkPrintf("sum: %f \n", sum);
      }

      x = neuronFunction(sum);
      //CkPrintf("x: %f \n", x);
    }

    void calculateError(std::vector<double> & incomingErrs, bool isHidden){
      if (incomingErrs.size() != weight.size()) {
        CkPrintf("FATAL ERROR IN NEURON WEIGHT OPERATION: err,weight %04d, %04d \n", incomingErrs.size(), weight.size());
      }
      if (isHidden) {
        /*
         *calculate the error of hidden layer
         */

         float tmp = 0.0;
         for (int k = 0; k<incomingErrs.size(); k++){
           tmp += incomingErrs[k] * this->weight[k];
         }

         error = x * (1 - x) * tmp;

         for (int j = 0; j<this->weight.size(); j++) {
           this->weight[j] += incomingErrs[j] * x;
         }

      } else {
        /*
         *calculate the error of output layer
         */
         error = x * ( 1 - x) * (target - x);

         for (int j = 0; j<this->weight.size(); j++) {
           this->weight[j] += incomingErrs[j] * x;
         }
      }
    }

    double neuronFunction(double x){

      //Function to be defined
      return x + 0.0f ;
    }

};

#endif
