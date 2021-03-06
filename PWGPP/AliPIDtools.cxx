#include "AliESDEvent.h"
#include "AliITSPIDResponse.h"
#include "AliTPCPIDResponse.h"
#include "AliTOFPIDResponse.h"
#include "AliPIDResponse.h"
#include "AliESDtrack.h"
#include "AliPIDtools.h"
#include "TLeaf.h"

std::map<Int_t, AliTPCPIDResponse *> AliPIDtools::pidTPC;     /// we should use better hash map
std::map<Int_t, AliPIDResponse *> AliPIDtools::pidAll;        /// we should use better hash map
AliESDtrack  AliPIDtools::dummyTrack;/// dummy value to save CPU - unfortunately PID object use AliVtrack - for the moment create global variable t avoid object constructions
TTree *       AliPIDtools::fFilteredTree = NULL;
TTree *       AliPIDtools::fFilteredTreeV0 = NULL;

AliPIDResponse* AliPIDtools::GetPID(Int_t hash ) {return pidAll[hash];}
AliTPCPIDResponse& AliPIDtools::GetTPCPID(Int_t hash ) {return pidAll[hash]->GetTPCResponse();}
AliITSPIDResponse& AliPIDtools::GetITSPID(Int_t hash ) {return pidAll[hash]->GetITSResponse();}
AliTOFPIDResponse& AliPIDtools::GetTOFPID(Int_t hash ) {return pidAll[hash]->GetTOFResponse();}

Int_t AliPIDtools::GetHash(Int_t run, Int_t passNumber, TString recoPass,Bool_t isMC){
  recoPass+=run;
  recoPass+=passNumber;
  recoPass+=isMC;
  return recoPass.Hash();
}

Double_t AliPIDtools::BetheBlochAleph(Int_t hash, Double_t bg){
  AliTPCPIDResponse *tpcPID=pidTPC[hash];
  if (tpcPID) return tpcPID->Bethe(bg);
  return 0;
}
Double_t AliPIDtools::BetheBlochAleph(Int_t hash, Double_t p,Int_t type){
  AliTPCPIDResponse *tpcPID=pidTPC[hash];
  Float_t bg = p/AliPID::ParticleMass(type);
  if (tpcPID) return tpcPID->Bethe(bg);
  return 0;
}

///  AliPIDtools::BetheBlochITS
/// \param hash   - hash value
/// \param p      - momentum (where?)
/// \param mass   - mass
/// \return
Double_t AliPIDtools::BetheBlochITS(Int_t hash, Double_t p, Double_t mass){
  if (pidAll[hash]== nullptr) return 0;
  AliITSPIDResponse &itsPID=GetITSPID(hash);
  return itsPID.Bethe(p, mass);
}

/// AliPIDtools::GetExpectedITSSignal(
/// \param hash   - hash value
/// \param p      - momentum (where?)
/// \return
Double_t AliPIDtools::GetExpectedITSSignal(Int_t hash, Double_t p, Int_t  particle){
  if (pidAll[hash]== nullptr) return 0;
  AliITSPIDResponse &itsPID=GetITSPID(hash);
  return itsPID.Bethe(p, (AliPID::EParticleType)particle);
}


/// GetExpectedTPCSignal
/// \param hash       - hash value of the PID version
/// \param p          - momenta
/// \param particle   - particle type
/// \return           - mean TPCdedx
Double_t AliPIDtools::GetExpectedTPCSignal(Int_t hash, Double_t p, Int_t  particle) {
  Double_t xyz[3] = {0., 0., 0.};
  Double_t pxyz[3] = {0, 0., 0.};
  Double_t cv[21] = {0.}; // dummy parameters for dummy tracks
  AliTPCPIDResponse *tpcPID=pidTPC[hash];
  if (tpcPID==0) return 0;
  pxyz[0]=p;
  dummyTrack.Set(xyz, pxyz, cv, 1);
  Double_t dEdx = tpcPID->GetExpectedSignal(&dummyTrack, (AliPID::EParticleType)particle, AliTPCPIDResponse::kdEdxDefault, kFALSE, kTRUE);
  return dEdx;
}
/// Load and reguster PID objects in hash maps
/// \param run
/// \param passNumber
/// \param recoPass
/// \param isMC
/// \return  - hash value of PID
Int_t AliPIDtools::LoadPID(Int_t run, Int_t passNumber, TString recoPass, Bool_t isMC){
  // Int_t run=246751, passNumber=1; TString recoPass("pass1"); Bool_t isMC=0;
  AliESDEvent ev;
  AliPIDResponse *pid = new AliPIDResponse(isMC);
  pid->SetUseTPCMultiplicityCorrection();
  pid->SetUseTPCEtaCorrection();
  pid->SetUseTPCPileupCorrection();
  pid->SetOADBPath("$ALICE_PHYSICS/OADB");
  pid->InitialiseEvent(&ev,passNumber, recoPass, run);
  AliTPCPIDResponse &tpcpid=pid->GetTPCResponse();
  // pid.InitFromOADB(246751,1,"pass1");
  Int_t  hash=GetHash(run,passNumber, recoPass,isMC);
  pidAll[hash]=pid;     /// we should clone them
  pidTPC[hash]=&tpcpid;  ///
  return hash;
}

Double_t AliPIDtools::GetExpectedTOFSigma(Int_t hash, Float_t mom, Int_t  type){
  Double_t dummyTime=0;
  if (pidAll[hash]== nullptr) return 0;
  AliTOFPIDResponse &tofPID=GetTOFPID(hash);
  return tofPID.GetExpectedSigma(mom,dummyTime,(AliPID::EParticleType)type);

}
Double_t AliPIDtools::GetExpectedTOFSignal(Int_t hash, const AliVTrack *track, Int_t type){
  if (pidAll[hash]== nullptr) return 0;
  AliTOFPIDResponse &tofPID=GetTOFPID(hash);
  return tofPID.GetExpectedSignal(track, (AliPID::EParticleType)type);
}

///  SetFiltered tree
Bool_t AliPIDtools::SetFilteredTree(TTree * filteredTree){
  if (filteredTree==NULL) return kFALSE;
  TBranch * branch = filteredTree->GetBranch("esdTrack.");
  if (branch==NULL) {
    ::Error("AliPIDtools::SetFilteredTreeV0","Invalid tree. Branch esdTrack does not exist");
    return kFALSE;
  }
  fFilteredTree=filteredTree;
  return kTRUE;
}

///  SetFiltered tree
Bool_t AliPIDtools::SetFilteredTreeV0(TTree * filteredTreeV0){
  if (filteredTreeV0==NULL) return kFALSE;
  TBranch * branch = filteredTreeV0->GetBranch("v0.");
  if (branch==NULL) {
    ::Error("AliPIDtools::SetFilteredTreeV0","Invalid tree. Branch v0 does not exist");
    return kFALSE;
  }
  fFilteredTreeV0=filteredTreeV0;
  return kTRUE;
}




/// GetExpected TPC signal for current track
/// \param hash                 - PID hash
/// \param particleType         - assumed particle type
/// \param corrMask             - corr mask
///                                 0x1 - eta correction
///                                 0x2 - multiplicity correction
///                                 0x4 - pile-up correction
///                                 0x8  - return pileup correction
/// \param returnType            0
///                                 0 - expected signal
///                                 1 - corrected signal
/// \return                     - expected dEdx signal
Double_t AliPIDtools::GetExpectedTPCSignal(Int_t hash, Int_t particleType, Int_t corrMask, Int_t returnType){
  //
  AliTPCPIDResponse *tpcPID=pidTPC[hash];
  Double_t dEdx=0;
  AliESDtrack **pptrack=0;
  TVectorF   **pptpcVertexInfo=0;
  TVectorF   **ppitsClustersPerLayer=0;
  Float_t primMult=0;
  if (fFilteredTree){  // data from filtered trees
    Int_t entry = fFilteredTree->GetReadEntry();
    static  TBranch * branch = NULL;
    static TBranch *branchVertex=0;
    static TBranch *branchITS=0;
    static TLeaf * leafPrim=0;
    static  Int_t treeNumber=-1;
    fFilteredTree->GetEntry(entry);   // load full tree - branch GetEntry is loading only for fit file in TChain  //TODO fix
    if (treeNumber!=fFilteredTree->GetTreeNumber()){
      branch=fFilteredTree->GetTree()->GetBranch("esdTrack.");
      if (fFilteredTree->GetFriend("E")) {
        branchVertex = fFilteredTreeV0->GetFriend("E")->GetBranch("tpcVertexInfoESD.");
        branchITS = fFilteredTreeV0->GetFriend("E")->GetBranch("itsClustersPerLayer.");
        leafPrim = fFilteredTreeV0->GetFriend("E")->GetLeaf("primMult");
      }
      treeNumber=fFilteredTree->GetTreeNumber();
    }
    pptrack = (AliESDtrack **)(branch->GetAddress());
    if (branchVertex) {
      pptpcVertexInfo = (branchVertex != NULL) ? (TVectorF **) (branchVertex->GetAddress()) : NULL;
      ppitsClustersPerLayer = (branchITS != NULL) ? (TVectorF **) (branchITS->GetAddress()) : NULL;
      SetPileUpProperties(**pptpcVertexInfo, **ppitsClustersPerLayer, leafPrim->GetValue(), tpcPID);
    }
    if (corrMask==-1) return entry;
    if (corrMask==-2) return (*pptrack)->Pt();
    if (corrMask==-3) return treeNumber;
    if (corrMask==-4 && pptpcVertexInfo) return (*(*pptpcVertexInfo))[0];
    //if (corrMask==-100) return tpcPID->GetP ileUpProperties(0);   - wait until new AliRoot distributed
    //if (corrMask==-101) return tpcPID->GetPileUpProperties(1);
    //if (corrMask==-102) return tpcPID->GetPileUpProperties(2);

  }
  if (pptrack==0) return 0;
  if (corrMask==0x8) return tpcPID->GetPileupCorrectionValue(*pptrack);
  if (returnType==0) {
    dEdx = tpcPID->GetExpectedSignal(*pptrack, (AliPID::EParticleType) particleType, AliTPCPIDResponse::kdEdxDefault, corrMask & 0x1, corrMask & 0x2, corrMask & 0x4);
    return dEdx;
  }
  if (returnType==1) {
    dEdx = tpcPID->GetCorrectedTrackdEdx(*pptrack, (AliPID::EParticleType) particleType, corrMask & 0x1, corrMask & 0x2, corrMask & 0x4, AliTPCPIDResponse::kdEdxDefault);
    return dEdx;
  }
  return 0;
}

/// GetExpected TPC signal for current V0 track
/// \param hash                 - PID hash
/// \param particleType         - assumed particle type
/// \param corrMask             - corr mask
///                                 0x1 - eta correction
///                                 0x2 - multiplicity correction
///                                 0x4 - pile-up correction
///                                 0x8  - return pileup correction
/// \param index                - track index
/// \return                     - expected dEdx signal
Double_t AliPIDtools::GetExpectedTPCSignalV0(Int_t hash, Int_t particleType, Int_t corrMask, Int_t index, Int_t returnType){
  //
  AliTPCPIDResponse *tpcPID=pidTPC[hash];
  Double_t dEdx=0;
  AliESDtrack **pptrack=0;
  TVectorF   **pptpcVertexInfo=0;
  TVectorF   **ppitsClustersPerLayer=0;
  if (fFilteredTreeV0){  // data from filtered trees
    Int_t entry = fFilteredTreeV0->GetReadEntry();
    static TBranch * branch0, *branch1 = NULL;
    static TBranch *branchVertex=0;
    static TBranch *branchITS=0;
    static Int_t treeNumber=-1;
    static TLeaf * leafPrim=0;
    fFilteredTreeV0->GetEntry(entry);   // load full tree - branch GetEntry is loading only for fit file in TChain  //TODO fix
    if (treeNumber!=fFilteredTreeV0->GetTreeNumber()){
      branch0=fFilteredTreeV0->GetTree()->GetBranch("track0.");
      branch1=fFilteredTreeV0->GetTree()->GetBranch("track1.");
      if (fFilteredTree->GetFriend("E")) {
          branchVertex = fFilteredTreeV0->GetFriend("E")->GetBranch("tpcVertexInfoESD.");
          branchITS = fFilteredTreeV0->GetFriend("E")->GetBranch("itsClustersPerLayer.");
          leafPrim = fFilteredTreeV0->GetFriend("E")->GetLeaf("primMult");
      }
      treeNumber=fFilteredTreeV0->GetTreeNumber();
    }
    pptrack = (index==0) ? (AliESDtrack **)(branch0->GetAddress()):(AliESDtrack **)(branch1->GetAddress());
    if (branchVertex) {
      pptpcVertexInfo = (branchVertex != NULL) ? (TVectorF **) (branchVertex->GetAddress()) : NULL;
      ppitsClustersPerLayer = (branchITS != NULL) ? (TVectorF **) (branchITS->GetAddress()) : NULL;
      SetPileUpProperties(**pptpcVertexInfo, **ppitsClustersPerLayer, leafPrim->GetValue(), tpcPID);
    }
    if (corrMask==-1) return entry;
    if (corrMask==-2) return (*pptrack)->Pt();
    if (corrMask==-3) return treeNumber;
    if (corrMask==-4 && pptpcVertexInfo) return (*(*pptpcVertexInfo))[0];
  }

  if (pptrack==0) return 0;
  if (corrMask==0x8) return tpcPID->GetPileupCorrectionValue(*pptrack);

  if (returnType==0) {
    dEdx = tpcPID->GetExpectedSignal(*pptrack, (AliPID::EParticleType) particleType, AliTPCPIDResponse::kdEdxDefault, corrMask & 0x1, corrMask & 0x2, corrMask & 0x4);
    return dEdx;
  }
  if (returnType==1) {
    dEdx = tpcPID->GetCorrectedTrackdEdx(*pptrack, (AliPID::EParticleType) particleType, corrMask & 0x1, corrMask & 0x2, corrMask & 0x4, AliTPCPIDResponse::kdEdxDefault);
    return dEdx;
  }
  return 0;
}
Bool_t AliPIDtools::SetPileUpProperties(const TVectorF & tpcVertexInfo, const TVectorF &itsClustersPerLayer, Int_t primMult, AliTPCPIDResponse *pidTPC){
  const Float_t  itsToTPC=2.38;        // conversion from SDD+SSD to TPC multiplicity
  const Float_t  multFraction=0.05;   //
  // ===| calculate derived variables |=========================================
  const Double_t shiftM = 0.5 * (tpcVertexInfo[1] + tpcVertexInfo[0]) - 25.;
  const Double_t multSSD = itsClustersPerLayer[4] + itsClustersPerLayer[5];
  const Double_t multSDD = itsClustersPerLayer[2] + itsClustersPerLayer[3];
  const Double_t multITSTPC = (multSSD + multSDD) / itsToTPC;
  const Double_t nPileUpSumCorr = (tpcVertexInfo[3] + tpcVertexInfo[4]) - multFraction * multITSTPC;
  const Double_t nPileUpPrim = nPileUpSumCorr / (1. - TMath::Abs(shiftM / 210.));
  // ===| set pileup event properties |=========================================
  pidTPC->SetEventPileupProperties(shiftM,nPileUpPrim,primMult);
}

AliESDtrack* AliPIDtools::GetCurrentTrack() {
  AliESDtrack **pptrack = 0;
  if (fFilteredTree) {  // data from filtered trees
    Int_t entry = fFilteredTree->GetReadEntry();
    static Int_t lastEntry=-1;
    static TBranch *branch = NULL;
    static Int_t treeNumber = -1;
    if (lastEntry!=entry) {
      fFilteredTree->GetEntry(entry);   // load full tree - branch GetEntry is loading only for fit file in TChain
      if (treeNumber != fFilteredTree->GetTreeNumber()) {
        branch = fFilteredTree->GetTree()->GetBranch("esdTrack.");
        treeNumber = fFilteredTree->GetTreeNumber();
      }
      lastEntry=entry;
    }
    pptrack = (AliESDtrack **) (branch->GetAddress());
  }
  return *pptrack;
}

AliESDtrack* AliPIDtools::GetCurrentTrackV0(Int_t index) {
  AliESDtrack **pptrack = 0;
  if (fFilteredTreeV0) {  // data from filtered trees
    Int_t entry = fFilteredTreeV0->GetReadEntry();
    static TBranch *branch0, *branch1 = NULL;
    static Int_t treeNumber = -1;
    fFilteredTreeV0->GetEntry(entry);   // load full tree - branch GetEntry is loading only for fit file in TChain  //TODO fix
    if (treeNumber != fFilteredTreeV0->GetTreeNumber()) {
      branch0 = fFilteredTreeV0->GetTree()->GetBranch("track0.");
      branch1 = fFilteredTreeV0->GetTree()->GetBranch("track1.");
      treeNumber = fFilteredTreeV0->GetTreeNumber();
    }
    pptrack = (index == 0) ? (AliESDtrack **) (branch0->GetAddress()) : (AliESDtrack **) (branch1->GetAddress());
    return *pptrack;
  }
  return 0;
}

/// GetITSPID for given particle and valu type
/// TODO - interface other PID options
/// \param hash              - hash value of PID
/// \param particleType      - particle type
/// \param valueType         - return type  of value
///                            0 - delta
///                            1 - n sigma
///                            >1 - custom likelihood
/// \param resol
/// \return  value
Double_t AliPIDtools::GetITSPID(Int_t hash, Int_t particleType, Int_t valueType, Float_t resol){
  if (particleType>AliPID::kSPECIESC) return 0;
  AliITSPIDResponse &itsPID=pidAll[hash]->GetITSResponse();
  AliESDtrack *track=GetCurrentTrack();
  if (valueType==0) return itsPID.GetSignalDelta(track,(AliPID::EParticleType)particleType);
  if (valueType==1) return itsPID.GetNumberOfSigmas(track,(AliPID::EParticleType)particleType);
  Double_t prob[AliPID::kSPECIESC]={};
  if (resol>0){
    for (Int_t i=0;i<AliPID::kSPECIESC;i++){
      Float_t delta=itsPID.GetSignalDelta(track,(AliPID::EParticleType)i);
      prob[i]=TMath::Gaus(delta,resol);
    }
  }else{
    for (Int_t i=0;i<AliPID::kSPECIESC;i++){
      Float_t delta=itsPID.GetNumberOfSigmas(track,(AliPID::EParticleType)i);
      prob[i]=TMath::Gaus(delta,1);
    }
  }
  Double_t sumProb=0;
  for (Int_t i=0;i<AliPID::kSPECIESC;i++){sumProb+=prob[i];}
  if (sumProb==0) return 0;
  return prob[particleType]/sumProb;
}

Double_t AliPIDtools::GetTOFPID(Int_t hash, Int_t particleType, Int_t valueType, Float_t resol){
  if (particleType>AliPID::kSPECIESC) return 0;
  AliTOFPIDResponse &tofPID=pidAll[hash]->GetTOFResponse();
  AliESDtrack *track=GetCurrentTrack();
  return 0;
}

///
/// \param hash
/// \param detCode
/// \param particleType
/// \param source
/// \return
Float_t AliPIDtools::NumberOfSigmas(Int_t hash, Int_t detCode, Int_t particleType, Int_t source){
  if (pidAll[hash]==NULL) return 0;
  AliESDtrack *track=NULL;
  if (source<0)track=GetCurrentTrack();
  if (source>=0)track=GetCurrentTrackV0(source%2);
  return pidAll[hash]->NumberOfSigmas((AliPIDResponse::EDetector) detCode, track, (AliPID::EParticleType)particleType);
}
