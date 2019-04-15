#ifndef FOOLGO_SRC_PLAYER_NODE_RECORD_H_
#define FOOLGO_SRC_PLAYER_NODE_RECORD_H_

#include <atomic>
#include <cstdint>
#include <map>
#include <mutex>
#include <ostream>

#include "../board/position.h"
#include "../def.h"

namespace foolgo {

class NodeRecord {
public:
NodeRecord();
NodeRecord(int32_t visited_time, float average_profit, bool is_in_search)
        : visited_time_(visited_time),
        average_profit_(average_profit),
        is_in_search_(is_in_search) {
}
NodeRecord(const NodeRecord &node_record);
NodeRecord& operator =(const NodeRecord &node_record) = default;
int32_t GetVisitedTime() const {
        return visited_time_;
}
void SetVisitedTimes(int32_t visited_times) {
        visited_time_ = visited_times;
}
float GetAverageProfit() const {
        return average_profit_;
}
void SetAverageProfit(float average_profit) {
        average_profit_ = average_profit;
}
bool IsInSearch() const {
        return is_in_search_;
}

const HashKey* GetChildHashKeyPtr(
        PositionIndex position_index) const {
        mutex_.lock();
        auto it = child_hash_keys_.find(position_index);
        mutex_.unlock();
        const HashKey* result = (it == child_hash_keys_.end() ? nullptr : &(it->second));
        return result;
}

void SetIsInSearch(bool is_in_search) {
        is_in_search_ = is_in_search;
}

const HashKey* GetChildPtr(PositionIndex position_index) const;
void InsertChildHashKey(PositionIndex position_index,
                        HashKey hash_key);

private:
int32_t visited_time_;
float average_profit_;
bool is_in_search_;
std::map<PositionIndex, HashKey> child_hash_keys_;
mutable std::mutex mutex_;

friend std ::ostream& operator <<(std::ostream &os,
                                  const NodeRecord &node_record);
};

}

#endif /* FOOLGO_SRC_PLAYER_NODE_RECORD_H_ */
