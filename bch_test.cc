#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include "bch_codec.h"
using namespace std;


int monte_carlo(int m,int t,unsigned int prim_poly,int ntrials)
{

    bch_control * bch = init_bch(m,t,prim_poly);
    cout << "initialized" << " m=" << bch->m << " n=" << bch->n << " t=" << bch->t << " ecc_bits=" << bch->ecc_bits << " ecc_bytes=" << bch->ecc_bytes << endl;

    int N = (1<<m)-1;
    int msgBits = N - bch->ecc_bits;

    cout << "running " << ntrials << " trials of BCH decoding with up to " << t << " errors\n";
    vector<uint8_t> data( msgBits / 8 );
    cout << " data.size()=" << data.size() << endl;

    for (int trial=0;trial < ntrials;++trial) {
        // make a random message
        for (size_t k=0;k<data.size();++k)
            data[k] = rand()&0xFF;

        // encode it
        vector<uint8_t> ecc( bch->ecc_bytes ,0);
        encode_bch(bch, &data[0],data.size(),&ecc[0]);

        // introduce up to t errors
        int nerrs = rand() % (t+1);
        vector<unsigned int> errLocIn( data.size()*8 );
        for (size_t k=0;k<errLocIn.size();++k) errLocIn[k] = k;
        random_shuffle( errLocIn.begin(),errLocIn.end());
        errLocIn.resize(nerrs);
        sort( errLocIn.begin(),errLocIn.end());
        for (size_t k=0;k < errLocIn.size();++k) {
           int i = errLocIn[k];
           data[i>>3] ^= (1<<(i&7));
        }

        // decode and make sure the right errors were corrected
        vector<unsigned int> errLocOut(nerrs+42);
        int ncorrected = decode_bch(bch, &data[0], data.size(),&ecc[0], NULL,NULL,&errLocOut[0]);
        if (ncorrected>=0) errLocOut.resize(ncorrected);
        sort( errLocOut.begin(),errLocOut.end());

        if (errLocIn != errLocOut) {
            cout << "Input Errors!= Found Errors !!!!" << endl;
            return 1;
        }
    }
    free_bch(bch);
    return 0;
}



int main(int argc, char ** argv)
{
    int m = 9;
    int t = 5;
    unsigned int prim_poly = 0; // zero indicates to use the appropriate element from prim_poly_tab
    int ntrials = 1000;
    return monte_carlo(m,t,prim_poly,ntrials);
}

