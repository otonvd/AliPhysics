// **************************************************************************************
// *                                                                                    *
// * Task for Jet properties and jet shape analysis in PWG4 Jet Task Force Train for pp *
// *                                                                                    *
// **************************************************************************************


/**************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id: */

#include "TList.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TString.h"
#include "THnSparse.h"
#include "TProfile.h"
#include "TFile.h"
#include "TKey.h"
#include "TRandom3.h"

#include "AliAODInputHandler.h" 
#include "AliAODHandler.h" 
#include "AliESDEvent.h"
#include "AliAODMCParticle.h"
#include "AliAODJet.h"
#include "AliGenPythiaEventHeader.h"
#include "AliGenHijingEventHeader.h"
#include "AliInputEventHandler.h"

#include "AliAnalysisHelperJetTasks.h"
#include "AliAnalysisManager.h"
#include "AliAnalysisTaskSE.h"
#include "AliVParticle.h"
#include "AliAODTrack.h"
#include "AliVEvent.h"


#include "AliAnalysisTaskJetProperties.h"

ClassImp(AliAnalysisTaskJetProperties)
  
//_________________________________________________________________________________//

  AliAnalysisTaskJetProperties::AliAnalysisTaskJetProperties()
   : AliAnalysisTaskSE()
   ,fESD(0)
   ,fAOD(0)
   ,fAODJets(0)  
   ,fAODExtension(0)
   ,fNonStdFile("")
   ,fBranchJets("jets")
   ,fTrackType(kTrackAOD)
   ,fJetRejectType(0)
   ,fUseAODInputJets(kTRUE)
   ,fFilterMask(0)
   ,fUsePhysicsSelection(kTRUE)
   ,fMaxVertexZ(10)
   ,fNContributors(2)
   ,fTrackPtCut(0)
   ,fTrackEtaMin(0)
   ,fTrackEtaMax(0)
   ,fJetPtCut(0)
   ,fJetEtaMin(0)
   ,fJetEtaMax(0)
   ,fAvgTrials(0)
   ,fJetList(0)
   ,fTrackList(0)
   ,fCommonHistList(0)
   ,fh1EvtSelection(0)
   ,fh1VertexNContributors(0)
   ,fh1VertexZ(0)
   ,fh1Xsec(0)
   ,fh1Trials(0)
   ,fh1PtHard(0)
   ,fh1PtHardTrials(0)
   ,fh2EtaJet(0)
   ,fh2PhiJet(0)
   ,fh2PtJet(0)
   ,fh1PtJet(0)
   ,fh2NtracksJet(0)
   ,fProNtracksJet(0)
   ,fh2EtaTrack(0)
   ,fh2PhiTrack(0)
   ,fh2PtTrack(0)
   ,fh2FF(0)
   ,fh2DelEta(0)
   ,fh2DelPhi(0)
   ,fh2DelR(0)
   ,fh1PtLeadingJet(0)
   ,fh2NtracksLeadingJet(0)
   ,fProNtracksLeadingJet(0)
   ,fh2DelR80pcNch(0)
   ,fProDelR80pcNch(0)
   ,fh2DelR80pcPt(0)
   ,fProDelR80pcPt(0)
   ,fh2AreaCh(0)
   ,fProAreaCh(0)
   ,fh3PtDelRNchSum(0)
   ,fh3PtDelRPtSum(0)
   ,fProDiffJetShape(0)
   ,fProIntJetShape(0)
{
  for(Int_t ii=0; ii<5; ii++){
    fProDelRNchSum[ii]    = NULL;  
    fProDelRPtSum[ii]     = NULL;
    fProDiffJetShapeA[ii] = NULL;
    fProIntJetShapeA[ii]  = NULL;
  }//ii loop
  // default constructor
}
//_________________________________________________________________________________//

AliAnalysisTaskJetProperties::AliAnalysisTaskJetProperties(const char *name) 
  : AliAnalysisTaskSE(name)
  ,fESD(0)
  ,fAOD(0)
  ,fAODJets(0)  
  ,fAODExtension(0)
  ,fNonStdFile("")
  ,fBranchJets("jets")
  ,fTrackType(kTrackAOD)
  ,fJetRejectType(0)
  ,fUseAODInputJets(kTRUE)
  ,fFilterMask(0)
  ,fUsePhysicsSelection(kTRUE)
  ,fMaxVertexZ(10)
  ,fNContributors(2)
  ,fTrackPtCut(0)
  ,fTrackEtaMin(0)
  ,fTrackEtaMax(0)
  ,fJetPtCut(0)
  ,fJetEtaMin(0)
  ,fJetEtaMax(0)
  ,fAvgTrials(0)
  ,fJetList(0)
  ,fTrackList(0)
  ,fCommonHistList(0)
  ,fh1EvtSelection(0)
  ,fh1VertexNContributors(0)
  ,fh1VertexZ(0)
  ,fh1Xsec(0)
  ,fh1Trials(0)
  ,fh1PtHard(0)
  ,fh1PtHardTrials(0)
  ,fh2EtaJet(0)
  ,fh2PhiJet(0)
  ,fh2PtJet(0)
  ,fh1PtJet(0)
  ,fh2NtracksJet(0)
  ,fProNtracksJet(0)
  ,fh2EtaTrack(0)
  ,fh2PhiTrack(0)
  ,fh2PtTrack(0)
  ,fh2FF(0)
  ,fh2DelEta(0)
  ,fh2DelPhi(0)
  ,fh2DelR(0)
  ,fh1PtLeadingJet(0)
  ,fh2NtracksLeadingJet(0)
  ,fProNtracksLeadingJet(0)
  ,fh2DelR80pcNch(0)
  ,fProDelR80pcNch(0)
  ,fh2DelR80pcPt(0)
  ,fProDelR80pcPt(0)
  ,fh2AreaCh(0)
  ,fProAreaCh(0)
  ,fh3PtDelRNchSum(0)
  ,fh3PtDelRPtSum(0)
  ,fProDiffJetShape(0)
  ,fProIntJetShape(0)
{
  for(Int_t ii=0; ii<5; ii++){
    fProDelRNchSum[ii]    = NULL;  
    fProDelRPtSum[ii]     = NULL;  
    fProDiffJetShapeA[ii] = NULL;
    fProIntJetShapeA[ii]  = NULL;
  }//ii loop
  // constructor
  DefineOutput(1,TList::Class());
}
//_________________________________________________________________________________//

AliAnalysisTaskJetProperties::~AliAnalysisTaskJetProperties()
{
  // destructor
  if(fJetList)   delete fJetList;
  if(fTrackList) delete fTrackList;
}
//_________________________________________________________________________________//

Bool_t AliAnalysisTaskJetProperties::Notify()
{
  //
  // Implemented Notify() to read the cross sections
  // and number of trials from pyxsec.root
  // (taken from AliAnalysisTaskJetSpectrum2)
  // 
  if(fDebug > 1) Printf("AliAnalysisTaskJetProperties::Notify()");

  TTree *tree = AliAnalysisManager::GetAnalysisManager()->GetTree();
  Float_t xsection = 0;
  Float_t ftrials  = 1;
  
  fAvgTrials = 1;
  if(tree){
    TFile *curfile = tree->GetCurrentFile();
    if (!curfile) {
      Error("Notify","No current file");
      return kFALSE;
    }
    if(!fh1Xsec||!fh1Trials){
      Printf("%s%d No Histogram fh1Xsec",(char*)__FILE__,__LINE__);
      return kFALSE;
    }
    AliAnalysisHelperJetTasks::PythiaInfoFromFile(curfile->GetName(),xsection,ftrials);
    fh1Xsec->Fill("<#sigma>",xsection);
    // construct a poor man average trials 
    Float_t nEntries = (Float_t)tree->GetTree()->GetEntries();
    if(ftrials>=nEntries && nEntries>0.)fAvgTrials = ftrials/nEntries;
  }
  return kTRUE;
}
//_________________________________________________________________________________//

void AliAnalysisTaskJetProperties::UserCreateOutputObjects()
{
  //(Here, event selection part is taken from AliAnalysisTaskFramentationFunction)
  // create output objects
  if(fDebug > 1) Printf("AliAnalysisTaskJetProperties::UserCreateOutputObjects()");

  // create list of tracks and jets   
  fJetList = new TList();
  fJetList->SetOwner(kFALSE);
  fTrackList = new TList();
  fTrackList->SetOwner(kFALSE);
  
  // Create histograms / output container
  OpenFile(1);
  fCommonHistList = new TList();
  fCommonHistList->SetOwner();
  
  Bool_t oldStatus = TH1::AddDirectoryStatus();
  TH1::AddDirectory(kFALSE);
  // Histograms/TProfiles	
  fh1EvtSelection            = new TH1F("fh1EvtSelection", "Event Selection", 6, -0.5, 5.5);
  fh1EvtSelection->GetXaxis()->SetBinLabel(1,"ACCEPTED");
  fh1EvtSelection->GetXaxis()->SetBinLabel(2,"event selection: rejected");
  fh1EvtSelection->GetXaxis()->SetBinLabel(3,"event class: rejected");
  fh1EvtSelection->GetXaxis()->SetBinLabel(4,"vertex Ncontr: rejected");
  fh1EvtSelection->GetXaxis()->SetBinLabel(5,"vertex z: rejected");
  fh1EvtSelection->GetXaxis()->SetBinLabel(6,"vertex type: rejected");

  
  fh1VertexNContributors     = new TH1F("fh1VertexNContributors", "Vertex N contributors", 2500,-.5, 2499.5);
  fh1VertexZ                 = new TH1F("fh1VertexZ", "Vertex z distribution", 30, -15., 15.);
  fh1Xsec                    = new TProfile("fh1Xsec","xsec from pyxsec.root",1,0,1);
  fh1Xsec->GetXaxis()->SetBinLabel(1,"<#sigma>");
  fh1Trials                  = new TH1F("fh1Trials","trials from pyxsec.root",1,0,1);
  fh1Trials->GetXaxis()->SetBinLabel(1,"#sum{ntrials}");
  fh1PtHard                  = new TH1F("fh1PtHard","PYTHIA Pt hard;p_{T,hard}",350,-.5,349.5);
  fh1PtHardTrials            = new TH1F("fh1PtHardTrials","PYTHIA Pt hard weight with trials;p_{T,hard}",350,-.5,349.5);
  

  Int_t kNbinsPtSlice=20; Float_t xMinPtSlice=0.0; Float_t xMaxPtSlice=100.0;
  Int_t kNbinsEta=40;     Float_t xMinEta=-2.0;    Float_t xMaxEta=2.0;
  Int_t kNbinsPhi=90;     Float_t xMinPhi=0.0;     Float_t xMaxPhi=TMath::TwoPi();
  Int_t kNbinsPt=125;     Float_t xMinPt=0.0;      Float_t xMaxPt=250.0;
  Int_t kNbinsNtracks=50; Float_t xMinNtracks=0.0; Float_t xMaxNtracks=50.0;
  Int_t kNbinsFF=80;      Float_t xMinFF=0.0;      Float_t xMaxFF=2.0;
  Int_t kNbinsDelR1D=50;  Float_t xMinDelR1D=0.0;  Float_t xMaxDelR1D=1.0;
  
  
  fh2EtaJet      = new TH2F("EtaJet","EtaJet", 
			    kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
			    kNbinsEta,     xMinEta,     xMaxEta);
  fh2PhiJet      = new TH2F("PhiJet","PhiJet",
			    kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
			    kNbinsPhi,     xMinPhi,     xMaxPhi);
  fh2PtJet       = new TH2F("PtJet","PtJet",
			    kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
			    kNbinsPt,      xMinPt,      xMaxPt);
  fh1PtJet       = new TH1F("PtJet1D","PtJet1D",
			    kNbinsPt,      xMinPt,      xMaxPt);
  fh2NtracksJet  = new TH2F("NtracksJet","NtracksJet",
			    kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
			    kNbinsNtracks, xMinNtracks, xMaxNtracks);
  fProNtracksJet = new TProfile("AvgNoOfTracksJet","AvgNoOfTracksJet",
				kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
				xMinNtracks, xMaxNtracks);
  fh2EtaTrack    = new TH2F("EtaTrack","EtaTrack", 
			    kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
			    kNbinsEta,     xMinEta,     xMaxEta);
  fh2PhiTrack    = new TH2F("PhiTrack","PhiTrack",
			    kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
			    kNbinsPhi,     xMinPhi,     xMaxPhi);
  fh2PtTrack     = new TH2F("PtTrack","PtTrack",
			    kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
			    kNbinsPt,      xMinPt,      xMaxPt);
  fh2FF          = new TH2F("FF","FF",
			    kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
			    kNbinsFF,      xMinFF,      xMaxFF);
  fh2DelEta      = new TH2F("DelEta","DelEta", 
			    kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
			    kNbinsEta,     xMinEta,     xMaxEta);
  fh2DelPhi      = new TH2F("DelPhi","DelPhi",
			    kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
			    kNbinsPhi,     xMinPhi,     xMaxPhi);
  fh2DelR        = new TH2F("DelR","DelR",
			    kNbinsPtSlice, xMinPtSlice, xMaxPtSlice,
			    kNbinsDelR1D,  xMinDelR1D,  xMaxDelR1D);
  

  
  Int_t kNbinsDelR=100;      Float_t xMinDelR=0.0;      Float_t xMaxDelR=1.0;
  Int_t kNbinsPtSliceJS=100; Float_t xMinPtSliceJS=0.0; Float_t xMaxPtSliceJS=250.0;
  
  fh1PtLeadingJet       = new TH1F("PtLeadingJet","PtLeadingJet",
				   kNbinsPt,      xMinPt,      xMaxPt);
  fh2NtracksLeadingJet  = new TH2F("NtracksLeadingJet","NtracksLeadingJet",
				   kNbinsPtSliceJS, xMinPtSliceJS, xMaxPtSliceJS,
				   kNbinsNtracks,   xMinNtracks,   xMaxNtracks);
  fProNtracksLeadingJet = new TProfile("AvgNoOfTracksLeadingJet","AvgNoOfTracksLeadingJet",
				       kNbinsPtSliceJS, xMinPtSliceJS, xMaxPtSliceJS,
				       xMinNtracks, xMaxNtracks);
  fh2DelR80pcNch        = new TH2F("delR80pcNch","delR80pcNch",
				   kNbinsPtSliceJS, xMinPtSliceJS, xMaxPtSliceJS,
				   kNbinsDelR,      xMinDelR,      xMaxDelR);
  fProDelR80pcNch       = new TProfile("AvgdelR80pcNch","AvgdelR80pcNch",
				       kNbinsPtSliceJS, xMinPtSliceJS, xMaxPtSliceJS,
				       xMinDelR,  xMaxDelR);
  fh2DelR80pcPt         = new TH2F("delR80pcPt","delR80pcPt",
				   kNbinsPtSliceJS, xMinPtSliceJS, xMaxPtSliceJS,
				   kNbinsDelR,      xMinDelR,      xMaxDelR);
  fProDelR80pcPt        = new TProfile("AvgdelR80pcPt","AvgdelR80pcPt",
				       kNbinsPtSliceJS, xMinPtSliceJS, xMaxPtSliceJS,
				       xMinDelR,  xMaxDelR);
  fh2AreaCh             = new TH2F("Area","Area",
				   kNbinsPtSliceJS, xMinPtSliceJS, xMaxPtSliceJS,
				   72, 0.0, TMath::Pi());
  fProAreaCh            = new TProfile("AvgArea","AvgArea",
				   kNbinsPtSliceJS, xMinPtSliceJS, xMaxPtSliceJS,
				   0.0, TMath::Pi());
  fh3PtDelRNchSum       = new TH3F("PtDelRNchSum","PtDelRNchSum",
				   kNbinsPtSliceJS, xMinPtSliceJS, xMaxPtSliceJS,
				   kNbinsDelR1D,    xMinDelR1D,    xMaxDelR1D,
				   kNbinsNtracks, xMinNtracks, xMaxNtracks);
  fh3PtDelRPtSum        = new TH3F("PtDelRPtSum","PtDelRPtSum",
				   kNbinsPtSliceJS, xMinPtSliceJS, xMaxPtSliceJS,
				   kNbinsDelR1D,    xMinDelR1D,    xMaxDelR1D,
				   kNbinsPt,        xMinPt,        xMaxPt);
  fProDiffJetShape      = new TProfile("DiffJetShape","DiffJetShape",
				  10,0.0,1.0,0.0,250.0);
  fProIntJetShape       = new TProfile("IntJetShape","IntJetShape",
				  10,0.0,1.0,0.0,250.0);

  TString title;
  for(Int_t ii=0; ii<5; ii++){
    if(ii==0)title = "_JetPt20to30";
    if(ii==1)title = "_JetPt30to40";
    if(ii==2)title = "_JetPt40to60";
    if(ii==3)title = "_JetPt60to80";
    if(ii==4)title = "_JetPt80to100";
    fProDelRNchSum[ii] = new TProfile(Form("AvgNchSumDelR%s",title.Data()),Form("AvgNchSumDelR%s",title.Data()),
				      kNbinsDelR1D,  xMinDelR1D,  xMaxDelR1D,
				      xMinNtracks, xMaxNtracks);
    
    fProDelRPtSum[ii]  = new TProfile(Form("AvgPtSumDelR%s",title.Data()),Form("AvgPtSumDelR%s",title.Data()),
				      kNbinsDelR1D,  xMinDelR1D,  xMaxDelR1D,
				      xMinPt,      xMaxPt);
    fProDelRNchSum[ii] ->GetXaxis()->SetTitle("R");  
    fProDelRNchSum[ii] ->GetYaxis()->SetTitle("<NchSum>");
    fProDelRPtSum[ii]  ->GetXaxis()->SetTitle("R");  
    fProDelRPtSum[ii]  ->GetYaxis()->SetTitle("<PtSum>");

    fProDiffJetShapeA[ii] = new TProfile(Form("DiffJetShape%s",title.Data()),Form("DiffJetShape%s",title.Data()),
					 10,0.0,1.0,0.0,250.0);
    fProIntJetShapeA[ii]  = new TProfile(Form("IntJetShape%s",title.Data()),Form("IntJetShape%s",title.Data()),
					 10,0.0,1.0,0.0,250.0);

    fProDiffJetShapeA[ii]->GetXaxis()->SetTitle("R"); 
    fProDiffJetShapeA[ii]->GetYaxis()->SetTitle("Diff jet shape"); 
    fProIntJetShapeA[ii]->GetXaxis()->SetTitle("R");  
    fProIntJetShapeA[ii]->GetYaxis()->SetTitle("Integrated jet shape");  
    
    fCommonHistList->Add(fProDelRNchSum[ii]);
    fCommonHistList->Add(fProDelRPtSum[ii]);
    fCommonHistList->Add(fProDiffJetShapeA[ii]);
    fCommonHistList->Add(fProIntJetShapeA[ii]);
  
}//ii loop
  
  fh2EtaJet     ->GetXaxis()->SetTitle("JetPt");  fh2EtaJet     ->GetYaxis()->SetTitle("JetEta");
  fh2PhiJet     ->GetXaxis()->SetTitle("JetPt");  fh2PhiJet     ->GetYaxis()->SetTitle("JetPhi");
  fh2PtJet      ->GetXaxis()->SetTitle("JetPt");  fh2PtJet      ->GetYaxis()->SetTitle("JetPt");
  fh1PtJet      ->GetXaxis()->SetTitle("JetPt");  fh1PtJet      ->GetYaxis()->SetTitle("#jets");
  fh2NtracksJet ->GetXaxis()->SetTitle("JetPt");  fh2NtracksJet ->GetYaxis()->SetTitle("#tracks");
  fProNtracksJet->GetXaxis()->SetTitle("JetPt");  fProNtracksJet->GetYaxis()->SetTitle("AgvNtracks");
  fh2EtaTrack   ->GetXaxis()->SetTitle("JetPt");  fh2EtaTrack   ->GetYaxis()->SetTitle("TrackEta");
  fh2PhiTrack   ->GetXaxis()->SetTitle("JetPt");  fh2PhiTrack   ->GetYaxis()->SetTitle("TrackPhi");
  fh2PtTrack    ->GetXaxis()->SetTitle("JetPt");  fh2PtTrack    ->GetYaxis()->SetTitle("TrackPt");
  fh2FF         ->GetXaxis()->SetTitle("JetPt");  fh2FF         ->GetYaxis()->SetTitle("FF");
  fh2DelEta     ->GetXaxis()->SetTitle("JetPt");  fh2DelEta     ->GetYaxis()->SetTitle("DelEta");
  fh2DelPhi     ->GetXaxis()->SetTitle("JetPt");  fh2DelPhi     ->GetYaxis()->SetTitle("DelPhi");
  fh2DelR       ->GetXaxis()->SetTitle("JetPt");  fh2DelR       ->GetYaxis()->SetTitle("DelR");

  fh1PtLeadingJet       ->GetXaxis()->SetTitle("JetPt");  fh1PtLeadingJet       ->GetYaxis()->SetTitle("#leading jets");
  fh2NtracksLeadingJet  ->GetXaxis()->SetTitle("JetPt");  fh2NtracksLeadingJet  ->GetYaxis()->SetTitle("#tracks leading jet");
  fProNtracksLeadingJet ->GetXaxis()->SetTitle("JetPt");  fProNtracksLeadingJet ->GetYaxis()->SetTitle("AvgNtracks leading jet");
  fh2DelR80pcNch        ->GetXaxis()->SetTitle("JetPt");  fh2DelR80pcNch        ->GetYaxis()->SetTitle("R containing 80% of tracks");
  fProDelR80pcNch       ->GetXaxis()->SetTitle("JetPt");  fProDelR80pcNch       ->GetYaxis()->SetTitle("<R> containing 80% of tracks");
  fh2DelR80pcPt         ->GetXaxis()->SetTitle("JetPt");  fh2DelR80pcPt         ->GetYaxis()->SetTitle("R containing 80% of pT");
  fProDelR80pcPt        ->GetXaxis()->SetTitle("JetPt");  fProDelR80pcPt        ->GetYaxis()->SetTitle("<R> containing 80% of pT");
  fh2AreaCh             ->GetXaxis()->SetTitle("JetPt");  fh2AreaCh             ->GetYaxis()->SetTitle("Jet area");
  fProAreaCh            ->GetXaxis()->SetTitle("JetPt");  fProAreaCh            ->GetYaxis()->SetTitle("<jet area>");
  fh3PtDelRNchSum       ->GetXaxis()->SetTitle("JetPt");  fh3PtDelRNchSum       ->GetYaxis()->SetTitle("R");  fh3PtDelRNchSum->GetZaxis()->SetTitle("NchSum");
  fh3PtDelRPtSum        ->GetXaxis()->SetTitle("JetPt");  fh3PtDelRPtSum        ->GetYaxis()->SetTitle("R");  fh3PtDelRPtSum ->GetZaxis()->SetTitle("PtSum");
  fProDiffJetShape->GetXaxis()->SetTitle("R"); 
  fProDiffJetShape->GetYaxis()->SetTitle("Diff jet shape"); 
  fProIntJetShape->GetXaxis()->SetTitle("R");  
  fProIntJetShape->GetYaxis()->SetTitle("Integrated jet shape");  
  
  fCommonHistList->Add(fh1EvtSelection);
  fCommonHistList->Add(fh1VertexNContributors);
  fCommonHistList->Add(fh1VertexZ);
  fCommonHistList->Add(fh1Xsec);
  fCommonHistList->Add(fh1Trials);
  fCommonHistList->Add(fh1PtHard);
  fCommonHistList->Add(fh1PtHardTrials);
  fCommonHistList->Add(fh2EtaJet);
  fCommonHistList->Add(fh2PhiJet);
  fCommonHistList->Add(fh2PtJet);
  fCommonHistList->Add(fh1PtJet);
  fCommonHistList->Add(fh2NtracksJet);
  fCommonHistList->Add(fProNtracksJet);
  fCommonHistList->Add(fh2EtaTrack);
  fCommonHistList->Add(fh2PhiTrack); 
  fCommonHistList->Add(fh2PtTrack); 
  fCommonHistList->Add(fh2FF);  
  fCommonHistList->Add(fh2DelEta);       
  fCommonHistList->Add(fh2DelPhi);   
  fCommonHistList->Add(fh2DelR); 
  
  fCommonHistList->Add(fh1PtLeadingJet); 
  fCommonHistList->Add(fh2NtracksLeadingJet); 
  fCommonHistList->Add(fProNtracksLeadingJet); 
  fCommonHistList->Add(fh2DelR80pcNch); 
  fCommonHistList->Add(fProDelR80pcNch); 
  fCommonHistList->Add(fh2DelR80pcPt); 
  fCommonHistList->Add(fProDelR80pcPt); 
  fCommonHistList->Add(fh2AreaCh); 
  fCommonHistList->Add(fProAreaCh); 
  fCommonHistList->Add(fh3PtDelRNchSum); 
  fCommonHistList->Add(fh3PtDelRPtSum); 
  fCommonHistList->Add(fProDiffJetShape);
  fCommonHistList->Add(fProIntJetShape);
  // =========== Switch on Sumw2 for all histos ===========
  for (Int_t i=0; i<fCommonHistList->GetEntries(); ++i){
    TH1 *h1 = dynamic_cast<TH1*>(fCommonHistList->At(i));
    if (h1) h1->Sumw2();
    else{
      TProfile *hPro = dynamic_cast<TProfile*>(fCommonHistList->At(i));
      if(hPro) hPro->Sumw2();
    }
  }
  
  TH1::AddDirectory(oldStatus);
  PostData(1, fCommonHistList);
}
//_________________________________________________________________________________//

void AliAnalysisTaskJetProperties::Init()
{
  // Initialization
  if(fDebug > 1) Printf("AliAnalysisTaskJetProperties::Init()");

}
//_________________________________________________________________________________//

void AliAnalysisTaskJetProperties::UserExec(Option_t *) 
{
  // Main loop
  // Called for each event
  if(fDebug > 1) Printf("AliAnalysisTaskJetProperties::UserExec()");
  
  if(fDebug > 1) Printf("Analysis event #%5d", (Int_t) fEntry);
  // Trigger selection
  
  AliInputEventHandler* inputHandler = (AliInputEventHandler*)
    ((AliAnalysisManager::GetAnalysisManager())->GetInputEventHandler());
  if(!(inputHandler->IsEventSelected() & AliVEvent::kMB)){
    if(inputHandler->InheritsFrom("AliESDInputHandler") && fUsePhysicsSelection){ // PhysicsSelection only with ESD input
      fh1EvtSelection->Fill(1.);
      if (fDebug > 1 ) Printf(" Trigger Selection: event REJECTED ... ");
      PostData(1, fCommonHistList);
      return;
    }
  }
  
  fESD = dynamic_cast<AliESDEvent*>(InputEvent());
  if(!fESD){
    if(fDebug>3) Printf("%s:%d ESDEvent not found in the input", (char*)__FILE__,__LINE__);
  }
  
  fMCEvent = MCEvent();
  if(!fMCEvent){
    if(fDebug>3) Printf("%s:%d MCEvent not found in the input", (char*)__FILE__,__LINE__);
  }
  
  // get AOD event from input/ouput
  TObject* handler = AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler();
  if( handler && handler->InheritsFrom("AliAODInputHandler") ) {
    fAOD  =  ((AliAODInputHandler*)handler)->GetEvent();
    if(fUseAODInputJets) fAODJets = fAOD;
    if (fDebug > 1)  Printf("%s:%d AOD event from input", (char*)__FILE__,__LINE__);
  }
  else {
    handler = AliAnalysisManager::GetAnalysisManager()->GetOutputEventHandler();
    if( handler && handler->InheritsFrom("AliAODHandler") ) {
      fAOD = ((AliAODHandler*)handler)->GetAOD();
      fAODJets = fAOD;
      if (fDebug > 1)  Printf("%s:%d AOD event from output", (char*)__FILE__,__LINE__);
    }
  }
  
  if(!fAODJets && !fUseAODInputJets){ // case we have AOD in input & output and want jets from output
    TObject* outHandler = AliAnalysisManager::GetAnalysisManager()->GetOutputEventHandler();
    if( outHandler && outHandler->InheritsFrom("AliAODHandler") ) {
      fAODJets = ((AliAODHandler*)outHandler)->GetAOD();
      if (fDebug > 1)  Printf("%s:%d jets from output AOD", (char*)__FILE__,__LINE__);
    }
  }
  
  if(fNonStdFile.Length()!=0){
    // case we have an AOD extension - fetch the jets from the extended output
    
    AliAODHandler *aodH = dynamic_cast<AliAODHandler*>(AliAnalysisManager::GetAnalysisManager()->GetOutputEventHandler());
    fAODExtension = (aodH?aodH->GetExtension(fNonStdFile.Data()):0);    
    if(!fAODExtension){
      if(fDebug>1)Printf("AODExtension not found for %s",fNonStdFile.Data());
    }
  }
  
  if(!fAOD){
    Printf("%s:%d AODEvent not found", (char*)__FILE__,__LINE__);
    return;
  }
  if(!fAODJets){
    Printf("%s:%d AODEvent with jet branch not found", (char*)__FILE__,__LINE__);
    return;
  }
  
  
  // *** vertex cut ***
  AliAODVertex* primVtx = fAOD->GetPrimaryVertex();
  Int_t nTracksPrim = primVtx->GetNContributors();
  fh1VertexNContributors->Fill(nTracksPrim);
  
  if (fDebug > 1) Printf("%s:%d primary vertex selection: %d", (char*)__FILE__,__LINE__,nTracksPrim);
  //if(!nTracksPrim){
  if(nTracksPrim<fNContributors){
    if (fDebug > 1) Printf("%s:%d primary vertex selection: event REJECTED...",(char*)__FILE__,__LINE__); 
    fh1EvtSelection->Fill(3.);
    PostData(1, fCommonHistList);
    return;
  }
  
  fh1VertexZ->Fill(primVtx->GetZ());
  
  if(TMath::Abs(primVtx->GetZ())>fMaxVertexZ){
    if (fDebug > 1) Printf("%s:%d primary vertex z = %f: event REJECTED...",(char*)__FILE__,__LINE__,primVtx->GetZ()); 
    fh1EvtSelection->Fill(4.);
    PostData(1, fCommonHistList);
    return; 
  }
  
  TString primVtxName(primVtx->GetName());
  
  if(primVtxName.CompareTo("TPCVertex",TString::kIgnoreCase) == 1){
    if (fDebug > 1) Printf("%s:%d primary vertex selection: TPC vertex, event REJECTED...",(char*)__FILE__,__LINE__);
    fh1EvtSelection->Fill(5.);
    PostData(1, fCommonHistList);
    return;
  }
  
  if (fDebug > 1) Printf("%s:%d event ACCEPTED ...",(char*)__FILE__,__LINE__); 
  fh1EvtSelection->Fill(0.);
  //___ get MC information __________________________________________________________________
  Double_t ptHard = 0.;
  Double_t nTrials = 1; // trials for MC trigger weight for real data
  
  if(fMCEvent){
    AliGenEventHeader* genHeader = fMCEvent->GenEventHeader();
    if(genHeader){
      AliGenPythiaEventHeader*  pythiaGenHeader = dynamic_cast<AliGenPythiaEventHeader*>(genHeader);
      AliGenHijingEventHeader*  hijingGenHeader = 0x0;
      
      if(pythiaGenHeader){
	if(fDebug>3) Printf("%s:%d pythiaGenHeader found", (char*)__FILE__,__LINE__);
	nTrials = pythiaGenHeader->Trials();
	ptHard  = pythiaGenHeader->GetPtHard();
	
	fh1PtHard->Fill(ptHard);
	fh1PtHardTrials->Fill(ptHard,nTrials);
	
	
      } else { // no pythia, hijing?
	
	if(fDebug>3) Printf("%s:%d no pythiaGenHeader found", (char*)__FILE__,__LINE__);
	
	hijingGenHeader = dynamic_cast<AliGenHijingEventHeader*>(genHeader);
	if(!hijingGenHeader){
	  Printf("%s:%d no pythiaGenHeader or hjingGenHeader found", (char*)__FILE__,__LINE__);
	} else {
	  if(fDebug>3) Printf("%s:%d hijingGenHeader found", (char*)__FILE__,__LINE__);
	}
      }
      
      fh1Trials->Fill("#sum{ntrials}",fAvgTrials);
    }
  }
  
  //___ fetch jets __________________________________________________________________________
  Int_t nJ = GetListOfJets(fJetList);
  Int_t nJets = 0;
  if(nJ>=0) nJets = fJetList->GetEntries();
  if(fDebug>2){
    Printf("%s:%d Selected jets: %d %d",(char*)__FILE__,__LINE__,nJ,nJets);
    if(nJ != nJets) Printf("%s:%d Mismatch Selected Jets: %d %d",(char*)__FILE__,__LINE__,nJ,nJets);
  }
  FillJetProperties(fJetList);
  FillJetShape(fJetList);
  fJetList->Clear();
  //Post output data.
  PostData(1, fCommonHistList);
}
//_________________________________________________________________________________//

void AliAnalysisTaskJetProperties::Terminate(Option_t *) 
{
  // terminated
  
  if(fDebug > 1) printf("AliAnalysisTaskJetProperties::Terminate() \n");
}
//_________________________________________________________________________________//

Int_t AliAnalysisTaskJetProperties::GetListOfJets(TList *list)
{
  //this functionality is motivated by AliAnalysisTaskFragmentationFunction
  if(fDebug > 1) printf("AliAnalysisTaskJetProperties::GetListOfJets() \n");
  // fill list of jets selected according to type
  if(!list){
    if(fDebug>1) Printf("%s:%d no input list", (char*)__FILE__,__LINE__);
    return -1;
  }
  
  if(fBranchJets.Length()==0){
    Printf("%s:%d no jet branch specified", (char*)__FILE__,__LINE__);
    if(fDebug>1)fAOD->Print();
    return 0;
  }
  TClonesArray *aodJets = 0; 
  if(fBranchJets.Length())      aodJets = dynamic_cast<TClonesArray*>(fAODJets->FindListObject(fBranchJets.Data()));
  if(!aodJets)                  aodJets = dynamic_cast<TClonesArray*>(fAODJets->GetList()->FindObject(fBranchJets.Data()));
  if(fAODExtension&&!aodJets)   aodJets = dynamic_cast<TClonesArray*>(fAODExtension->GetAOD()->FindListObject(fBranchJets.Data()));
  
  if(!aodJets){
    if(fBranchJets.Length())Printf("%s:%d no jet array with name %s in AOD", (char*)__FILE__,__LINE__,fBranchJets.Data());
    if(fDebug>1)fAOD->Print();
    return 0;
  }
  
  Int_t nJets = 0;
  for(Int_t ijet=0; ijet<aodJets->GetEntries(); ijet++){
    AliAODJet *tmp = dynamic_cast<AliAODJet*>(aodJets->At(ijet));
    if(!tmp) continue;
    if( tmp->Pt() < fJetPtCut ) continue;
    if( tmp->Eta() < fJetEtaMin || tmp->Eta() > fJetEtaMax)continue;
    if(fJetRejectType==kReject1Track && tmp->GetRefTracks()->GetEntriesFast()==1)continue;//rejecting 1track jet if...
    list->Add(tmp);
    nJets++;
  }//ij loop
  list->Sort();
  return nJets;
}
//_________________________________________________________________________________//

Int_t AliAnalysisTaskJetProperties::GetListOfJetTracks(TList* list, const AliAODJet* jet)
{
  //this functionality is motivated by AliAnalysisTaskFragmentationFunction
  if(fDebug > 1) printf("AliAnalysisTaskJetProperties::GetListOfJetTracks() \n");
  
  // list of jet tracks from trackrefs
  Int_t nTracks = jet->GetRefTracks()->GetEntriesFast();
  Int_t NoOfTracks=0;
  for (Int_t itrack=0; itrack<nTracks; itrack++) {
    if(fTrackType==kTrackUndef){
      if(fDebug>2)Printf("%s:%d unknown track type %d in AOD", (char*)__FILE__,__LINE__,kTrackUndef);
      return 0;
    }//if
    else if(fTrackType == kTrackAOD){
      AliAODTrack* trackAod = dynamic_cast<AliAODTrack*>(jet->GetRefTracks()->At(itrack));
      if(!trackAod){
	AliError("expected ref track not found ");
	continue;
      }
      list->Add(trackAod);
      NoOfTracks++;
    }//if
    
    else if(fTrackType==kTrackAODMC){
      AliAODMCParticle* trackmc = dynamic_cast<AliAODMCParticle*>(jet->GetRefTracks()->At(itrack));
      if(!trackmc){
	AliError("expected ref trackmc not found ");
	continue;
      }
      list->Add(trackmc);
      NoOfTracks++;
    }//if
    
    else if (fTrackType==kTrackKine){
      AliVParticle* trackkine = dynamic_cast<AliVParticle*>(jet->GetRefTracks()->At(itrack));
      if(!trackkine){
	AliError("expected ref trackkine not found ");
	continue;
      }
      list->Add(trackkine);
      NoOfTracks++;
    }//if
  }//itrack loop
  list->Sort();
  return NoOfTracks;
}//initial loop
//_________________________________________________________________________________//

void AliAnalysisTaskJetProperties::FillJetProperties(TList *jetlist){
  //filling up the histograms jets and tracks inside jet
  if(fDebug > 1) printf("AliAnalysisTaskJetProperties::FillJetProperties() \n");
  
  for(Int_t iJet=0; iJet < jetlist->GetEntries(); iJet++){
    Float_t JetPt;Float_t JetPhi;Float_t JetEta;Float_t JetE;
    AliAODJet *jet = dynamic_cast<AliAODJet*>(jetlist->At(iJet));
    if(!jet)continue;
    JetEta = jet->Eta();
    JetPhi = jet->Phi();
    JetPt  = jet->Pt();
    JetE   = jet->E();
    fh2EtaJet ->Fill(JetPt,JetEta);
    fh2PhiJet ->Fill(JetPt,JetPhi);
    fh2PtJet  ->Fill(JetPt,JetPt);
    fh1PtJet  ->Fill(JetPt);
    
    Int_t nJT = GetListOfJetTracks(fTrackList,jet);
    Int_t nJetTracks = 0;
    if(nJT>=0) nJetTracks = fTrackList->GetEntries();
    if(fDebug>2){
      Printf("%s:%d Jet tracks: %d %d",(char*)__FILE__,__LINE__,nJT,nJetTracks);
      if(nJT != nJetTracks) Printf("%s:%d Mismatch Jet Tracks: %d %d",(char*)__FILE__,__LINE__,nJT,nJetTracks);
    }
    
    fh2NtracksJet  ->Fill(JetPt,fTrackList->GetEntries());
    fProNtracksJet ->Fill(JetPt,fTrackList->GetEntries());
    
    for (Int_t j =0; j< fTrackList->GetEntries(); j++){
      if(fTrackType==kTrackUndef)continue;
      	Float_t TrackEta=-99.0; Float_t TrackPt=-99.0; Float_t TrackPhi=-99.0;
	Float_t FF=-99.0;       Float_t DelEta=-99.0;  Float_t DelPhi=-99.0; Float_t DelR=-99.0; Float_t AreaJ=-99.0;
	if(fTrackType==kTrackAOD){
	  AliAODTrack *trackaod = dynamic_cast<AliAODTrack*>(fTrackList->At(j));
	  if(!trackaod)continue;
	  TrackEta = trackaod->Eta();
	  TrackPhi = trackaod->Phi();
	  TrackPt  = trackaod->Pt();
	}//if kTrackAOD
	else if(fTrackType==kTrackAODMC){
	  AliAODMCParticle* trackmc = dynamic_cast<AliAODMCParticle*>(fTrackList->At(j));
	  if(!trackmc)continue;
	  TrackEta = trackmc->Eta();
	  TrackPhi = trackmc->Phi();
	  TrackPt  = trackmc->Pt();
	}//if kTrackAODMC
	else if(fTrackType==kTrackKine){
	  AliVParticle* trackkine = dynamic_cast<AliVParticle*>(fTrackList->At(j));
	  if(!trackkine)continue;
	  TrackEta = trackkine->Eta();
	  TrackPhi = trackkine->Phi();
	  TrackPt  = trackkine->Pt();
	}//kTrackKine
	if(JetPt)FF = TrackPt/JetPt;
	DelEta      = TMath::Abs(JetEta - TrackEta);
	DelPhi      = TMath::Abs(JetPhi - TrackPhi);
	if(DelPhi>TMath::Pi())DelPhi = TMath::Abs(DelPhi-TMath::TwoPi());
	DelR        = TMath::Sqrt(DelEta*DelEta + DelPhi*DelPhi);
	AreaJ       = TMath::Pi()*DelR*DelR;
	fh2EtaTrack ->Fill(JetPt,TrackEta);
	fh2PhiTrack ->Fill(JetPt,TrackPhi);
	fh2PtTrack  ->Fill(JetPt,TrackPt);
	fh2FF       ->Fill(JetPt,FF);
	fh2DelEta   ->Fill(JetPt,DelEta);
	fh2DelPhi   ->Fill(JetPt,DelPhi);
	fh2DelR     ->Fill(JetPt,DelR);
    }//track loop
    fTrackList->Clear();
  }//iJet loop
}//FillJetProperties
//_________________________________________________________________________________//

void AliAnalysisTaskJetProperties::FillJetShape(TList *jetlist){
  //filling up the histograms
  if(fDebug > 1) printf("AliAnalysisTaskJetProperties::FillJetShape() \n");
  Float_t JetEta; Float_t JetPhi; Float_t JetPt;
  AliAODJet *jet = dynamic_cast<AliAODJet*>(jetlist->At(0));//Leading jet
  if(jet){
    JetEta = jet->Eta();
    JetPhi = jet->Phi();
    JetPt  = jet->Pt();
    fh1PtLeadingJet->Fill(JetPt);
    Float_t NchSumA[50]         = {0.};
    Float_t PtSumA[50]          = {0.};
    Float_t delRPtSum80pc       = 0;
    Float_t delRNtrkSum80pc     = 0;
    Float_t PtSumDiffShape[10]  = {0.0};
    Float_t PtSumIntShape[10]   = {0.0};
    Int_t kNbinsR               = 10;

    Int_t nJT = GetListOfJetTracks(fTrackList,jet);
    Int_t nJetTracks = 0;
    if(nJT>=0) nJetTracks = fTrackList->GetEntries();
    if(fDebug>2){
      Printf("%s:%d Jet tracks: %d %d",(char*)__FILE__,__LINE__,nJT,nJetTracks);
      if(nJT != nJetTracks) Printf("%s:%d Mismatch Jet Tracks: %d %d",(char*)__FILE__,__LINE__,nJT,nJetTracks);
    }
    fh2NtracksLeadingJet->Fill(JetPt,nJetTracks);
    fProNtracksLeadingJet->Fill(JetPt,nJetTracks);
    Int_t   *index     = new Int_t   [nJetTracks];//dynamic array containing index
    Float_t *delRA     = new Float_t [nJetTracks];//dynamic array containing delR
    Float_t *delEtaA   = new Float_t [nJetTracks];//dynamic array containing delEta
    Float_t *delPhiA   = new Float_t [nJetTracks];//dynamic array containing delPhi
    Float_t *trackPtA  = new Float_t [nJetTracks];//dynamic array containing pt-track
    Float_t *trackEtaA = new Float_t [nJetTracks];//dynamic array containing eta-track
    Float_t *trackPhiA = new Float_t [nJetTracks];//dynamic array containing phi-track
    for(Int_t ii=0; ii<nJetTracks; ii++){
      index[ii]     = 0;
      delRA[ii]     = 0.;
      delEtaA[ii]   = 0.;
      delPhiA[ii]   = 0.;
      trackPtA[ii]  = 0.;
      trackEtaA[ii] = 0.;
      trackPhiA[ii] = 0.;
    }//ii loop
  
    for (Int_t j =0; j< nJetTracks; j++){
      if(fTrackType==kTrackUndef)continue;      
      
      Float_t TrackEta=-99.0; Float_t TrackPt=-99.0; Float_t TrackPhi=-99.0;
      Float_t DelEta=-99.0;  Float_t DelPhi=-99.0; Float_t DelR=-99.0; Float_t AreaJ=-99.0;
      
      if(fTrackType==kTrackAOD){
	AliAODTrack *trackaod = dynamic_cast<AliAODTrack*>(fTrackList->At(j));
	if(!trackaod)continue;
	TrackEta = trackaod->Eta();
	TrackPhi = trackaod->Phi();
	TrackPt  = trackaod->Pt();
      }//if kTrackAOD
      else if(fTrackType==kTrackAODMC){
	AliAODMCParticle* trackmc = dynamic_cast<AliAODMCParticle*>(fTrackList->At(j));
	if(!trackmc)continue;
	TrackEta = trackmc->Eta();
	TrackPhi = trackmc->Phi();
	TrackPt  = trackmc->Pt();
      }//if kTrackAODMC
      else if(fTrackType==kTrackKine){
	AliVParticle* trackkine = dynamic_cast<AliVParticle*>(fTrackList->At(j));
	if(!trackkine)continue;
	TrackEta = trackkine->Eta();
	TrackPhi = trackkine->Phi();
	TrackPt  = trackkine->Pt();
      }//if kTrackKine
      
      DelEta = TMath::Abs(JetEta - TrackEta);
      DelPhi = TMath::Abs(JetPhi - TrackPhi);
      if(DelPhi>TMath::Pi())DelPhi = TMath::Abs(DelPhi-TMath::TwoPi());
      DelR   = TMath::Sqrt(DelEta*DelEta + DelPhi*DelPhi);
      AreaJ  = TMath::Pi()*DelR*DelR;
      
      fh2AreaCh ->Fill(JetPt,AreaJ);
      fProAreaCh->Fill(JetPt,AreaJ);
      delRA[j]     = DelR;
      delEtaA[j]   = DelEta;
      delPhiA[j]   = DelPhi;
      trackPtA[j]  = TrackPt;
      trackEtaA[j] = TrackEta;
      trackPhiA[j] = TrackPhi;
      
      //calculating diff and integrated jet shapes
      Float_t kDeltaR = 0.1;
      Float_t RMin    = kDeltaR/2.0;
      Float_t RMax    = kDeltaR/2.0;
      Float_t tmpR    = 0.05;
      for(Int_t ii1=0; ii1<kNbinsR;ii1++){
	if((DelR > (tmpR-RMin)) && (DelR <=(tmpR+RMax)))PtSumDiffShape[ii1]+= TrackPt;
	if(DelR>0.0 && DelR <=(tmpR+RMax))PtSumIntShape[ii1]+= TrackPt;
	tmpR += 0.1;
      }//ii1 loop
      
      for(Int_t ibin=1; ibin<=50; ibin++){
	Float_t xlow = 0.02*(ibin-1);
	Float_t xup  = 0.02*ibin;
	if( xlow <= DelR && DelR < xup){
	  NchSumA[ibin-1]++;
	  PtSumA[ibin-1]+= TrackPt;
	}//if loop
      }//for ibin loop
    }//track loop
    fTrackList->Clear();
    
    //---------------------
    Float_t tmp1R = 0.05;
    for(Int_t jj1=0; jj1<kNbinsR;jj1++){
      if(JetPt>20 && JetPt<=100){
	fProDiffJetShape->Fill(tmp1R,PtSumDiffShape[jj1]/JetPt);
	fProIntJetShape ->Fill(tmp1R,PtSumIntShape[jj1]/JetPt);
      }
      Float_t jetPtMin0=20.0; Float_t jetPtMax0=30.0;
      for(Int_t k=0; k<5; k++){
	if(k==0){jetPtMin0=20.0;jetPtMax0=30.0;}
	if(k==1){jetPtMin0=30.0;jetPtMax0=40.0;}
	if(k==2){jetPtMin0=40.0;jetPtMax0=60.0;}
	if(k==3){jetPtMin0=60.0;jetPtMax0=80.0;}
	if(k==4){jetPtMin0=80.0;jetPtMax0=100.0;}
	if(JetPt>jetPtMin0 && JetPt<=jetPtMax0){
	  fProDiffJetShapeA[k]->Fill(tmp1R,PtSumDiffShape[jj1]/JetPt);
	  fProIntJetShapeA[k] ->Fill(tmp1R,PtSumIntShape[jj1]/JetPt);
	}//if
      }//k loop
      tmp1R +=0.1;
    }//jj1 loop
    //----------------------//
    Float_t PtSum = 0;
    Int_t NtrkSum = 0;
    Bool_t iflagPtSum   = kFALSE;
    Bool_t iflagNtrkSum = kFALSE;
    TMath::Sort(nJetTracks,delRA,index,0);
    for(Int_t ii=0; ii<nJetTracks; ii++){
      NtrkSum ++;
      PtSum += trackPtA[index[ii]];
      /*
	cout << index[ii] << "\t" <<
	delR[ii] << "\t" <<
	delEta[ii]<< "\t" <<
	delPhi[ii]<< "\t" <<
	trackPt[ii]<< "\t" <<
	trackEta[ii]<< "\t" <<
	trackPhi[ii]<< "\t DelR " <<
	delR[index[ii]] << endl;
      */
      if(!iflagNtrkSum){
	if((Float_t)NtrkSum/(Float_t)nJetTracks > 0.79){
	  delRNtrkSum80pc = delRA[index[ii]];
	  iflagNtrkSum = kTRUE;
	}//if NtrkSum
      }//if iflag
      if(!iflagPtSum){
	if(PtSum/JetPt >= 0.8000){
	  delRPtSum80pc = delRA[index[ii]];
	  iflagPtSum = kTRUE;
	}//if PtSum
      }//if iflag
    }//track loop 2nd
    delete [] index;
    delete [] delRA;
    delete [] delEtaA;
    delete [] delPhiA;
    delete [] trackPtA;
    delete [] trackEtaA;
    delete [] trackPhiA;
    fh2DelR80pcNch ->Fill(JetPt,delRNtrkSum80pc);
    fProDelR80pcNch->Fill(JetPt,delRNtrkSum80pc);
    fh2DelR80pcPt  ->Fill(JetPt,delRPtSum80pc);
    fProDelR80pcPt ->Fill(JetPt,delRPtSum80pc);
    for(Int_t ibin=0; ibin<50; ibin++){
      Float_t iR = 0.02*ibin + 0.01;
      fh3PtDelRNchSum->Fill(JetPt,iR,NchSumA[ibin]);
      fh3PtDelRPtSum ->Fill(JetPt,iR,PtSumA[ibin]);
      Float_t jetPtMin=20.0; Float_t jetPtMax=30.0;
      for(Int_t k=0; k<5; k++){
	if(k==0){jetPtMin=20.0;jetPtMax=30.0;}
	if(k==1){jetPtMin=30.0;jetPtMax=40.0;}
	if(k==2){jetPtMin=40.0;jetPtMax=60.0;}
	if(k==3){jetPtMin=60.0;jetPtMax=80.0;}
	if(k==4){jetPtMin=80.0;jetPtMax=100.0;}
	if(JetPt>jetPtMin && JetPt<jetPtMax){
	  fProDelRNchSum[k]->Fill(iR,NchSumA[ibin]);
	  fProDelRPtSum[k]->Fill(iR,PtSumA[ibin]);
	}//if
      }//k loop
    }//ibin loop
  }//if(jet)
}//FillJetShape()
//_________________________________________________________________________________//


