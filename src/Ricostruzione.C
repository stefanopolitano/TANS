///////////////////////////////////////////////
// Macro to reconstruct events from the tree //
// Author A.Mecca (UNITO)                    // 
///////////////////////////////////////////////

#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"
#include "TCanvas.h"
#include "TH1F.h"
//#include "Read_dat_simulation.cxx"

#include "Trackelet.h"

using std::cout;

//Move to a separate file
typedef struct{
	Double_t x0,y0,z0;
	Int_t m;
}VERTICE;

Double_t mediaIntornoA(const std::vector<Double_t>& zRec, const Double_t& zModa, const Double_t& tolleranza);

void Ricostruzione(TString fileName = "vertFile0_noscat.root", TString treeName = "VT"){
	//Histograms and Canvas
	//TCanvas* cZetaV = new TCanvas("cZetaV","cZetaV",10,10,1200,1000);
	//TH1F* hZetaV = new TH1F("hZetaV","hZetaV",100,-50,50);
	
	TCanvas* ciccio = new TCanvas("ciccio","ciccio",10,10,1200,1000);
	TH1F* hciccio = new TH1F("hciccio","hciccio",20,-20,40);
	
	//TCanvas* cZetaL1 = new TCanvas("cZetaL1","cZetaL1",10,10,1200,1000);
	//TH1F* hZetaL1 = new TH1F("hZetaL1","hZetaL1",100,-50,50);
	
	//Open file and tree
	TFile* sourceFile = new TFile(fileName.Data());
	if(sourceFile->IsZombie()){
		cout<<"Error: \""<<fileName<<"\" not found\n";
		return;
	}
	else if(!(sourceFile->IsOpen())){
		cout<<"Error: could not open \""<<fileName<<"\"\n";
		return;
	}
	
	TTree* tree = (TTree*)(sourceFile->Get(treeName.Data()));
	if(!tree){
		cout<<"\n\""<<tree<<"\" not found in \""<<fileName<<"\"\n";
		return;
	}
	
	//Read constants from file
	//Read_dat_simulation("Data/Dati_Simulazione.dat");
	
	//Containers
	VERTICE vtx;
	TClonesArray* L1Hits = new TClonesArray("Hit", 400);
	TClonesArray* L2Hits = new TClonesArray("Hit", 400);
	
	TBranch* b_vtx = tree->GetBranch("Vertex");
	b_vtx->SetAddress(&vtx);
	TBranch* b_L1Hits = tree->GetBranch("L1_hit");
	b_L1Hits->SetAddress(&L1Hits);
	TBranch* b_L2Hits = tree->GetBranch("L2_hit");
	b_L2Hits->SetAddress(&L2Hits);
	/*
	tree->GetBranch("Vertex")->SetAddress(&vtx);
	tree->GetBranch("L1_hit")->SetAddress(&L1Hits);
	tree->GetBranch("L2_hit")->SetAddress(&L2Hits);
	tree->SetBranchStatus("*",1); //Activates reading of all branches
	*/
	//Main loop
	long nEntries = tree->GetEntries();
	cout<<"Processing "<<nEntries<<" entries: ";
	for(long k = 0; k < 100/*nEntries*/; k++){
		//if((100*k)%nEntries == 0) cout<<"\r\t\t\t\t"<< (100*k)/nEntries << " %";
		//if(tree->GetEntry(k) <= 0) continue;
		b_vtx->GetEntry(k);
		b_L1Hits->GetEntry(k);
		b_L2Hits->GetEntry(k);
		cout<<"\r\t\t\t\t"<<k+1;

		
		Trackelet* dummyTrackelet = new Trackelet(); //memory allocation
		TObject* thisObj1;
		TObject* thisObj2;
		Hit* thisHit1; 
		Hit* thisHit2;
		std::vector<Double_t> zRec;
		
		for(int i = 0; i < L1Hits->GetEntries(); i++){
			thisObj1 = (*L1Hits)[i];
			thisHit1 = (Hit*)thisObj1;
			
			for(int j = 0; j < L2Hits->GetEntries(); j++){
				thisObj2 = (*L2Hits)[i];
				thisHit2 = (Hit*)thisObj2;
				
				if(fabs(thisHit1->GetPHI() - thisHit2->GetPHI())>0.08) continue;
				new(dummyTrackelet) Trackelet(thisHit1, thisHit2); //Avoid alloc-dealloc overhead by reusing the same memory
				zRec.push_back(dummyTrackelet->findZVertex());
			}
			//hZetaL1->Fill(thisHit1->GetZ());
		}
		
		new(hciccio) TH1F("hciccio","hciccio",20,-20,40); // Dobbiamo trovare un modo efficiente di reinizializzare il grafico. Per il momento evitiamo solo eccessivi alloc-dealloc 
		for(size_t k = 0; k < zRec.size(); k++)
			hciccio->Fill(zRec.at(k));
			
		Double_t zModa = hciccio->GetMaximum();
		
		const Double_t tolleranza = 1; //cm
		Double_t zRecoMean = mediaIntornoA(zRec, zModa, tolleranza);
		
		
		delete dummyTrackelet;
		//delete thisObj1;
		//delete thisHit1;
		
		//hZetaV->Fill(vtx.z0,1);
		
		L1Hits->Clear("C");
		L1Hits->Clear("C");
	}
	cout<<'\n';
	//Draw
	//cZetaV->cd();
	//hZetaV->Draw();
	
	ciccio->cd();
	hciccio->Draw();
	
	//cZetaL1->cd();
	//hZetaL1->Draw();
	
	delete L1Hits;
	delete L2Hits;
	sourceFile->Close();
	return;
}

Double_t mediaIntornoA(const std::vector<Double_t>& zRec, const Double_t& zModa, const Double_t& tolleranza){
	Double_t temp = 0.;
	UInt_t zMediati = 0;
	for(size_t k = 0; k < zRec.size(); k++){
		if(fabs(zRec.at(k) - zModa) > tolleranza) continue;
		temp += zRec.at(k);
		zMediati++;
	}
	if(zMediati > 0)
		return temp/zMediati;
	else return -100.;
}