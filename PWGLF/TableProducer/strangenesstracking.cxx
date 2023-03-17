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
  using TracksExt = soa::Join<aod::FullTracks, aod::TracksExtra, aod::TracksDCA, aod::TrackSelection, aod::TracksCov, aod::McTrackLabels>;

  Configurable<double> bz{"bz", 50., "magnetic field"};

  void init(InitContext const&)
  {
  }

  void process(aod::Collision const& collision, 
    aod::TrackedCascades const &trackedCascades, aod::Cascades const &cascades,
    aod::TrackedV0s const &trackedV0s, aod::V0s const &v0s,
    aod::Tracked3Bodys const &tracked3Bodys, aod::Decay3Bodys const &decay3Bodys,
    TracksExt const &tracks)
  {
    for (const auto &trackedCascade : trackedCascades) {
      // trackedCascade.globalIndex();
      trackedCascade.itsTrack();
      const auto &casc = trackedCascade.cascade();
      casc.bachelor();
      const auto &v0 = casc.v0();
      const auto &ptrack = v0.posTrack_as<TracksExt>();
      const auto &ntrack = v0.negTrack_as<TracksExt>();
      auto ptrackcov = getTrackParCov(ptrack);
      auto ntrackcov = getTrackParCov(ntrack);
      // propagate to primary vertex
      auto primaryVertex = getPrimaryVertex(collision);
      auto covMatrixPV = primaryVertex.getCov();
      o2::dataformats::DCA impactParameterPos;
      o2::dataformats::DCA impactParameterNeg;
      ptrackcov.propagateToDCA(primaryVertex, bz, &impactParameterPos);
      ntrackcov.propagateToDCA(primaryVertex, bz, &impactParameterNeg);
    }

    for (const auto &trackedV0 : trackedV0s) {
      const auto &v0 = trackedV0.v0();
      v0.posTrack();
      v0.negTrack();
    }

    for (const auto &tracked3Body : tracked3Bodys) {
      tracked3Body.itsTrack();
      const auto &decay3Body = tracked3Body.decay3Body();
      decay3Body.track0();
      decay3Body.track1();
      decay3Body.track2();
    }
  }
};

WorkflowSpec defineDataProcessing(ConfigContext const& cfgc)
{
  return WorkflowSpec{
    adaptAnalysisTask<StrangenessTrackingTask>(cfgc)};
}