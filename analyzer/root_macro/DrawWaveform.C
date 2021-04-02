void DrawWaveform(const char* filename="Cs137_GAGG.root")
{

	TFile* file = new TFile(filename,"read");
	TTree* tree = (TTree*)file->Get("tree");
	double sampling[1024];
	double wf[1024];
	tree->SetBranchAddress("sample_time",sampling);
	tree->SetBranchAddress("wf_ch1",wf);

	int entries = tree->GetEntries();

	TH2D* h_wf  = new TH2D("h_wf","h_wf",1024,0,1024/1e7*1e6,4092,-2500,2500);


	for(int ev=0;ev<entries;ev++){
		tree->GetEntry(ev);
		for(int samp=0;samp<1024;samp++){
			h_wf->Fill(sampling[samp],wf[samp]);
		}
		//h_ene->Fill(area);
	}
	
	h_wf->Draw("colz");
	//g->Draw("ap");


}
