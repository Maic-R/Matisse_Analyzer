#ifndef Cluster_h
#define Cluster_h

#include "AnalysisObject.h"

#include <unordered_map>

class Cluster : public AnalysisObject{
    
    public:

    Cluster();
    ~Cluster() override;

    void Begin(const CommandLine& cl) override;
    void Analyze(unsigned int iClz, unsigned int iSec) override;
    void End() override;

    private:

    const unsigned int m_MatrixDim = 5;
    unsigned int m_Pixel_thr;   // to identify a cluster

    struct calParam
    {
        double a;
        double b;
        unsigned int sector;

        calParam(double m, double q, unsigned int sec){
            a = q;
            b = m;
            sector = sec;
        }
    };

    std::unordered_map<unsigned int, calParam> calParameters;
    

    void Book() override;

    void CalibrateHisto(TH1F* hist, TH1F* hist2, unsigned int val, unsigned int sec);

    // single pixel cluster distribution and multiplicity considering all sectors
    TH1F* hSingle_pxl_cluster;
    TH1F* hMultiplicity;
    TH1F* hsum ;

    // corresponding canvases
    TCanvas* cSingle_pxl_cluster;
    TCanvas* cMultiplicity;
    TCanvas* csum;

    // histo for sectors
    TH1F* hMatrix_sector[m_Nsectors];
    TH1F* hCluster_sector[m_Nsectors];
    TH1F* hMultiplicity_sector[m_Nsectors];
    //TH1F* hRow_pxl_sector[m_Nsectors];
    //TH1F* hCol_pxl_sector[m_Nsectors];

    // canvases for sector
    TCanvas* cMatrix_sector;
    TCanvas* cCluster_sector;
    TCanvas* cMultiplicity_sector;
    //TCanvas* cRow_pxl_sector;
    //TCanvas* cCol_pxl_sector;

};

struct Clz{
    static unsigned int Count;
    static unsigned int Mult;
    static unsigned int Height;
    static unsigned int MatrixHeight;
    static const unsigned int MatrixSize = 25;
};

#endif