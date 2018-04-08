/* Generic Neural Network Library
 Copyright 2018 Jarrett Cigainero. */

#ifndef nnet
#define nnet

#define nronCNT 3000

    extern double* nNetInput;
    extern double* nNetOutput;
    extern double* nNetSupervise;

    void netInit(void);
    void netRun(void);
    void ForwardNetRun(void);

struct Nvars {
    public:
        double* BackOuts;          //Back propagation of inputs.
        double* weights;             //Input weights.
        double errInhib;             //Error adjust inhibitor.
        double output;               //Nron output.
        double PrepOut;              //Prepared output.
        double backput;              //Back prop input.
        double backerr;              //Back prop error.
        unsigned int* inConnect;    //Input connections.
        unsigned int nronLayer;    //0 means first layer, 1 means hidden layer, 2 means output layer.
        unsigned int outConnect;    //output connection.
        Nvars(void);         //constructor.
};

Nvars nron[nronCNT];
class threader{
    public:
        static void forwardCalc(unsigned int);
        static void forwardConnect(unsigned int);
        static void weightCalc(unsigned int);
        static void backCalc(unsigned int);
        static void backConnect(unsigned int);
        static void BackPutsDiv(unsigned int);
        static void L2BackConnect(unsigned int);
    };

#endif
