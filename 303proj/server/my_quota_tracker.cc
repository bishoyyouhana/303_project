// http://www.cplusplus.com/reference/ctime/time/ is helpful here
#include <deque>
#include <iostream>
#include <memory>

#include "quota_tracker.h"

using namespace std;

/// quota_tracker stores time-ordered information about events.  It can count
/// events within a pre-set, fixed time threshold, to decide if a new event can
/// be allowed without violating a quota.  
class my_quota_tracker : public quota_tracker {

  size_t qAmount;
  double qDuration;
  std::deque<std::pair<time_t, size_t>> quotaTracker;

public:
  /// Construct a tracker that limits usage to quota_amount per quota_duration
  /// seconds
  ///
  /// @param amount   The maximum amount of service
  /// @param duration The time over which the service maximum can be spread out
  my_quota_tracker(size_t amount, double duration) {
    qAmount = amount;
    qDuration = duration;
  }

  /// Destruct a quota tracker
  virtual ~my_quota_tracker() {
  }

  /// Decide if a new event is permitted, and if so, add it.  The attempt is
  /// allowed if it could be added to events, while ensuring that the sum of
  /// amounts for all events within the duration is less than q_amnt.
  ///
  /// @param amount The amount of the new request
  ///
  /// @return false if the amount could not be added without violating the
  ///         quota, true if the amount was added while preserving the quota
  virtual bool check_add(size_t amount) {
    // cout << "my_quota_tracker.cc::check_add() is not implemented\n";
    // first get current time
    time_t timer;
    time(&timer);
    // then from the head of list, check if duration is maximized 
    while (true) {
      if (quotaTracker.size()==0) // special case
        break;
      if(difftime(timer,quotaTracker.at(0).first) > qDuration) // remove head
        quotaTracker.erase(quotaTracker.begin());
      else
        break;
    }
    // then check capacity of qouta tracker, see if amount is allowed
    size_t currAmount = 0;
    for(auto it = quotaTracker.begin(); it != quotaTracker.end(); it++) {
      currAmount += (*it).second;
    }
    // then check amount and add task in if true
    if (currAmount + amount <= qAmount) {
      std::pair<time_t, size_t> tail;
      tail.first = timer;
      tail.second = amount;
      quotaTracker.emplace_back(tail);
      return true;
    }
    return false;
  }
};

/// Construct a tracker that limits usage to quota_amount per quota_duration
/// seconds
///
/// @param amount   The maximum amount of service
/// @param duration The time over which the service maximum can be spread out
quota_tracker *quota_factory(size_t amount, double duration) {
  return new my_quota_tracker(amount, duration);
}