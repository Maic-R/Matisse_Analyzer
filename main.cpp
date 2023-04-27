#include <iostream>

#include "MatisseAnalyzer.h"

#include <TApplication.h>

int main(int argc, char** argv){

    MatisseAnalyzer analyzer( argc, argv );
    TApplication app("Matisse analyzer", &argc, argv);

    analyzer.beginAnalysis();

    analyzer.analyze();

    analyzer.endAnalysis();

    analyzer.runApplication(app);

    /*
    TCanvas *canvas = new TCanvas("fCanvas", "fCanvas", 600, 400);
    TH1D h ("h","h",10,0,10);
    h.Fill(1);
    h.Draw();
    canvas->Modified();
    canvas->Update();
    while (!gSystem->ProcessEvents()) {
       h.Fill(10 * gRandom->Rndm());
       canvas->Modified();
       canvas->Update();
       gSystem->Sleep(100);
    } 
    */
    return 0;
}