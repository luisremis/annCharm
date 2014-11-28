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
    double target; //Value from the oracle vector.
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

    double collectError(int neuronIndex)
    {
    /*  double sum =0;

      for (int i = 0; i < weight.size(); ++i)
      {
        sum += error * weight[i];
      }
      return sum;
    */
      CkAssert(weight.size() > neuronIndex);
      return weight.at(neuronIndex)*error;
    }

    void calculateError(std::vector<double> & errWeightVec, std::vector<double>& aj)
    {
      /*
      This function calculates the errors and updates the weight
      */

      /* error = 0.0;
      for (int i = 0; i < incomingErrs.size(); ++i){
        error += incomingErrs[i] ;
      }

      error *= x * (1 - x);

      for (int i = 0; i < weight.size(); ++i) {
        weight[i] += error * x;
      } */

      error = 0.0f;
      for (int i = 0; i < errWeightVec.size(); i++) {
        error += errWeightVec[i];
      }
      error = error*x*(1-x);

      for (int i = 0; i < weight.size(); ++i) {
	      //weight[i] += error * x;
	      weight[i] += error * aj[i];
      }
    }

    void calculateOutputError(double oracle, std::vector<double>& aj)
    {    
        /*
        This function calculates the errors and updates the weight
        of the output layer only
        */
        //error = x * (1 - x) * (oracle - x); 
        //TODO: @aditya CHECK THIS equation! and keep summing error until runBackward is called
        error = (1-x)*(oracle-x);

	//TODO: hold off this update until run backward is called
	//TODO: error should be multiplied by incoming aj and not x, equation is w_ji+= delta_j*a_i
        for (int i = 0; i < weight.size(); ++i) {
          //weight[i] += error * x;
          weight[i] += error * aj[i];
        }
    }

    double neuronFunction(double x){

      //Function to be defined
      return x + 0.0f ;
    }

};

#endif
