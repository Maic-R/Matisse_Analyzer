#include "Noise.h"
#include "Debug.h"

#include <TFile.h>

#include <iostream>

Noise::Noise():
AnalysisObject(),
m_HeaderTree(nullptr), hNoise_sector{nullptr}, hNoise_electron_sector{nullptr},
hPedestal_map(nullptr), hNoise_map(nullptr), hPedestal(nullptr), hNoise_electron(nullptr),
cNoise_sector(nullptr), cNoise_electron_sector(nullptr),
cPedestal_map(nullptr), cNoise_map(nullptr), cPedestal(nullptr), cNoise_electron(nullptr)
{

    rawData.nPixels = 0;
    rawData.nEntries = 0;  

    for(unsigned int i=0; i < rawData.Header.size(); ++i)
        rawData.Header[i] = 0;
    
    for(unsigned int i=0; i < rawData.Noise.size(); ++i){
        rawData.Noise[i] = 0;
        rawData.Pedestal[i] = 0;
    }

}

Noise::~Noise(){
    delete m_HeaderTree;
}

void Noise::PrintRAWData() const{
    std::cout << "----------------\n";
    std::cout << "RAW data values:\n";
    std::cout << "Number of pixels : " << rawData.nPixels << std::endl;
    std::cout << "Number of entries : " << rawData.nEntries << std::endl;

    std::cout << "Header content : \n";
    for(unsigned int i=0; i<rawData.Header.size(); ++i)
        std::cout << i << " " << rawData.Header.at(i) << "\n";
    
    std::cout << std::endl;
        
    std::cout << "Noise content : \n";
    for(unsigned int i=0; i<rawData.Noise.size(); ++i)
        std::cout << i << " " << rawData.Noise.at(i) << "\n";
    
    std::cout << std::endl;
        
    std::cout << "Pedestal content : \n";
    for(unsigned int i=0; i<rawData.Pedestal.size(); ++i)
        std::cout << i << " " << rawData.Pedestal.at(i) << "\n";
    
    std::cout << std::endl;
    std::cout << std::endl;
    return;
}

void Noise::Book(){

    // craetion of histograms for sector
    for(unsigned int i=0; i < m_Nsectors; ++i){

        Save( hNoise_sector[i] = new TH1F( ("hNoise_sector_" + std::to_string(i)).c_str(), 
            ("Noise distr sector #" + std::to_string(i)).c_str(), 100, 0, 50 ) );
        hNoise_sector[i]->GetXaxis()->SetTitle("Noise [ADC]");

        /*Save( hNoise_electron_sector[i] = new TH1F( ("hNoise_electron_sector_" + std::to_string(i)).c_str(), 
            ("Noise distr electrons sector #" + std::to_string(i)).c_str(), 100, 0, 300 ) );
        hNoise_electron_sector[i]->GetXaxis()->SetTitle("Noise [e^{-}]");*/
    }

    Save( hPedestal_map = new TH2F("hPedestal_map", "Pedestal map", m_Ncols, 0, m_Ncols, m_Nrows, 0, m_Nrows) );
    Save( hNoise_map = new TH2F("hNoise_map", "Noise map", m_Ncols, 0, m_Ncols, m_Nrows, 0, m_Nrows) );
    // Save( hPedestal = new TH1F("hPedestal", "Pedestal distribution", 100, 0, 10000) );
    // Save( hNoise_electron = new TH1F("hNoise_electron", "Noise distribution", 80, 0, 80) );

    Save( CreateCanvasForSector(cNoise_sector, hNoise_sector) );
    // Save( CreateCanvasForSector(cNoise_electron_sector, hNoise_electron_sector) );

    Save( CreateCanvas(cPedestal_map, hPedestal_map) );
    Save( CreateCanvas(cNoise_map, hNoise_map) );
    // Save( CreateCanvas(cPedestal, hPedestal) );
    // Save( CreateCanvas(cNoise_electron, hNoise_electron) );

    return;
}

void Noise::Begin(const CommandLine& cl){

    std::cout << "Starting noise analysis ... \n";
    std::cout << std::endl;
    Noise::Book();

    // get all the trees
    TFile* rootFile_RAW = new TFile( cl.getFileNameRAW().c_str(), "READ" );
    m_HeaderTree = dynamic_cast<TTree*>( rootFile_RAW->Get("raw_headertree") );

    m_HeaderTree->SetBranchAddress("NPixels", &rawData.nPixels);
    m_HeaderTree->SetBranchAddress("Header", &rawData.Header);
    m_HeaderTree->SetBranchAddress("Pedestal", &rawData.Pedestal);
    m_HeaderTree->SetBranchAddress("Noise", &rawData.Noise);
    rawData.nEntries = (int)m_HeaderTree->GetEntries();


    std::cout << "NPixels branch status = " << m_HeaderTree->GetBranchStatus("NPixels") << std::endl;
    std::cout << "Header branch status = " << m_HeaderTree->GetBranchStatus("Header") << std::endl;
    std::cout << "Pedestal branch status = " << m_HeaderTree->GetBranchStatus("Pedestal") << std::endl;
    std::cout << "Noise branch status = " << m_HeaderTree->GetBranchStatus("Noise") << std::endl;
    std::cout << std::endl;

    #if VERBOSE
        Noise::PrintRAWData();
    #endif

    return;
}

void Noise::Analyze(unsigned int iClz, unsigned int iSec){

    // for noise analysis we don't need to loop over all the clusters, so 
    // if the noise has already been analyzed, the function simply returns
    if(m_Already_analyzed) return;

    m_HeaderTree->GetEntry(0);

    // create pedestal map
    for(int j=0; j < rawData.nPixels; ++j){

        int row_address = j/24;
        int col_address = j%24;

        // sector
        int sector = col_address/6;
        // cut first row
        if( row_address == 1 ) continue;


        hPedestal_map->SetBinContent(col_address, row_address, rawData.Pedestal.at(j));
        hNoise_map->SetBinContent(col_address, row_address, rawData.Noise.at(j));

        // noise in adc and electrons
        hNoise_sector[sector]->Fill(rawData.Noise.at(j));

        // to calibrate histo of Noise
        //CalibrateHistoNoise(hNoise_sector[sector], rawData.Noise.at(j), iSec);

        // hNoise_electron_sector  PER RIEMPIRE QUESTO SERVE FARE IL FIT
        // hNoise_electron ANCHE QUESTO
    }

    m_Already_analyzed = true;

    return;
}

void Noise::End(){

    std::cout << "Noise analysis completed. Plotting the results ... \n";

    if(m_PlotAll){
        for(auto it = m_PlotPairs.begin(); it != m_PlotPairs.end(); ++it)
            Noise::Plot(it->first, it->second);
        for(auto it = m_PlotPairsSectors.begin(); it != m_PlotPairsSectors.end(); ++it)
            Noise::PlotForSector(it->first, it->second);
    }

    else if(m_PlotSec){
        for(auto it = m_PlotPairsSectors.begin(); it != m_PlotPairsSectors.end(); ++it)
            Noise::PlotForSector(it->first, it->second);
    }

    else{
        for(auto it = m_PlotPairs.begin(); it != m_PlotPairs.end(); ++it)
            Noise::Plot(it->first, it->second);
    }
    
    return;
}