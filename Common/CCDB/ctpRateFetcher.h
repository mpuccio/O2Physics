#ifndef COMMON_TOOLS_FETCHCTPRATES_H_
#define COMMON_TOOLS_FETCHCTPRATES_H_

#include <string>

#include "CCDB/BasicCCDBManager.h"
#include "Framework/AnalysisHelpers.h"

namespace o2 {

namespace ctp {
class CTPRunScalers;
class CTPConfiguration;
}

namespace parameters {
class GRPLHCIFData;
}

class ctpRateFetcher {
 public:
  ctpRateFetcher() = default;
  double fetch(framework::Service<o2::ccdb::BasicCCDBManager>& ccdb, uint64_t timeStamp, int runNumber, std::string sourceName);

 private:
  void getCTPconfig(framework::Service<o2::ccdb::BasicCCDBManager>& ccdb, uint64_t timeStamp, int runNumber);
  void getCTPscalers(framework::Service<o2::ccdb::BasicCCDBManager>& ccdb, uint64_t timeStamp, int runNumber);
  void getLHCIFdata(framework::Service<o2::ccdb::BasicCCDBManager>& ccdb, uint64_t timeStamp, int runNumber);
  double fetchCTPratesInputs(framework::Service<o2::ccdb::BasicCCDBManager>& ccdb, uint64_t timeStamp, int runNumber, int input);
  double fetchCTPratesClasses(framework::Service<o2::ccdb::BasicCCDBManager>& ccdb, uint64_t timeStamp, int runNumber, std::string className);
  double pileUpCorrection(double rate);

  int mRunNumber = -1;
  ctp::CTPConfiguration* mConfig = nullptr;
  ctp::CTPRunScalers* mScalers = nullptr;
  parameters::GRPLHCIFData* mLHCIFdata = nullptr;
};
}  // namespace o2

#endif