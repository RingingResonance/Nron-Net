#include <iostream>
#include <cmath>
#include <thread>
#include "nnet.h"


#define iputsCNT 10000      //100 inputs per nron for now.
#define nNiputs 10000      //total number of main data inputs of this net.
//#define nronCNT 3000      //300 nrons. 100 X 3
#define oputsCNT 10000     //total number of outputs.
#define lyrCNT 3
#define nronThrds 8
#define bequiet 1



using namespace std;

    double* nNetInput;
    double* nNetOutput;
    double* nNetSupervise;

    void weightInit(void);
    void plasticityCalc(void);
    float TanH(double);
    double plastic;
    unsigned int layers = 0;



int main()
{
    netInit();
    unsigned int x = 0;

    /* Keep between -100 and 100 or you will have to re-tune the loop! */
    nNetInput[0] = 10;
    nNetInput[1] = 100;
    nNetInput[2] = 100;
    nNetInput[3] = 10;
    nNetInput[4] = 10;
    x = 5;
    while (x < nNiputs){
        nNetInput[x] = 100;
        x++;
    }

    /* Keep between -100 and 100 or you will have to re-tune the loop! */
    nNetSupervise[0] = 100;
    nNetSupervise[1] = 20;
    nNetSupervise[2] = 50;
    nNetSupervise[3] = 80;
    nNetSupervise[4] = 100;
    x = 5;
    while (x < oputsCNT){
        nNetSupervise[x] = 100;
        x++;
    }

    x = 0;
    unsigned int v = 0;
    cout << "Nron Net Started! \n";
    while (1 == 1){
        netRun();
            cout << nNetOutput[0] << " " << nNetOutput[1] << " ";
            cout << nNetOutput[2] << " " << nNetOutput[3] << " ";
            cout << nNetOutput[4] << " " << nNetOutput[5] << "\n";
            v = 0;
        x++;
    }
    cout << "done. \n";
    cin >> x;
    return 0;
}

Nvars::Nvars(void)
{
    BackOuts = new double[iputsCNT];
    weights = new double[iputsCNT];
    inConnect = new unsigned int[iputsCNT];     //where our inputs connect to.
}

void weightInit(void){
    cout << "Presetting weights... \n";
    unsigned int i = 0;
    unsigned int n = 0;
    while (i < nronCNT){
        n = 0;
        while (n < iputsCNT){
            nron[i].weights[n] = 0.1;
            n++;
        }
    i++;
    }
    cout << "Done. \n";
}

void netInit(void){
    //cout << "Creating Nron vars in memory... \n";

    cout << "Initializing Nron Net... \n" << "Stage 1... \n";
    nNetInput = new double[nNiputs];
    nNetOutput = new double[oputsCNT];
    nNetSupervise = new double[oputsCNT];
    unsigned int i = 0;
    unsigned int n = 0;

    /* Initialize the first layer.*/
    while (i < iputsCNT){
        n = 0;
        while (n < iputsCNT){
            nron[i].inConnect[n] = n;
            n++;
        }
        nron[i].nronLayer = 0;
        i++;
    }

    cout << "Stage 2... \n";
    /* Initialize the hidden and output layers. */
    i = iputsCNT;
    unsigned int x = 0;
    unsigned int why = 0;
    while (i < nronCNT){
        n = 0;
        while (n < iputsCNT){
            nron[i].inConnect[n] = n + x;
            n++;
        }
        nron[i].nronLayer = 1;
        if (why == iputsCNT - 1){
            x += iputsCNT;
            why = 0;
        }
        else {
            why++;
        }
        i++;
    }

    cout << "Stage 3... \n";
    /* Finish Initializing the output layer. */
    i = nronCNT - oputsCNT;
    x = 0;
    while (i < nronCNT){
        nron[i].outConnect = x;
        nron[i].nronLayer = 2;
        x++;
        i++;
    }
    cout << "Done. \n";
    weightInit();
}

void ForwardNetRun(void){
    int i = 0;
    //forwardCalc(i);
    //forwardConnect(i);
}

void netRun(void){
    thread threads[nronThrds];

int i = 0;
    // spawn n threads:
    for (int i = 0; i < nronThrds; i++) {
        threads[i] = thread(threader::forwardConnect, i);
    }
    for (auto& th : threads) {
        th.join();
    }
    // spawn n threads:
    for (int i = 0; i < nronThrds; i++) {
        threads[i] = thread(threader::forwardCalc, i);
    }
    for (auto& th : threads) {
        th.join();
    }

    // spawn n threads:
    for (int i = 0; i < nronThrds; i++) {
        threads[i] = thread(threader::backConnect, i);
    }
    for (auto& th : threads) {
        th.join();
    }

    // spawn n threads:
    for (int i = 0; i < nronThrds; i++) {
        threads[i] = thread(threader::L2BackConnect, i);
    }
    for (auto& th : threads) {
        th.join();
    }

    // spawn n threads:
    for (int i = 0; i < nronThrds; i++) {
        threads[i] = thread(threader::BackPutsDiv, i);
    }
    for (auto& th : threads) {
        th.join();
    }

    // spawn n threads:
    for (int i = 0; i < nronThrds; i++) {
        threads[i] = thread(threader::backCalc, i);
    }
    for (auto& th : threads) {
        th.join();
    }

    unsigned int x;
    x = lyrCNT * 2;
    if (layers > x){
        // spawn n threads:
        for (int i = 0; i < nronThrds; i++) {
            threads[i] = thread(threader::weightCalc, i);
        }
        for (auto& th : threads) {
            th.join();
        }
        //weightCalc();
        layers = 0;
    }
    else {
        layers++;
    }
}

void threader::forwardCalc(unsigned int t){
    register unsigned int i = 0;
    register unsigned int n = 0;
    register double adder = 0;
    unsigned int tSplit;
    unsigned int tCount = 0;
    unsigned int tEnd;

    //cout << t << "\n";
    tSplit = nronCNT / nronThrds;
    if (t != 0){
        tCount = tSplit * t;
    }
    tEnd = tSplit + tCount;
    i = tCount;
    while (i < tEnd){
        n = 0;
        adder = 0;
        if (nron[i].nronLayer == 0){
            while (n < iputsCNT){
                adder += nNetInput[nron[i].inConnect[n]] * nron[i].weights[n];      //First layer
                //adder += 1 * nron[i].weights[n];
                ++n;
                adder += nNetInput[nron[i].inConnect[n]] * nron[i].weights[n];
                ++n;
            }
        }
        else {
            while (n < iputsCNT){
                adder += nron[nron[i].inConnect[n]].output * nron[i].weights[n];    //Hidden layer or output layer.
                //adder += 1 * nron[i].weights[n];
                ++n;
                adder += nron[nron[i].inConnect[n]].output * nron[i].weights[n];
                ++n;
            }
        }
        nron[i].PrepOut = adder / iputsCNT;
        ++i;
    }
    if (bequiet != 1)
    cout << "Thread " << t << " finished forward calculation. \n";
}

void threader::forwardConnect(unsigned int t){
    register unsigned int i = 0;
    unsigned int tSplit;
    unsigned int tCount = 0;
    unsigned int tEnd;
    //cout << t << "\n";
    tSplit = nronCNT / nronThrds;
    if (t != 0){
        tCount = tSplit * t;
    }
    tEnd = tSplit + tCount;
    i = tCount;

    //L2tst = nron[10].nronLayer;
    while (i < tEnd){
        nron[i].output = nron[i].PrepOut;
        if (nron[i].nronLayer == 2){
            nNetOutput[nron[i].outConnect] = nron[i].output;
            //nNetOutput[1] = 1;
        }
        ++i;
    }
    if (bequiet != 1)
    cout << "Thread " << t << " finished forward connect. \n";
}

float TanH(double x){
    float y = 0;
    double temp1 = 0;
    double temp2 = 0;
    x *= 2;
    temp1 = pow(2.71828,x);
    temp2 = temp1;
    temp1 -= 1;
    temp2 += 1;
    y = temp1 / temp2;
    return y;
}

void threader::backCalc(unsigned int t){
    register unsigned int i = 0;
    register unsigned int n = 0;
    register double error = 0;
    unsigned int tSplit;
    unsigned int tCount = 0;
    unsigned int tEnd;
    //cout << t << "\n";
    tSplit = nronCNT / nronThrds;
    if (t != 0){
        tCount = tSplit * t;
    }
    tEnd = tSplit + tCount;
    i = tCount;

    //double backTemp = 0;

    /* Back propagation error calculations. */
    //L1tst = nron[20].backput - nron[20].output;
    while (i < tEnd){
        //if (nron[i].nronLayer != 2){
            //backTemp = TanH(nron[i].backput);
            error = nron[i].backput - nron[i].output;
            if (nron[i].nronLayer != 0){
                n = 0;
                while (n < iputsCNT){
                    nron[i].BackOuts[n] = nron[i].backput;
                    ++n;
                }
            }
        //}
        nron[i].backerr = error;
        ++i;
        //cout << "Test";
    }
    if (bequiet != 1)
    cout << "Thread " << t << " finished back calculation. \n";
}

void threader::weightCalc(unsigned int t){
/* Weight calculations. */
    register unsigned int i = 0;
    register unsigned int n = 0;
    register double wCalc = 0.0001;    //time in seconds between each process. Tuned for -100 to 100
    register double a = 0;
    unsigned int tSplit;
    unsigned int tCount = 0;
    unsigned int tEnd;
    //cout << t << "\n";
    tSplit = nronCNT / nronThrds;
    if (t != 0){
        tCount = tSplit * t;
    }
    tEnd = tSplit + tCount;
    i = tCount;


    while (i < tEnd){
        n = 0;
        while (n < iputsCNT){
            a = nron[i].backerr * nron[nron[i].inConnect[n]].output;
            nron[i].weights[n] += (a * wCalc) /* plastic*/;        //Supposed to Integrate. Timing is off for now.
            if (nron[i].weights[n] > 10){
                nron[i].weights[n] = 10;
            }
            else if (nron[i].weights[n] < -10){
                nron[i].weights[n] = -10;
            }
            ++n;
        }
        ++i;
    }
    if (bequiet != 1)
    cout << "Thread " << t << "finished weight calculation. \n";
}

/* Fix Me. Why is this so slow? */
void threader::backConnect(unsigned int t){
    unsigned int i = 0;
    register unsigned int x = 0;
    register unsigned int w = 0;
    register float adder = 0;
    unsigned int layr = 0;
    unsigned int backnum = 0;
    unsigned int tSplit;
    unsigned int tCount;
    unsigned int tEnd;
    //cout << t << "\n";

    //L1tst = nron[20].backput;
    layr = iputsCNT;
    while (i < nronCNT){
        if (backnum == iputsCNT){
            backnum = 0;
            layr += iputsCNT;
        }
        else{
            backnum++;
        }

        if (nron[i].nronLayer != 2){
            tCount = 0;
            tSplit = iputsCNT / nronThrds;
            if (t != 0){
                tCount = tSplit * t;
            }
            tEnd = tSplit + tCount;
            x = tCount;

            adder = 0;
            while (x < tEnd){
                w = layr + x;
                adder += nron[w].BackOuts[backnum] * nron[w].weights[backnum];
                ++x;
            }
            nron[i].backput = adder;
        }
        //cout << "Test \n" << i;
        ++i;
    }
    if (bequiet != 1)
    cout << "Thread " << t << " finished back connect. \n";
}

void threader::BackPutsDiv(unsigned int t){
    unsigned int i = 0;
    register float z = 0;
    unsigned int tSplit;
    unsigned int tCount = 0;
    unsigned int tEnd;
    //cout << t << "\n";
    tSplit = nronCNT / nronThrds;
    if (t != 0){
        tCount = tSplit * t;
    }
    tEnd = tSplit + tCount;
    i = tCount;

    while (i < tEnd){
        if (nron[i].nronLayer != 2){
            z = nron[i].backput;
            z /= iputsCNT;
            nron[i].backput = z;
        }
        ++i;
    }
    if (bequiet != 1)
    cout << "Thread " << t << " finished back divide. \n";
}

void threader::L2BackConnect(unsigned int t){
    unsigned int i = 0;
    unsigned int tSplit;
    unsigned int tCount = 0;
    unsigned int tEnd;
    //cout << t << "\n";
    tSplit = nronCNT / nronThrds;
    if (t != 0){
        tCount = tSplit * t;
    }
    tEnd = tSplit + tCount;
    i = tCount;

   while (i < tEnd){
        if (nron[i].nronLayer == 2){
            nron[i].backput = nNetSupervise[nron[i].outConnect];
        }
        ++i;
    }
    if (bequiet != 1)
    cout << "Thread " << t << " finished final connect. \n";
}

/* I say things the way that I say things so that my brain knows that what I am saying is true to my own understanding.
If I say something that is false then I am either lying or my understanding is incomplete. */

void plasticityCalc(void){
    unsigned int i = 0;
    plastic = 0;
    float x = 0;
    float y = 0;
    float z = 0;
    while (i < oputsCNT){
        x = nNetSupervise[i];
        y = nNetOutput[i];
        if (x - y < 0){
            plastic += (x - y) * -1;
        }
        else{
            plastic += x - y;
        }
        i++;
    }
    z = plastic;
    plastic = z;
    plastic /= oputsCNT;
    plastic = TanH(plastic);
    if (plastic > 1){
        plastic = 1;
    }
    else if (plastic < -1){
        plastic = -1;
    }
    //L1tst = plastic;
}




