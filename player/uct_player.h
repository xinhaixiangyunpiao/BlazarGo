#ifndef FOOLGO_SRC_PLAYER_UCT_PLAYER_H_

#define FOOLGO_SRC_PLAYER_UCT_PLAYER_H_

#include <atomic>
#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <cassert>
#include <condition_variable>
#include <cmath>
#include <cstdint>
#include <future>
#include <mutex>
#include <memory>
#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sstream>
#include <unordered_map>

#include "../board/force.h"
#include "../board/full_board.h"
#include "../board/position.h"
#include "../game/monte_carlo_game.h"
#include "node_record.h"
#include "passable_player.h"
#include "transposition_table.h"
#include "../convey.h"

extern convey con;

namespace foolgo {

using std::cout;
using std::endl;
using std::string;
using std::stringstream;

template<BoardLen BOARD_LEN>
class UctPlayer : public PassablePlayer<BOARD_LEN> {
public:
UctPlayer(uint32_t seed,int history_count_per_move, int future_count_per_move, int thread_count);

PositionIndex NextMoveWithPlayableBoard(
        FullBoard<BOARD_LEN> &full_board);

virtual void BeforePlay(PositionIndex index){
}

private:
int history_count_per_move_;
int future_count_per_move_;
TranspositionTable<BOARD_LEN> transposition_table_;
uint32_t seed_;
int thread_count_;
mutable std::mutex mutex_;

void SearchAndModifyNodes(const FullBoard<BOARD_LEN> &full_board,
                          std::atomic<int> *future_count_ptr,
                          int thread_index);
void Search(FullBoard<BOARD_LEN> &full_board,
            std::atomic<int> *history_count_ptr,
            std::atomic<int> *future_count_ptr,
            std::vector<FullBoard<BOARD_LEN>> &Boards,
            int thread_index);
PositionIndex MaxUcbChild(
        const FullBoard<BOARD_LEN> &full_board,
        int thread_index);
float ModifyAverageProfitAndReturnNewProfit(
        FullBoard<BOARD_LEN> *full_board_ptr,
        std::atomic<int> *future_count_ptr,
        int thread_index);
void BestChild(const FullBoard<BOARD_LEN> &full_board);
};

namespace {

int markBoard[81];
int timesBoard[81];
float finalBoard[81];

float Ucb(const NodeRecord &node_record, int visited_count_sum) {
        //assert(node_record.GetVisitedTime() > 0);
        if(node_record.GetVisitedTime() <= 0){
                return 0;
        }
        else
        return node_record.GetAverageProfit()
               + sqrt(2 * log(visited_count_sum) / node_record.GetVisitedTime());
}

template<BoardLen BOARD_LEN>
float GetRegionRatio(const FullBoard<BOARD_LEN> &full_board,
                     Force force) {
        int black_region = full_board.BlackRegion();
        float black_ratio = static_cast<float>(black_region)
                            / BoardLenSquare<BOARD_LEN>();
        return force == Force::BLACK_FORCE ? black_ratio : 1 - black_ratio;
}
}

template<BoardLen BOARD_LEN>
UctPlayer<BOARD_LEN>::UctPlayer(uint32_t seed,int history_count_per_move, 
                                int future_count_per_move,
                                int thread_count)
        : seed_(seed),
        history_count_per_move_(history_count_per_move),
        future_count_per_move_(future_count_per_move),
        thread_count_(thread_count) {
}

static int ceng[8] = {0};
static int ERROR = 0;

template<BoardLen BOARD_LEN>
PositionIndex UctPlayer<BOARD_LEN>::NextMoveWithPlayableBoard(
        FullBoard<BOARD_LEN> &full_board) {
        std::atomic<int> current_history_count(0);
        std::atomic<int> current_future_count(0);
        std::vector<FullBoard<BOARD_LEN>> Boards;
        std::vector<std::future<void> > futures;
        memset(markBoard,0,sizeof(markBoard));
        ERROR = 0;
        for(int i = 0; i < 8; i++)
                ceng[i] = 0;
        for(int i = 0; i < 81; i++)
                timesBoard[i] = history_count_per_move_;
        memset(finalBoard,0,sizeof(finalBoard));
        for (int i=0; i < thread_count_; ++i) {
                auto f = std::async(std::launch::async,
                                    &UctPlayer<BOARD_LEN>::Search, this,
                                    std::ref(full_board),
                                    &current_history_count,
                                    &current_future_count,
                                    std::ref(Boards),
                                    i);
                futures.push_back(std::move(f));
        }
        for (std::future<void> &future : futures) {
                future.wait();
        }
        /** 异常检测 **/
        if(ERROR)
                return -1;
        /** 去重 **/
        std::unordered_map <HashKey,int> tags;
        std::vector<FullBoard<BOARD_LEN>> newBoards;
        int Boards_size = Boards.size();
        for(int i = 0; i < Boards_size; i++){
                if(tags[Boards[i].HashKey()]++ > 0){

                }else{
                        newBoards.push_back(Boards[i]);
                }
        }
        int newBoards_size = newBoards.size();
        cout << "Size: " << newBoards_size << endl;
        for(auto i : newBoards){
                BestChild(i);
        }
        /** 统计结果 **/
        stringstream ss;
        float max = 0;
        int index = -1;
        ss << "scores: " << std::endl;
//        for(int i = 0; i < 81; i++) {
//                ss << std::setw(8) << markBoard[i];
//                if((i+1)%9 == 0)
//                        ss << std::endl;
//        }
//        ss << std::endl << "times: " << std::endl;
//        for(int i = 0; i < 81; i++) {
//                ss << std::setw(8) << timesBoard[i];
//                if((i+1)%9 == 0)
//                        ss << std::endl;
//        }
//        ss << std::endl;
//        ss << "final: " << std::endl;
        for(int i = 0; i < 81; i++) {
                if(timesBoard[i] == 0)
                        finalBoard[i] = 0;
                else
                        finalBoard[i] = (float)(markBoard[i])/timesBoard[i];
                ss.precision(2);
                ss.setf(std::ios::fixed);
                ss << std::setw(8) << finalBoard[i];
                if((i+1)%9 == 0)
                        ss << std::endl;
        }
        for(int i = 0; i < 81; i++) {
                if(finalBoard[i] > max) {
                        index = i;
                        max = finalBoard[i];
                }
        }
        ss << "max: " << max << std::endl;
        ss << "index: " << index << std::endl;
        con.SetInformation(ss.str());
        return (PositionIndex)index;
}
//  std::atomic<bool> *is_end_ptr,

template<BoardLen BOARD_LEN>
void UctPlayer<BOARD_LEN>::Search(FullBoard<BOARD_LEN> &full_board,
                                  std::atomic<int> *history_count_ptr,
                                  std::atomic<int> *future_count_ptr,
                                  std::vector<FullBoard<BOARD_LEN>>& Boards,
                                  int thread_index) {
        FullBoard<BOARD_LEN> board;
        board.Init();
        Force ChessColor = full_board.getChessColor();
        Force OppoColor = full_board.getOppoColor();
        int OppoNum = full_board.getOppoNum();
        int OppoNumKnow0 = full_board.getOppoNumKnow();
        PositionIndex jieIndex = (full_board.getjiex()-1)*9+full_board.getjiey()-1;
        while(*history_count_ptr < history_count_per_move_) {
                board.Copy(full_board);
                mutex_.lock();
                (*history_count_ptr)++;
                int per = (*history_count_ptr)*100/history_count_per_move_;
                con.SetCurrentPercent(per);
                cout << "cnt: " << (*history_count_ptr) << endl;
                mutex_.unlock();
                int OppoNumKnow = OppoNumKnow0;
                auto vector = board.PlayableIndexes(OppoColor);
                while(OppoNumKnow < OppoNum) {
                        int size = vector.size();
                        if(size == 0)
                                break;
                        int num;
                        srand(rand()*(int)time(0)*17+rand());
                        num = (int)(size)*double(rand())/(RAND_MAX+1.0);
                        PositionIndex next = vector.at(num);
                        if(next == jieIndex) {
                                auto it = vector.begin()+num;
                                vector.erase(it);
                                continue;
                        }
                        if(board.JudgeEye(next,ChessColor)) {
                                auto it = vector.begin()+num;
                                vector.erase(it);
                                continue;
                        }
                        if(board.CouldEat(Move(OppoColor,next)) == 0) {
                                board.PlayMove(Move(OppoColor,next));
                                auto it = vector.begin()+num;
                                vector.erase(it);
                                OppoNumKnow++;
                                mutex_.lock();
                                timesBoard[next]--;
                                mutex_.unlock();
                        }else{
                                auto it = vector.begin()+num;
                                vector.erase(it);
                        }
                }
                board.setLastForce(OppoColor);
                if(jieIndex >= 0) {
                        board.setKoIndex(jieIndex);
                }
                SearchAndModifyNodes(board,future_count_ptr,thread_index);           //UCT
                if(ERROR == 1)
                        return;
                mutex_.lock();
                Boards.push_back(board);
                mutex_.unlock();
                *future_count_ptr = 0;
        }
}

template<BoardLen BOARD_LEN>
void UctPlayer<BOARD_LEN>::SearchAndModifyNodes(
        const FullBoard<BOARD_LEN> &full_board,
        std::atomic<int> *future_count_ptr,
        int thread_index)  {
        int future_count_ = 0;
        while (future_count_++ < future_count_per_move_) {
                PositionIndex max_ucb_index = MaxUcbChild(full_board, thread_index);
                if(max_ucb_index < 0 || max_ucb_index > 80) {
                        ERROR = 1;
                        return;
                }
                FullBoard<BOARD_LEN> max_ucb_child;
                max_ucb_child.Copy(full_board);
                Play(&max_ucb_child, max_ucb_index);
                ModifyAverageProfitAndReturnNewProfit(&max_ucb_child, future_count_ptr,
                                                      thread_index);
        }
}

template<BoardLen BOARD_LEN>
PositionIndex UctPlayer<BOARD_LEN>::MaxUcbChild(
        const FullBoard<BOARD_LEN> &full_board,
        int thread_index) {
        std::lock_guard<std::mutex> lock(mutex_);
        Force current_force = NextForce(full_board);
        auto playable_index_vector = full_board.PlayableIndexes(current_force);
        for(int i = 0; i < 8; i++){
                if(ceng[i] > 400){
                        ERROR = 1;
                        return -1;
                }
        }
        if(playable_index_vector.empty()) {
                return -1.0f;
        }
        int visited_count_sum = 0;
        std::vector<PositionIndex> null_indexes;
        for (PositionIndex position_index : playable_index_vector) {
                const NodeRecord *node_record_ptr = transposition_table_.GetChild(
                        full_board, position_index);
                if (node_record_ptr == nullptr) {
                        null_indexes.push_back(position_index);
                } else if (null_indexes.empty()) {
                        visited_count_sum += node_record_ptr->GetVisitedTime();
                }
        }

        if (!null_indexes.empty()) {
                int index = thread_index % thread_count_;
                if (index < null_indexes.size()) {
                        return null_indexes.at(index);
                }
                return null_indexes.at(index < null_indexes.size() ? index : 0);
        }
        float max_ucb = -1.0f;
        PositionIndex max_ucb_index = POSITION_INDEX_PASS;
        for (PositionIndex position_index : playable_index_vector) {
                const NodeRecord *node_record_ptr = transposition_table_.GetChild(
                        full_board, position_index);
                if (node_record_ptr == nullptr || node_record_ptr->IsInSearch()) {
                        continue;
                }
                float ucb = Ucb(*node_record_ptr, visited_count_sum);
                if (ucb > max_ucb
                    && !full_board.IsSuicide(
                            Move(NextForce(full_board), position_index))) {
                        max_ucb = ucb;
                        max_ucb_index = position_index;
                }
        }
        return max_ucb_index;
}

template<BoardLen BOARD_LEN>
float UctPlayer<BOARD_LEN>::ModifyAverageProfitAndReturnNewProfit(
        FullBoard<BOARD_LEN> *full_board_ptr,
        std::atomic<int> *future_count_ptr,
        int thread_index) {
        for(int i = 0; i < 8; i++){
                if(thread_count_ == i){
                        mutex_.lock();
                        ceng[i]++;
                        mutex_.unlock();
                }
        }
        float new_profit;
        NodeRecord *node_record_ptr = transposition_table_.Get(*full_board_ptr);
        if (node_record_ptr == nullptr) {
                MonteCarloGame<BOARD_LEN> monte_carlo_game(*full_board_ptr, seed_);
                if (!full_board_ptr->IsEnd()) {
                        monte_carlo_game.Run();
                }
                ++(*future_count_ptr);
                Force force = full_board_ptr->LastForce();
                new_profit = GetRegionRatio(monte_carlo_game.GetFullBoard(), force);
                NodeRecord node_record(1, new_profit, false);
                transposition_table_.Insert(*full_board_ptr, node_record);
        } else {
                mutex_.lock();
                node_record_ptr->SetIsInSearch(true);
                mutex_.unlock();
                if (full_board_ptr->IsEnd()) {
                        ++(*future_count_ptr);
                        new_profit = node_record_ptr->GetAverageProfit();
                } else {
                        if (full_board_ptr->PlayableIndexes(NextForce(*full_board_ptr))
                            .empty()) {
                                full_board_ptr->Pass(NextForce(*full_board_ptr));
                        } else {
                                PositionIndex max_ucb_index = MaxUcbChild(*full_board_ptr,
                                                                          thread_index);
                                if(ERROR == 1)
                                        return 0;
                                Play(full_board_ptr, max_ucb_index);
                        }
                        if(ERROR == 1)
                                return 0;
                        new_profit = 1.0f - ModifyAverageProfitAndReturnNewProfit(full_board_ptr,
                                                                                  future_count_ptr,
                                                                                  thread_index);
                        if(ERROR == 1)
                                return 0;
                        float previous_profit = node_record_ptr->GetAverageProfit();
                        float modified_profit = (previous_profit
                                                 * node_record_ptr->GetVisitedTime() + new_profit)
                                                / (node_record_ptr->GetVisitedTime() + 1);
                        node_record_ptr->SetAverageProfit(modified_profit);
                }

                node_record_ptr->SetVisitedTimes(node_record_ptr->GetVisitedTime() + 1);
        }

        if (node_record_ptr != nullptr) {
                std::lock_guard<std::mutex> lock(mutex_);
                node_record_ptr->SetIsInSearch(false);
        }
        for(int i = 0; i < 8; i++){
                if(thread_count_ == i){
                        mutex_.lock();
                        ceng[i]--;
                        mutex_.unlock();
                }
        }
        return new_profit;
}

template<BoardLen BOARD_LEN>
void UctPlayer<BOARD_LEN>::BestChild(
        const FullBoard<BOARD_LEN> &full_board) {
        Force force = NextForce(full_board);
        auto playable_index_vector = full_board.PlayableIndexes(force);

        for (PositionIndex index : playable_index_vector) {
                const NodeRecord *node_record = transposition_table_.GetChild(full_board,
                                                                              index);
                if(node_record == nullptr)
                        continue;
                markBoard[index] += node_record->GetVisitedTime();
        }
}

}

#endif /* FOOLGO_SRC_PLAYER_UCT_PLAYER_H_ */
