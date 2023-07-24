#ifndef Seed_h
#define Seed_h

#include <TH2F.h>

#include "AnalysisObject.h"

#include <vector>
#include <utility>

class Seed : public AnalysisObject{

    public:

    Seed();
    ~Seed() override;

    void Begin(const CommandLine& cl) override;
    void Analyze(unsigned int iClz, unsigned int iSec) override;
    void End() override;

    // Added to spot noisy pixels.
    inline 
    const std::vector<std::pair<int, int>>& GetNoisyPixels() {return m_NoisyPixels;}

    private:

    void Book() override;

    void CalculateNoisyPixels();

    // histograms for the position of seeds within the matrix
    TH1F* hSeed_row;
    TH1F* hSeed_col;
    TH2F* hSeed_map;
    TH1F* hSeed_all;

    // corresponding canvases
    TCanvas* cSeed_row;
    TCanvas* cSeed_col;
    TCanvas* cSeed_map;
    TCanvas* cSeed_all;

    // histograms for sector
    TH1F* hSeed_sector[m_Nsectors];
    TH1F* hRow_sector[m_Nsectors];
    TH1F* hCol_sector[m_Nsectors];

    // canvases for sector
    TCanvas* cSeed_sector;
    TCanvas* cRow_sector;
    TCanvas* cCol_sector;

    // Yet added by Davide for noisy pixels.
    std::vector<std::pair<int, int>> m_NoisyPixels;
    unsigned int m_NoiseThreshold;

};

#endif