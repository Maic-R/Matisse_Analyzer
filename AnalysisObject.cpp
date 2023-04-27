#include "AnalysisObject.h"
#include "Debug.h"

#include <string>

unsigned int AnalysisObject::m_Nhisto = 15;
unsigned int AnalysisObject::m_Ncan = 10;
unsigned int AnalysisObject::m_Nfunc = 3;

AnalysisObject::AnalysisObject(){

    m_vHistos.reserve(m_Nhisto);
    m_vCanvas.reserve(m_Ncan);
    m_vFuncs.reserve(m_Nfunc);

    // by default plot only total histograms 
    m_PlotSec = false;
    m_PlotAll = false;
    m_Already_analyzed = false;

    DBPRINT("AnalysisObject created: ", this);
}

AnalysisObject::~AnalysisObject(){
    m_vHistos.clear();
    m_vCanvas.clear();
    m_vFuncs.clear();
    m_PlotPairs.clear();
    m_PlotPairsSectors.clear();
}

void AnalysisObject::Plot(TH1* obj, TCanvas* can) {
    can->cd();
    obj->Draw("colz");
    return;
}

void AnalysisObject::PlotForSector(TH1F** obj, TCanvas* can) {
    can->Divide(2,2);
    for(unsigned int i=0; i < m_Nsectors; ++i){
        can->cd(i+1);
        obj[i]->Draw();    
    }
    return;
}

void AnalysisObject::Fit(TH1F* hist, TF1* func, float min, float max) {
    return;
}

void AnalysisObject::CalibrateInEnergy(TH1F* hist, CalibrationParam param){
    return;
}

// save 1D histo
void AnalysisObject::Save(TH1F* hist){
    // check
    if( hist == nullptr ) return;
    if( hist->InheritsFrom(TH1::Class()) )
        m_vHistos.emplace_back(hist);
    DBPRINT("Histogram saved: ", hist->GetName());
    return;
}

// save 2D histo
void AnalysisObject::Save(TH2F* hist){
    // check
    if( hist == nullptr ) return;
    if( hist->InheritsFrom(TH1::Class()) )
        m_vHistos.emplace_back(hist);
    DBPRINT("2D histogram saved: ", hist->GetName());
    return;
}

// save canvas
void AnalysisObject::Save(TCanvas* can){
    // check
    if( can == nullptr ) return;
    if( can->InheritsFrom(TPad::Class()) )
        m_vCanvas.emplace_back(can);
    DBPRINT("Canvas saved: ", can->GetName());
    return;
}

// save fit function
void AnalysisObject::Save(TF1* func){
    // check
    if( func == nullptr ) return;
    if( func->InheritsFrom(TObject::Class()) )
        m_vFuncs.emplace_back(func);
    DBPRINT("Function saved: ", func->GetName());
    return;
}

TCanvas* AnalysisObject::CreateCanvas(TCanvas* can, TH1* hist, int width, int height){

    // do not create total canvas if plot for sectors is required
    if(m_PlotSec) 
        return nullptr;

    if(!m_PlotSec || m_PlotAll){
        std::string histName = hist->GetName();
        // replace the first character with 'c' (canvas) instead of 'h' (histo)
        std::string canName = histName.replace(0,1,"c");

        std::string canTitle = hist->GetTitle();

        can = new TCanvas(canName.c_str(), canTitle.c_str(), width, height);

        m_PlotPairs.insert( std::make_pair(hist, can) );
    }

    return can;

}

TCanvas* AnalysisObject::CreateCanvasForSector(TCanvas* can, TH1F** hist, int width, int height){

    if(m_PlotSec || m_PlotAll){
        // take the first element of the array. It is equivalent to take also the second, third or fourth
        std::string histName = hist[0]->GetName();
        // replace the first character with 'c' instead of 'h'
        std::string canName = histName.replace(0,1,"c");

        // remove the _number from the histogram name
        for(auto it = canName.begin(); it != canName.end(); ++it){
            if(std::isdigit(*it)){
                canName.erase(it - 1, canName.end());
                break;
            }
        }

        std::string canTitle = hist[0]->GetTitle();
        // remove the # etc from the histo title
        size_t hashpos = canTitle.find_first_of('#');
        canTitle.erase(hashpos);

        can = new TCanvas(canName.c_str(), canTitle.c_str(), width, height);

        m_PlotPairsSectors.insert( std::make_pair(hist, can) );
    }

    // do not create canvas for sector if plot for sectors is not required
    if(!m_PlotSec) 
        return nullptr;

    return can;
}

void AnalysisObject::SetPlotSettings(const std::string& word){

    // settings must begin with "--"
    if( word.find("--") == std::string::npos ) return;

    bool sector = word.find("sector") != std::string::npos;
    bool all = word.find("all") != std::string::npos;

    if( sector )
        m_PlotSec = true;

    if( all )
        m_PlotAll = true;        

    DBPRINT("Plot setting for sector: ", m_PlotSec);
    DBPRINT("Plot setting for all sectors: ", m_PlotAll);

    return;
}
