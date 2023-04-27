#include "Seed.h"
#include "Debug.h"
#include "CommandLine.h"
#include "TreeData.h"

Seed::Seed():
    AnalysisObject(),
    hSeed_row(nullptr), hSeed_col(nullptr), hSeed_map(nullptr),
    cSeed_row(nullptr), cSeed_col(nullptr), cSeed_map(nullptr),
    cSeed_all(nullptr),
    hSeed_sector{nullptr}, hRow_sector{nullptr}, hCol_sector{nullptr},
    cSeed_sector(nullptr)
{
    DBPRINT("Seed object created: ", this);
}

Seed::~Seed(){
}

// list all the histograms, canvases and fit functions here
void Seed::Book() {

    // seed row distribution
    Save( hSeed_row = new TH1F("hSeed_row", "Seed row", m_Nrows, 0, m_Nrows) );
    hSeed_row->GetXaxis()->SetTitle("Row");
    Save( CreateCanvas(cSeed_row, hSeed_row) );

    // seed col distribution
    Save( hSeed_col = new TH1F("hSeed_col", "Seed col", m_Ncols, 0, m_Ncols) );
    hSeed_col->GetXaxis()->SetTitle("Column");
    Save( CreateCanvas(cSeed_col, hSeed_col) );
  
    // seed map
    Save( hSeed_map = new TH2F("hSeed_map", "Seed map", m_Nrows, 0, m_Nrows, m_Ncols, 0, m_Ncols) );
    hSeed_map->GetXaxis()->SetTitle("Column");
    hSeed_map->GetYaxis()->SetTitle("Row");
    Save( CreateCanvas(cSeed_map, hSeed_map) );


    // creation of all the histograms for sector
    for(unsigned int i=0; i < m_Nsectors; ++i){

        Save( hSeed_sector[i] = new TH1F ( ("hSeed_sct_" + std::to_string(i)).c_str(), 
            ("Seed sector # " + std::to_string(i)).c_str(), 100, 0, 800) );
        hSeed_sector[i]->GetXaxis()->SetTitle("Signal [ADC]");

        Save( hRow_sector[i] = new TH1F ( ("hRow_sct_" + std::to_string(i)).c_str(),
            ("Row sector # " + std::to_string(i)).c_str(), m_Nrows, 0, m_Nrows) );
        hRow_sector[i]->GetXaxis()->SetTitle("Row address");

        Save( hCol_sector[i] = new TH1F ( ("hCol_sct_" + std::to_string(i)).c_str(),
            ("Column sector # " + std::to_string(i)).c_str(), m_Ncols, 0, m_Ncols) );
        hCol_sector[i]->GetXaxis()->SetTitle("Column address");
    }

    // creation of canvas for sectors
    Save( CreateCanvasForSector(cSeed_sector, hSeed_sector) ); 

    Save( hSeed_all = new TH1F("hSeed_all", "Seed distribution", 100, 0, 700) );
    hSeed_all->GetXaxis()->SetTitle("Signal [ADC]");
    Save( CreateCanvas(cSeed_all, hSeed_all) );
    
    return;
}

void Seed::Begin(const CommandLine& cl){

    std::cout << "Starting seed analysis ... \n";
    std::cout << std::endl;
    Seed::Book();

    return;
}

void Seed::Analyze(unsigned int iClz, unsigned int iSec){

    TreeData<int>* dataInt = TreeData<int>::GetInstance();
    TreeData<float>* dataFloat = TreeData<float>::GetInstance();

    // rows distribution
    int row_seed = dataInt->Get("row_seed")->at(iClz);
    hSeed_row->Fill(row_seed);

    // columns distribution
    int col_seed = dataInt->Get("col_seed")->at(iClz);
    hSeed_col->Fill(col_seed);

    // 2D distribution
    hSeed_map->Fill(col_seed, row_seed);
    
    // seed pulse height
    float seed_height = dataFloat->Get("clusterh")->at(iClz);
    hSeed_sector[iSec]->Fill(seed_height);

    hRow_sector[iSec]->Fill(row_seed);
    hCol_sector[iSec]->Fill(col_seed);

    // aggregated histo
    hSeed_all->Fill(seed_height);

    return;
}

void Seed::End(){

    std::cout << "Seed analysis completed. Plotting the results ... \n";

    m_Already_analyzed = true;

    if(m_PlotAll){
        for(auto it = m_PlotPairs.begin(); it != m_PlotPairs.end(); ++it)
            Seed::Plot(it->first, it->second);
        for(auto it = m_PlotPairsSectors.begin(); it != m_PlotPairsSectors.end(); ++it)
            Seed::PlotForSector(it->first, it->second);
    }

    else if(m_PlotSec){
        for(auto it = m_PlotPairsSectors.begin(); it != m_PlotPairsSectors.end(); ++it)
            Seed::PlotForSector(it->first, it->second);
    }

    else{
        for(auto it = m_PlotPairs.begin(); it != m_PlotPairs.end(); ++it)
            Seed::Plot(it->first, it->second);
    }

    return;
}