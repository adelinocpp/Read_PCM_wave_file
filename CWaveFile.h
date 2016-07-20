#ifndef CWAVEFILE_H
#define CWAVEFILE_H

#include <fstream>
#include <string.h>
#include "AVFfunctions.h"

//--------------------------------------------------------------------------
//   Interface da classe CWaveFile
//   Abstração para carga de arquivo e manipulação na forma de um double
//--------------------------------------------------------------------------
using namespace std;
using namespace math;

typedef complex<double> Complexdouble;

class CWaveFile
{
private:
    int Nbits;
    int Fs;
    vector<double> data;
    string FileName;
    int nChannels;
    bool isLoad;
public:
    CWaveFile();
    ~CWaveFile() {};
    int GetSampleFrequency()
    {
        return Fs;
    };
    int GetNumPoints()
    {
        return (int)data.size();
    };
    int GetResolution()
    {
        return Nbits;
    };
    string GetFileName()
    {
        return FileName.c_str();
    };
    bool LoadFile(string sFilePath);
    vector<double> GetDataVector()
    {
        return data;
    };
    vector<double> BuildTimeVector();
    vector<double> GetDataVector(double tIni, double tFim);
    vector<double> BuildTimeVector(double tIni, double tFim);
    vector<double> GetDataVector(int idxIni, int idxFim);
    vector<double> BuildTimeVector(int idxIni, int idxFim);
    CWaveFile GetWaveFile(int idxIni, int idxFim);
    CWaveFile GetWaveFile(double tIni, double tFim);
};
//--------------------------------------------------------------------------
#endif // CWAVEFILE_H

