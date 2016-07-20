//--------------------------------------------------------------------------
// Implementação da Classe CWaveFile
//--------------------------------------------------------------------------
#include "CWaveFile.h"
#include "matrix.h"
using namespace std;
using namespace math;
//--------------------------------------------------------------------------
CWaveFile::CWaveFile()
{
    Nbits = 0;
    Fs = 0;
    data.clear();
    FileName = "";
    nChannels = 0;
    isLoad = false;
}
//--------------------------------------------------------------------------
bool CWaveFile::LoadFile(string sFilePath)
{
    ifstream file(sFilePath.c_str(), ios::in|ios::binary|ios::ate);
    if (file.is_open())
    {
        ifstream::pos_type size;
        char * memblock;
        int i, nSamples, iSignal;
        unsigned short int tData01, tData02, tData03, tData04;
        long int posFile, lDataSize;
        unsigned char wParam;
        unsigned short int sampleValor;
        string strRiff;

        size = file.tellg();
        memblock = new char [size];
        file.seekg (0, ios::beg);
        file.read (memblock, size);
        file.close();

        for (i = 0; i < 4; i++)
            strRiff.push_back(memblock[i]);
        if (strRiff != "RIFF")
        {
            printf("Erro ao ler arquivo:\n%s.\nArquivo não possui a tag \"RIFF\".\n",sFilePath.c_str());
            return false;
        }
        strRiff.clear();
        for (i = 8; i < 12; i++)
            strRiff.push_back(memblock[i]);
        if (strRiff != "WAVE")
        {
            printf("Erro ao ler arquivo:\n%s.\nArquivo não possui a tag \"WAVE\".\n",sFilePath.c_str());
            return false;
        }

        strRiff.clear();
        for (i = 12; i < 16; i++)
            strRiff.push_back(memblock[i]);
        if (strRiff != "fmt ")
        {
            printf("Erro ao ler arquivo:\n%s.\nArquivo não possui a tag \"fmt \".\n",sFilePath.c_str());
            return false;
        }

        posFile = 16;
        wParam = memblock[posFile];
        if (wParam != 0x10)
        {
            printf("Erro ao ler arquivo:\n%s.\nArquivo não é do tipo PCM.\n",sFilePath.c_str());
            return false;
        }

        posFile = 22;
        nChannels = (unsigned int)memblock[posFile];
        if (nChannels > 1)
        {
            printf("Erro ao ler arquivo:\n%s.\nArquivo possui mais de um canal.\n",sFilePath.c_str());
            return false;
        }
        posFile = 24;
        tData01 = (unsigned int)memblock[posFile];
        posFile = 25;
        tData02 = (unsigned int)memblock[posFile];
        Fs = tData02*0x100 + tData01;

        posFile = 34;
        Nbits = (int)memblock[posFile];
        if (Nbits != 16)
        {
            printf("Erro ao ler arquivo:\n%s.\nArquivo possui resolução diferente de 16 bits.\n",sFilePath.c_str());
            return false;
        }

        posFile = 40;
        tData01 = (unsigned char)memblock[posFile];
        posFile++;
        tData02 = (unsigned char)memblock[posFile];
        posFile++;
        tData03 = (unsigned char)memblock[posFile];
        posFile++;
        tData04 = (unsigned char)memblock[posFile];
        posFile++;

        lDataSize = tData04*0x1000000 + tData03*0x10000 + tData02*0x100 + tData01;

        nSamples =  8*lDataSize/(Nbits*nChannels);
        double dataValue;
        for (i = 0; i < nSamples; i++)
        {
            wParam = memblock[posFile];
            posFile++;
            sampleValor = wParam;
            wParam = memblock[posFile];
            posFile++;

            sampleValor = sampleValor + wParam*0x100;

            iSignal = 1;
            if (sampleValor & 0x8000)
            {
                iSignal = -1;
                sampleValor = ~sampleValor + 0x0001;
            }
            sampleValor = sampleValor & 0x7FFF;
            dataValue = iSignal*(((float)sampleValor)/32767);
            data.push_back((double)dataValue);
        }
        FileName = sFilePath;
        isLoad = true;
        delete[] memblock;
        printf("Arquivo: %s Carregado com sucesso.\n",sFilePath.c_str());
        return true;
    }
    else
    {
        printf("Erro ao ler arquivo:\n%s\nArquivo não encontrado.\n",sFilePath.c_str());
        return false;
    }
};
//--------------------------------------------------------------------------
vector<double> CWaveFile::BuildTimeVector()
{
    vector<double> vTime;
    int i, nSamples;
    double t0;

    if ( this->isLoad == false)
        return vTime;

    nSamples = data.size();
    t0 = 1/(2*Fs);
    for(i = 0; i < nSamples; i++)
        vTime.push_back((t0 + (double)i/Fs));
    return vTime;
};
//--------------------------------------------------------------------------
vector<double> CWaveFile::GetDataVector(double tIni, double tFim)
{
    vector<double> tData;
    int i, nSamples, idxIni, idxFim;
    double t0;

    nSamples = data.size();
    if ((tFim < tIni) || (tFim > nSamples/Fs) || (isLoad == false) || ((tFim-tIni) < 1/Fs) )
        return tData;

    t0 = 1/(2*Fs);
    idxIni = (ceil((tIni - t0)*Fs) < 0) ? 0 : ceil((tIni - t0)*Fs);
    idxFim = floor((tFim - t0)*Fs);

    for(i = idxIni; i <= idxFim; i++)
        tData.push_back(data[i]);
    return tData;
};
//--------------------------------------------------------------------------
vector<double> CWaveFile::BuildTimeVector(double tIni, double tFim)
{
    vector<double> vTime;
    int i, nSamples, idxIni, idxFim;
    double t0;

    nSamples = data.size();
    if ((tFim < tIni) || (tFim > nSamples/Fs) || (isLoad == false) || ((tFim-tIni) < 1/Fs) )
        return vTime;

    t0 = 1/(2*Fs);
    idxIni = (ceil((tIni - t0)*Fs) < 0) ? 0 : ceil((tIni - t0)*Fs);
    idxFim = floor((tFim - t0)*Fs);

    for(i = idxIni; i <= idxFim; i++)
        vTime.push_back(t0 + (double)i/Fs);
    return vTime;
};
//--------------------------------------------------------------------------
vector<double> CWaveFile::GetDataVector(int idxIni, int idxFim)
{
    vector<double> tData;
    int i, nSamples;
    nSamples = data.size();
    if ((idxFim <= idxIni) || (idxFim > nSamples) || (isLoad == false))
        return tData;
    for(i = idxIni; i < idxFim; i++)
        tData.push_back(data[i]);
    return tData;
};
//--------------------------------------------------------------------------
vector<double> CWaveFile::BuildTimeVector(int idxIni, int idxFim)
{
    vector<double> vTime;
    int i, nSamples;
    double t0;

    nSamples = data.size();
    if ((idxFim <= idxIni) || (idxFim > nSamples) || (isLoad == false))
        return vTime;
    t0 = 1/(2*Fs);
    for(i = idxIni; i <= idxFim; i++)
        vTime.push_back(t0 + (double)i/Fs);
    return vTime;
};
//--------------------------------------------------------------------------
CWaveFile CWaveFile::GetWaveFile(int idxIni, int idxFim)
{
    CWaveFile mWaveFile;
    mWaveFile.FileName = this->FileName;
    mWaveFile.Fs = this->Fs;
    mWaveFile.Nbits = this->Nbits;
    mWaveFile.nChannels = this->nChannels;
    mWaveFile.isLoad = this->isLoad;
    mWaveFile.data = this->GetDataVector(idxIni, idxFim);
    return mWaveFile;
};
//--------------------------------------------------------------------------
CWaveFile CWaveFile::GetWaveFile(double tIni, double tFim)
{
    CWaveFile mWaveFile;
    mWaveFile.FileName = this->FileName;
    mWaveFile.Fs = this->Fs;
    mWaveFile.Nbits = this->Nbits;
    mWaveFile.nChannels = this->nChannels;
    mWaveFile.isLoad = this->isLoad;
    mWaveFile.data = this->GetDataVector(tIni, tFim);
    return mWaveFile;
};
//--------------------------------------------------------------------------

