#ifndef Noise_h
#define Noise_h

#include "AnalysisObject.h"

#include <TTree.h>
#include <TH2F.h>

#include <array>

// class for noise analysis
class Noise : public AnalysisObject{

    public:

    Noise();
    ~Noise();

    void Begin(const CommandLine& cl) override;
    void Analyze(unsigned int iClz, unsigned int iSec) override;
    void End() override;

    void PrintRAWData() const;

    struct RAWData{
        int nPixels;
        int nEntries;
        std::array<int, 50> Header;
        std::array<double, 600> Pedestal;
        std::array<double, 600> Noise;
    };

    private: 

    void Book() override;

    TTree* m_HeaderTree;

    // histograms for sectors
    TH1F* hNoise_sector[m_Nsectors];
    TH1F* hNoise_electron_sector[m_Nsectors];

    // canvases for sectors
    TCanvas* cNoise_sector;
    TCanvas* cNoise_electron_sector;

    // total histograms
    TH2F* hPedestal_map;
    TH2F* hNoise_map;
    TH1F* hPedestal;
    TH1F* hNoise_electron;

    // corresponding canvases
    TCanvas* cPedestal_map;
    TCanvas* cNoise_map;
    TCanvas* cPedestal;
    TCanvas* cNoise_electron;

    RAWData rawData;

};

#endif