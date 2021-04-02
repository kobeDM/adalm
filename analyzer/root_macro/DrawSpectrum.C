void DrawSpectrum(const char* filename="Cs137_GAGG.root")
{

	TFile* file = new TFile(filename,"read");
	TTree* tree = (TTree*)file->Get("tree");
	double ph,area;
	tree->SetBranchAddress("ph_max_ch1",&ph);
	tree->SetBranchAddress("area_ch1",&area);

	int entries = tree->GetEntries();

	TH1D* h_ene = new TH1D("h_ene","h_ene",512,0,2500);
	TH2D* h_wf  = new TH2D("h_wf","h_wf",512,0,1024,512,-2500,2500);

	TGraph* g = new TGraph();

	for(int ev=0;ev<entries;ev++){
		tree->GetEntry(ev);
		g->SetPoint(ev,ph,area);
		h_ene->Fill(ph);
		//h_ene->Fill(area);
	}
	
	h_ene->Draw();
	//g->Draw("ap");


}
