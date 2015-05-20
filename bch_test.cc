#include <iostream>
#include <algorithm>
#include <vector>
#include <stdlib.h>
#include "bch_codec.h"
#include <string.h>
using namespace std;

namespace {
    vector<unsigned int> randperm(int n,int k) // like the matlab/octave function
    {
        vector<unsigned int> res;
        if (2*k > n) {
            res.resize(n);
            for (int i=0;i<n;++i) 
                res[i] = i;
            random_shuffle( res.begin(),res.end());
            res.resize(k);
        }else{
            res.reserve(k);
            for (int i=0;i<k;++i) {
                int next = rand() % n;
                while ( find(res.begin(),res.end(),next) != res.end() ) {
                    next = rand() % n;
                }
                res.push_back(next);
            }
        }
        return res;
    }
}

int monte_carlo(int m,int t,unsigned int prim_poly,int ntrials)
{

    bch_control * bch = init_bch(m,t,prim_poly);
    cout << "initialized" << " m=" << bch->m << " n=" << bch->n << " t=" << bch->t << " ecc_bits=" << bch->ecc_bits << " ecc_bytes=" << bch->ecc_bytes << endl;

    int N = (1<<m)-1;
    int msgBits = N - bch->ecc_bits;

    cout << "running " << ntrials << " trials of BCH decoding with up to " << t << " errors\n";
    vector<uint8_t> data( msgBits / 8 );
    vector<uint8_t> dataClean;
    cout << " data.size()=" << data.size() << endl;

    for (int trial=0;trial < ntrials;++trial) {
        // make a random message
        for (size_t k=0;k<data.size();++k)
            data[k] = rand()&0xFF;
        dataClean = data;

        // encode it
        vector<uint8_t> ecc( bch->ecc_bytes ,0);
        encode_bch(bch, &data[0],data.size(),&ecc[0]);

        // introduce up to t errors
        int nerrs = rand() % (t+1);
        vector<unsigned int> errLocIn = randperm( data.size()*8 ,nerrs );
        for (size_t k=0;k < errLocIn.size();++k) {
           int i = errLocIn[k];
           data[i>>3] ^= (1<<(i&7));
        }

        // decode and make sure the right errors were corrected
        vector<unsigned int> errLocOut(t);
        int nerrFound = decode_bch(bch, &data[0], data.size(),&ecc[0], NULL,NULL,&errLocOut[0]);
        if (nerrFound != nerrs ) {
            cerr << "decode_bch return value=" << nerrFound  << " expected " << nerrs <<endl;
            if (nerrFound<0)
                cerr << strerror(-nerrFound) << endl;
            return 1;
        }
        errLocOut.resize(nerrFound);

        sort( errLocIn.begin(),errLocIn.end());
        sort( errLocOut.begin(),errLocOut.end());
        if (errLocIn != errLocOut) {
            cerr << "Input Errors!= Found Errors !!!!" << endl;
            return 1;
        }

        correct_bch(bch,&data[0],data.size(),&errLocOut[0],nerrFound);

        if (dataClean != data) {
            cerr << "data not corrected\n";
            return 1;
        }
    }

    cout << "now testing bitwise functions\n";

    data.resize(msgBits);
    for (int trial=0;trial < ntrials;++trial) {
        for (size_t k=0;k<data.size();++k)
            data[k] = rand()&1;
        dataClean = data;

        vector<uint8_t> ecc(bch->ecc_bits);
        encodebits_bch(bch,&data[0],&ecc[0]);

        // introduce up to t errors
        int nerrs = rand() % (t+1);
        vector<unsigned int> errLocIn = randperm( msgBits ,nerrs );
        for (size_t k=0;k < errLocIn.size();++k) 
            data[errLocIn[k]] ^= 1;
        
        // decode and make sure the right errors were corrected
        vector<unsigned int> errLocOut(t);
        int nerrFound = decodebits_bch(bch, &data[0], &ecc[0], &errLocOut[0]);
        if (nerrFound != nerrs) {
            cerr << "decodebits_bch return value=" << nerrFound  << " expected " << nerrs <<endl;
            if (nerrFound<0)
                cerr << strerror(-nerrFound) << endl;
            return 1;
        }
        errLocOut.resize(nerrFound);
        sort( errLocIn.begin(),errLocIn.end());
        sort( errLocOut.begin(),errLocOut.end());
        if (errLocIn != errLocOut) {
            cerr << "Errors Location mismatch:" << endl;
            cerr << "input errors @ ";
            for (size_t k=0;k < errLocIn.size();++k) 
                cerr << errLocIn[k] << " ";
            cerr << "\nfound errors @ ";
            for (size_t k=0;k < errLocOut.size();++k) 
                cerr << errLocOut[k] << " ";
            cerr << "\n";

            //return 1;
        }   
        correctbits_bch(bch,&data[0],&errLocOut[0],nerrFound);
        if (dataClean != data) {
            cerr << "data not corrected\n";
            return 1;
        }
    }
    free_bch(bch);
    return 0;
}

int bit_testvectors()
{
    // from mktestvectors.m
    unsigned int prim_poly=37;
    const int N=31;
    const int K=21;
    const int ntrials=21;
    int m=5;
    int t=2;
    //char msgbits[ntrials][K+1] = {"100000000000000000000", "010000000000000000000", "001000000000000000000", "000100000000000000000", "000010000000000000000", "000001000000000000000", "000000100000000000000", "000000010000000000000", "000000001000000000000", "000000000100000000000", "000000000010000000000", "000000000001000000000", "000000000000100000000", "000000000000010000000", "000000000000001000000", "000000000000000100000", "000000000000000010000", "000000000000000001000", "000000000000000000100", "000000000000000000010", "000000000000000000001", };
    //char eccbits[ntrials][N-K+1] = {"1110110100", "0111011010", "0011101101", "1111000010", "0111100001", "1101000100", "0110100010", "0011010001", "1111011100", "0111101110", "0011110111", "1111001111", "1001010011", "1010011101", "1011111010", "0101111101", "1100001010", "0110000101", "1101110110", "0110111011", "1101101001", };
    char msgbits[ntrials][K+1] = {"100000000000000000000", "010000000000000000000", "001000000000000000000", "100100000000000000000", "010010000000000000000", "101001000000000000000", "110100100000000000000", "011010010000000000000", "101101001000000000000", "110110100100000000000", "111011010010000000000", "011101101001000000000", "001110110100100000000", "000111011010010000000", "000011101101001000000", "000001110110100100000", "000000111011010010000", "000000011101101001000", "000000001110110100100", "000000000111011010010", "000000000011101101001", };
    char eccbits[ntrials][N-K+1] = {"1110110100", "0111011010", "0011101101", "0001110110", "0000111011", "0000011101", "0000001110", "0000000111", "0000000011", "0000000001", "0000000000", "0000000000", "0000000000", "0000000000", "0000000000", "0000000000", "0000000000", "0000000000", "0000000000", "0000000000", "0000000000", };

    bch_control * bch = init_bch(m,t,prim_poly);
    int fails=0;
    for (int trial=0;trial<ntrials;++trial) {
        char ecc[N-K+1];
        memset(ecc,0,sizeof(ecc));
        encodebits_bch(bch,(const uint8_t*)msgbits[trial],(uint8_t*)ecc);

        for (int i=0;i<N-K;++i) 
            ecc[i] |= '0';
        cout << "message=" << msgbits[trial] << " ecc=" << ecc;
        if (strcmp(eccbits[trial],ecc)) {
           cout << " WANTED " << eccbits[trial];
           ++fails;
        }

        unsigned int errLoc[t];
        int nerr = decodebits_bch(bch, (const uint8_t*)msgbits[trial], (const uint8_t*)eccbits[trial],errLoc);
        if (nerr) {
            cout << " nerr=" << nerr;
        }

        cout << endl;
    }
    free_bch(bch);
    return fails;
}

int main(int argc, char ** argv)
{
    bit_testvectors();
    int m = 9;
    int t = 5;
    unsigned int p = 0; // zero indicates to use the appropriate element from prim_poly_tab
    int ntrials = 1000;
    if (argc>1) m = atoi(argv[1]);
    if (argc>2) t = atoi(argv[2]);
    if (argc>3) p = atoi(argv[3]);
    if (argc>4) ntrials = atoi(argv[4]);
    return monte_carlo(m,t,p,ntrials);
}
