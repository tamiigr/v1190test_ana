/*
   Analyzer for v1190 test
   T.Sudo
   */

#include "TArtEventStore.hh"
#include "TArtRawEventObject.hh"
#include "TArtRawSegmentObject.hh"
#include "TArtRawDataObject.hh"

#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TGraph.h>
#include <TROOT.h>
#include <TSystem.h>
#include <TTree.h>
#include <TFile.h>
#include <TString.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TLegend.h>
#include <TStyle.h>

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <functional>

#include <stdio.h>

const int V1190CH = 128;
const int canvas_unit = 16;
int main(int argc, char *argv[])
{

	/* useage message */
	if (argc != 3)
	{
		printf("\n");
		printf("**************************************\n");
		printf("\n");
		printf("useage: ./offline ****.ridf ****.root\n");
		printf("\n");
		printf("**************************************\n");
		printf("\n");
		exit(1);
	}

	char *infilename;
	char *outfilename;
	infilename = argv[1];
	std::string input(argv[2]);

	/* check whether the input file exists */
	FILE *fp = fopen(infilename, "r");
	if (fp == NULL)
	{
		printf("\n");
		printf("**************************************\n");
		printf("\n");
		printf("ERROR!!  input file: %s does not exist.\n", infilename);
		printf("\n");
		printf("**************************************\n");
		printf("\n");
		exit(1);
	}
	fclose(fp);

	TFile *fout = new TFile((input + ".root").c_str(), "RECREATE");

	gSystem->Load("libanaroot.so");

	TArtEventStore *estore = new TArtEventStore();
	estore->Open(infilename);
	TArtRawEventObject *rawevent = estore->GetRawEventObject();
	/* Difine Histograms */
	char hid[256], hname[256];
	TH1F *rawTDC[V1190CH];	   // = new TH1F[V1190CH];
	TH1F *rawTDC_tra[V1190CH]; // = new TH1F[V1190CH];
	TLegend *lg[V1190CH];
	for (int i = 0; i < V1190CH; i++)
	{
		sprintf(hid, "h%d", 500 + i);
		sprintf(hname, "TDC ch%d raw Reading  spectrum, all hit", i);
		rawTDC[i] = new TH1F(hid, hname, 1500, 0, 1.5);
		rawTDC[i] -> SetStats(0);

		sprintf(hid, "h%d", 1000 + i);
		sprintf(hname, "TDC ch%d raw Trailing spectrum, all hit", i);
		rawTDC_tra[i] = new TH1F(hid, hname, 1500, 0, 1.5);
		rawTDC_tra[i] -> SetStats(0);

		lg[i] = new TLegend(0.50,0.48,0.85,0.88);
	}
	//Define Canvas
	//one canvas draw 16 histograms
	TCanvas *cx[8];
	for (int i = 0; i < 8; i++)
	{
		sprintf(hid, "TDC_unit_No%d", i + 1);
		cx[i] = new TCanvas(hid, hid, 1600, 1200);
		cx[i]->Divide(4, 4);
	}

	TH1F *TDC_hit_pattern_reading = new TH1F("Event ratio of reading", "TDC hit pattern", V1190CH, 0, V1190CH);
	TH1F *TDC_hit_pattern_traling = new TH1F("Event ratio of traling", "TDC hit pattern", V1190CH, 0, V1190CH);

	TDC_hit_pattern_reading->GetYaxis()->SetNdivisions(509, "");
	TDC_hit_pattern_reading->GetXaxis()->SetLabelFont(132);
	TDC_hit_pattern_reading->GetYaxis()->SetLabelFont(132);
	TDC_hit_pattern_reading->GetXaxis()->SetTitleFont(132);
	TDC_hit_pattern_reading->GetYaxis()->SetTitleFont(132);
	TDC_hit_pattern_reading->GetXaxis()->SetLabelOffset(0.01);
	TDC_hit_pattern_reading->GetYaxis()->SetLabelOffset(0.01);
	TDC_hit_pattern_reading->GetXaxis()->SetTitleOffset(1.2);
	TDC_hit_pattern_reading->GetYaxis()->SetTitleOffset(1.1);
	TDC_hit_pattern_reading->GetXaxis()->SetLabelSize(0.06);
	TDC_hit_pattern_reading->GetYaxis()->SetLabelSize(0.06);
	TDC_hit_pattern_reading->GetYaxis()->SetTitle("ith ch ev. / tot. rev.");
	TDC_hit_pattern_reading->GetXaxis()->SetTitle("TDC channel (ch)");
	TDC_hit_pattern_reading->GetXaxis()->SetTitleSize(0.05);
	TDC_hit_pattern_reading->GetYaxis()->SetTitleSize(0.05);
	TDC_hit_pattern_reading->GetXaxis()->CenterTitle(true);
	TDC_hit_pattern_reading->GetYaxis()->CenterTitle(true);

	TDC_hit_pattern_traling->GetYaxis()->SetNdivisions(509, "");
	TDC_hit_pattern_traling->GetXaxis()->SetLabelFont(132);
	TDC_hit_pattern_traling->GetYaxis()->SetLabelFont(132);
	TDC_hit_pattern_traling->GetXaxis()->SetTitleFont(132);
	TDC_hit_pattern_traling->GetYaxis()->SetTitleFont(132);
	TDC_hit_pattern_traling->GetXaxis()->SetLabelOffset(0.01);
	TDC_hit_pattern_traling->GetYaxis()->SetLabelOffset(0.01);
	TDC_hit_pattern_traling->GetXaxis()->SetTitleOffset(1.2);
	TDC_hit_pattern_traling->GetYaxis()->SetTitleOffset(1.1);
	TDC_hit_pattern_traling->GetXaxis()->SetLabelSize(0.06);
	TDC_hit_pattern_traling->GetYaxis()->SetLabelSize(0.06);
	TDC_hit_pattern_traling->GetYaxis()->SetTitle("ith ch ev. / tot. rev.");
	TDC_hit_pattern_traling->GetXaxis()->SetTitle("TDC channel (ch)");
	TDC_hit_pattern_traling->GetXaxis()->SetTitleSize(0.05);
	TDC_hit_pattern_traling->GetYaxis()->SetTitleSize(0.05);
	TDC_hit_pattern_traling->GetXaxis()->CenterTitle(true);
	TDC_hit_pattern_traling->GetYaxis()->CenterTitle(true);

	int neve = 0;
	int module;

	int ch, val;
	int geo, edge;

	int v1190_hit_reading[V1190CH];
	int v1190_hit_traling[V1190CH];
	int v1190_multi[V1190CH]; 
	int v1190_multi_tral[V1190CH]; 
	int tdc[V1190CH];
	int tdc_tra[V1190CH];
	for (int ich = 0; ich < V1190CH; ich++)
	{	
		v1190_multi[ich] = 0;  
		v1190_multi_tral[ich] = 0;  
	}
	int uint_flag;
	TTree *tree = new TTree("tree", "title");
	tree->Branch("tdc", tdc, "tdc[128]/I");
	tree->Branch("tdc_hit_leading", v1190_hit_reading, "tdc_hit_leading[128]/I");
	tree->Branch("tdc_hit_trailing", v1190_hit_traling, "tdc_hit_trailing[128]/I");
	tree->Branch("tdc_multi", v1190_multi, "tdc_multi[128]/I");

	while (estore->GetNextEvent())
	{

		/* Initialization */
		for (int ich = 0; ich < V1190CH; ich++)
		{
			//v1190_multi[ich] = 0;
			v1190_hit_reading[ich] = 0;
			v1190_hit_traling[ich] = 0;
			tdc[ich] = -1;
			tdc_tra[ich] = -1;
		}
		edge = -1;
		val=-1;
		/* Initialization to here */
		for (int i = 0; i < rawevent->GetNumSeg(); i++)
		{
			TArtRawSegmentObject *seg = rawevent->GetSegment(i);
			module = seg->GetModule();

			if (neve == 2)
				printf("module=%d\n", module);

			switch (module)
			{
				/* V1190 analysis */
				case 24:
					for (int j = 0; j < seg->GetNumData(); j++)
					{
						TArtRawDataObject *v1190 = seg->GetData(j);
						geo = v1190->GetGeo();
						edge = v1190->GetEdge();
						ch = v1190->GetCh();
						val = v1190->GetVal();

						//This is for Reading edge data
						if (edge == 0)
						{
							v1190_multi[ch]++;
							tdc[ch] = val;
							v1190_hit_reading[ch] = 1;
						}
						if (edge == 1)
						{
							tdc_tra[ch] = val;
							v1190_hit_traling[ch] = 1;
							v1190_multi_tral[ch]++;
						}

						if (i == 0)
							uint_flag = ch / canvas_unit;
					}
					break;
			} /** end of switch **/
		}	  //  end of rawevent->GetNumSeg()

		/* Fill in the Histograms */
		if (neve >= 0)
		{
			for (int ich = 0; ich < V1190CH; ich++)
			{
				if (v1190_hit_reading[ich] == 1)
				{
					rawTDC[ich]->Fill(tdc[ich] / 10000.);
					TDC_hit_pattern_reading->Fill(ich);
				}
				if (v1190_hit_traling[ich] == 1)
				{
					rawTDC_tra[ich]->Fill(tdc_tra[ich] / 10000.);
					TDC_hit_pattern_traling->Fill(ich);
				}
			}
			tree->Fill();
			estore->ClearData();
			if(val>0) neve++;
			if (neve % 1000 == 0)
			{
				printf("Event: %d\n", neve);
			}
		}
	}
	int index(0);
	for (int i = 0; i < V1190CH / canvas_unit; i++)
	{
		for (int j = 0; j < canvas_unit; j++)
		{
			cx[i]->cd(j + 1);
			rawTDC[index]->GetXaxis()->SetRangeUser(0.8, 1.5);
			rawTDC[index]->SetLineColor(1);
			rawTDC[index]->Draw();
			rawTDC_tra[index]->GetXaxis()->SetRangeUser(0.8, 1.5);
			rawTDC_tra[index]->SetLineColor(2);
			rawTDC_tra[index]->Draw("same");
			lg[index] -> AddEntry((TObject*)0,Form("Total ev. : %d" ,neve),"");
			lg[index] -> AddEntry((TObject*)0,Form("Lead ev. : %d" ,v1190_multi[index]),"");
			lg[index] -> AddEntry((TObject*)0,Form("Trail ev. : %d" ,v1190_multi_tral[index]),"");
			lg[index] -> Draw("same");
			index++;
		}
	}
	TDC_hit_pattern_reading->Scale(1. / neve);
	TDC_hit_pattern_reading->Write();

	TDC_hit_pattern_traling->Scale(1. / neve);
	TDC_hit_pattern_traling->Write();
	cx[uint_flag] -> Write();
	cx[uint_flag] -> SaveAs((input + ".pdf").c_str());
	tree->Write();
	fout->Close();
	printf("Analyzed %d events.\n", neve);
	printf("\n");
	return 0;
}
