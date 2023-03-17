// Copyright 2019-2020 CERN and copyright holders of ALICE O2.
// See https://alice-o2.web.cern.ch/copyright for details of the copyright holders.
// All rights not expressly granted are reserved.
//
// This software is distributed under the terms of the GNU General Public
// License v3 (GPL Version 3), copied verbatim in the file "COPYING".
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "Framework/AnalysisTask.h"
#include "Framework/AnalysisDataModel.h"
#include "Framework/ASoA.h"
#include "Framework/runDataProcessing.h"
#include "Common/DataModel/TrackSelectionTables.h"
#include "Common/Core/trackUtilities.h"
#include "ReconstructionDataFormats/DCA.h"
#include "PWGLF/DataModel/LFStrangenessTracking.h"

using namespace o2;
using namespace o2::framework;
using namespace o2::framework::expressions;

struct StrangenessTrackingTask {
  // using TracksExt = soa::Join<aod::FullTracks, aod::TracksExtra, aod::TracksDCA, aod::TrackSelection, aod::TracksCov, aod::McTrackLabels>;
  using TracksExt = soa::Join<aod::FullTracks, aod::TracksExtra, aod::TracksDCA, aod::TrackSelection, aod::McTrackLabels>;

  Configurable<double> bz{"bz", 50., "magnetic field"};

  OutputObj<TH1F> hDCA{"h_DCA"};

  float Square(float x) { return x*x; }

  void init(InitContext const&)
  {
    hDCA.setObject(new TH1F("h_dca", "DCA;DCA (cm)", 200, -2., 2.));
  }

  void process(aod::Collision const& collision, 
    aod::TrackedCascades const &trackedCascades, aod::Cascades const &cascades,
    aod::V0s const &v0s, TracksExt const &tracks, aod::McParticles const &mcParticles)
  {
    for (const auto &trackedCascade : trackedCascades) {
      auto trackCovTrk = getTrackParCov(trackedCascade);
      auto primaryVertex = getPrimaryVertex(collision);
      auto covMatrixPV = primaryVertex.getCov();
      o2::dataformats::DCA impactParameterTrk;
      trackCovTrk.propagateToDCA(primaryVertex, bz, &impactParameterTrk);

      hDCA->Fill(impactParameterTrk.getY());

      const auto &casc = trackedCascade.cascade();
      const auto &bachelor = casc.bachelor_as<TracksExt>();
      const auto &v0 = casc.v0();
      const auto &ptrack = v0.posTrack_as<TracksExt>();
      const auto &ntrack = v0.negTrack_as<TracksExt>();

      if (ptrack.mcParticle().has_mothers() && ntrack.mcParticle().has_mothers() && 
          ptrack.mcParticle().mothersIds()[0] == ntrack.mcParticle().mothersIds()[0]) {
            const auto &v0part = ptrack.mcParticle().mothers_as<aod::McParticles>()[0];
            if (v0part.has_mothers() && bachelor.mcParticle().has_mothers() &&
                v0part.mothersIds()[0] == bachelor.mcParticle().mothersIds()[0])
                LOG(debug) << "cascade with PDG code: " << v0part.mothers_as<aod::McParticles>()[0].pdgCode();
      }
    }
  }

  // void process(aod::Collision const& collision, 
  //   aod::TrackedCascades const &trackedCascades, aod::Cascades const &cascades,
  //   aod::TrackedV0s const &trackedV0s, aod::V0s const &v0s,
  //   aod::Tracked3Bodys const &tracked3Bodys, aod::Decay3Bodys const &decay3Bodys,
  //   TracksExt const &tracks, aod::McParticles const &mcParticles)
  // {
  //   for (const auto &trackedCascade : trackedCascades) {
  //     const auto &itsTrack = trackedCascade.itsTrack();
  //     const auto &casc = trackedCascade.cascade();
  //     const auto &bachelor = casc.bachelor_as<TracksExt>();
  //     bachelor.mcParticle().pdgCode();
  //     const auto &v0 = casc.v0();
  //     const auto &ptrack = v0.posTrack_as<TracksExt>();
  //     ptrack.mcParticle().pdgCode();
  //     const auto &ntrack = v0.negTrack_as<TracksExt>();
  //     ptrack.mcParticle().pdgCode();

  //     if (ptrack.mcParticle().has_mothers() && ntrack.mcParticle().has_mothers() && 
  //         ptrack.mcParticle().mothersIds()[0] == ntrack.mcParticle().mothersIds()[0]) {
  //           const auto &v0part = ptrack.mcParticle().mothers_as<aod::McParticles>()[0];
  //           if (v0part.has_mothers() && bachelor.mcParticle().has_mothers() &&
  //               v0part.mothersIds()[0] == bachelor.mcParticle().mothersIds()[0])
  //               LOG(debug) << "cascade with PDG code: " << v0part.mothers_as<aod::McParticles>()[0].pdgCode();
  //     }
  //     // auto ptrackcov = getTrackParCov(ptrack);
  //     // auto ntrackcov = getTrackParCov(ntrack);
  //     auto trackCovTrk = getTrackParCov(trackedCascade);

  //     // propagate to primary vertex
  //     auto primaryVertex = getPrimaryVertex(collision);
  //     auto covMatrixPV = primaryVertex.getCov();
  //     // o2::dataformats::DCA impactParameterPos;
  //     // o2::dataformats::DCA impactParameterNeg;
  //     o2::dataformats::DCA impactParameterTrk;
  //     // ptrackcov.propagateToDCA(primaryVertex, bz, &impactParameterPos);
  //     // ntrackcov.propagateToDCA(primaryVertex, bz, &impactParameterNeg);
  //     trackCovTrk.propagateToDCA(primaryVertex, bz, &impactParameterTrk);

  //     float dca = TMath::Sqrt(Square(trackCovTrk.getX() - primaryVertex.getX()) + Square(trackCovTrk.getY() - primaryVertex.getY()));
  //     hDCA->Fill(impactParameterTrk.getY());
  //   }

  //   for (const auto &trackedV0 : trackedV0s) {
  //     const auto &v0 = trackedV0.v0();
  //     v0.posTrack();
  //     v0.negTrack();
  //   }

  //   for (const auto &tracked3Body : tracked3Bodys) {
  //     tracked3Body.itsTrack();
  //     const auto &decay3Body = tracked3Body.decay3Body();
  //     decay3Body.track0();
  //     decay3Body.track1();
  //     decay3Body.track2();
  //   }
  // }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<StrangenessTrackingTask>(cfgc)};
}