///////////////////////////////////////
// Macro to compile the TANS project //
// Author A.Mecca (UNITO)            // 
///////////////////////////////////////

#include <iostream>
#include <vector>

#include "TSystem.h"

using std::cout;
using std::string;
using std::vector;


void compile_simulation(bool run = false, bool debug = false){
		
	vector<string> macroPaths = {
		"src/Punto.cxx",
		"src/Hit.cxx",
		"src/Kinem_File.cxx",
		"src/Intersezione.cxx",
		"src/Trackelet.cxx",
		"src/Simulazione.C",
		"src/Ricostruzione.C"
	};
	
	gSystem->SetBuildDir("bin", kTRUE);
	gSystem->AddIncludePath("-I./interface");
	//cout<<"-------\n"<<gSystem->GetIncludePath()<<"\n-------\n";

	for(size_t i = 0; i < macroPaths.size(); i++){
		if(debug) cout<<"+++ Processing \""<<macroPaths.at(i)<<"\" \t+++\n";
		gSystem->CompileMacro(macroPaths.at(i).c_str(),"kg");
	}
	if(run)
		gROOT->ProcessLine("Simulazione()");
}

