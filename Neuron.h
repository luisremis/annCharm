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
    double dx;
    double error;
    double target; //Value from the oracle vector.
    std::vector<double> weight;

    Neuron(int type, unsigned int neuronsPreviousLayer) {

      x = drand48();
      error = 0.0f; //TODO

      for (int i = 0; i < neuronsPreviousLayer; ++i)
      {
        weight.push_back(drand48() *pow(10, -3));
        //weight.push_back(pow(10,-3)*(drand48()-drand48()));
        //weight.push_back(drand48());
        //weight.push_back(1.0f);
        //CkPrintf("Weight %lf\n", weight[i]);
      }
    }

    void pup(PUP::er &p){
      p|x;
      p|dx;
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
        //CkPrintf("sum: %f aj: %f \n", sum, aj.at(i));
      }

      x = neuronFunction(sum);
      dx = dNeuronFunction(sum);
      //CkPrintf("x: %f %f\n", x, sum);
    }

    double collectError(int nIndex)
    {
      CkAssert(weight.size() > nIndex);
      return weight.at(nIndex)*error;
    }

    void updateWeight(std::vector<double>& aj, std::vector<double> & incomingErrs)
    {
      /*
      This function calculates the errors and updates the weight
      */

      error = 0.0f;
      for (int i = 0; i < incomingErrs.size(); ++i){
        error += incomingErrs[i] ;
      }

      error *= x * (1 - x);
      //error *= dx;

      for (int i = 0; i < weight.size(); ++i) {
        weight[i] += 0.1*error * aj[i];
      }
    }

    void updateWeight(std::vector<double>& aj){ //this is for the output layer only
        for (int i = 0; i < weight.size(); ++i) {
          weight[i] += 0.1*error*aj[i];
        }
    }

    void calculateOutputError(double oracle)
    {    
        /*
        This function calculates the errors and updates the weight
        of the output layer only
        */
        error = x * (1 - x) * (oracle - x); 
        //error = dx * (1 - dx) * (oracle - x); 
    }

    double neuronFunction(double x){

      double result = 1.0f/(1.0f + exp(-x));

      //result = 0.5f;
      //Function to be defined
      return  result;
      //return x;
    }

    double dNeuronFunction(double x) { 

      double result = neuronFunction(x)/(1.0f-neuronFunction(x));

      if (neuronFunction(x) == 1.0f)
        result = neuronFunction(x) / (1.0f - 0.5f) ;

      //if (neuronFunction(x) == 1.0f)
        //result = 0.5f;

    	return result;
    }
};

#endif
