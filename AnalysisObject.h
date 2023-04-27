#ifndef AnalysisObject_h
#define AnalysisObject_h

#include <vector>
#include <unordered_map>

#include <TCanvas.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>

#include "CommandLine.h"

// general interface for all the analysis objects: seed, cluster and noise
class AnalysisObject{

    public:

    void SetPlotSettings(const std::string&); 

    virtual void Begin(const CommandLine& cl) = 0;
    virtual void Analyze(unsigned int iClz, unsigned int iSec) = 0;
    virtual void End() = 0;

    virtual ~AnalysisObject();

    inline const unsigned int GetNrows() const { return m_Nrows; }
    inline const unsigned int GetNcols() const { return m_Ncols; }
    inline const unsigned int GetNsectors() const { return m_Nsectors; }   

    inline
    const std::vector<TH1*>& GetListOfHistograms() const { return m_vHistos; }

    inline 
    const std::vector<TCanvas*>& GetListOfCanvas() const { return m_vCanvas; }

    inline
    const std::vector<TF1*>& GetListOfFunctions() const { return m_vFuncs; }

    inline
    bool AlreadyAnalyzed() const { return m_Already_analyzed; }

    protected:

    AnalysisObject();

    // calibration parameters of a linear fit
    struct CalibrationParam{

        CalibrationParam(double slope, double offset):
            a(slope), b(offset)
        { }

        double a;
        double b;
    };

    void Plot(TH1*, TCanvas*);
    void PlotForSector(TH1F**, TCanvas*);
    void Fit(TH1F* hist, TF1* func, float min, float max);
    void CalibrateInEnergy(TH1F* hist, CalibrationParam param);

    // Functions to save histograms, canvases and fit functions
    void Save(TH1F* hist);
    void Save(TH2F* hist);
    void Save(TCanvas* can);
    void Save(TF1* func);

    inline
    const std::unordered_map<TH1*, TCanvas*>& GetPlotPairs() const { return m_PlotPairs; }

    inline
    const std::unordered_map<TH1F**, TCanvas*>& GetPlotPairsSectors() const { return m_PlotPairsSectors; }

    // create canvas starting from the histogram name and title
    TCanvas* CreateCanvas(TCanvas* can, TH1* hist, int width = 800, int height = 800);
    TCanvas* CreateCanvasForSector(TCanvas* can, TH1F** hist, int width = 1060, int height = 860);

    // function to list all the histograms, canvases and functions for the analysis
    virtual void Book() = 0;

    static const unsigned int m_Nrows = 24;
    static const unsigned int m_Ncols = 24;
    static const unsigned int m_Nsectors = 4;

    // list of all histos, cans and funcs
    std::vector<TH1*> m_vHistos;
    std::vector<TCanvas*> m_vCanvas;
    std::vector<TF1*> m_vFuncs;

    std::unordered_map<TH1*,TCanvas*> m_PlotPairs;
    std::unordered_map<TH1F**, TCanvas*> m_PlotPairsSectors;

    // plot for sector and plot all sectors: plot settings
    bool m_PlotAll;
    bool m_PlotSec;
    bool m_Already_analyzed;

    private:
        
    static unsigned int m_Nhisto;
    static unsigned int m_Ncan;
    static unsigned int m_Nfunc;

};

#endif