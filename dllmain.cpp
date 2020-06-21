extern "C"
{
#include "coldclear.h"
}

using namespace System;

namespace ColdClear
{
    template<typename T, int N>
    array<T>^ ConvertCArray(T(&arr)[N])
    {
        array<T>^ result = gcnew array<T>(N);
        for (int i = 0; i < N; i++)
        {
            result[i] = arr[i];
        }
        return result;
    }

    template<typename T>
    array<T>^ ConvertCArray(T* arr, int N)
    {
        array<T>^ result = gcnew array<T>(N);
        for (int i = 0; i < N; i++)
        {
            result[i] = arr[i];
        }
        return result;
    }

    template<typename T, int N>
    void ConvertCSharpArray(array<T>^ src, T(&dest)[N])
    {
        if (src == nullptr)
        {
            throw gcnew ArgumentNullException();
        }
        if (src->Length != N)
        {
            throw gcnew ArgumentException("Length mismatch");
        }
        for (int i = 0; i < N; i++)
        {
            dest[i] = src[i];
        }
    }

    public enum class CCPiece
    {
        CC_I, CC_O, CC_T, CC_L, CC_J, CC_S, CC_Z, CC_None
    };

    public enum class CCTspinStatus
    {
        CC_NONE_TSPIN_STATUS,
        CC_MINI,
        CC_FULL,
    };

    public value class CCPieceBag
    {
    public:
        static CCPieceBag Empty()
        {
            return CCPieceBag(0);
        }
        static CCPieceBag Full()
        {
            return CCPieceBag(127);
        }
        bool Add(CCPiece piece)
        {
            int p = (int)piece;
            if (p >= 7) return false;
            int mask = 1 << p;
            if ((bag & mask) != 0) return false;
            bag |= mask;
            return true;
        }
        bool Remove(CCPiece piece)
        {
            int p = (int)piece;
            if (p >= 7) return false;
            int mask = 1 << p;
            if ((bag & mask) == 0) return false;
            bag ^= mask;
            return true;
        }

        uint8_t Get() { return bag; }
    private:
        CCPieceBag(int b) : bag(b) {}
        uint8_t bag;
    };

    public value struct Plan
    {
        CCPiece Piece;
        CCTspinStatus TSpin;

        /// <summary>
        /// Expected cell X coordinates of placement, (0, 0) being the bottom left.
        /// </summary>
        array<uint8_t>^ ExpectedX;

        /// <summary>
        /// Expected cell Y coordinates of placement, (0, 0) being the bottom left.
        /// </summary>
        array<uint8_t>^ ExpectedY;

        /// <summary>
        /// Expected lines that will be cleared after placement, with -1 indicating no line.
        /// </summary>
        array<int32_t>^ ClearedLines;
    };
    Plan ConvertPlan(CCPlanPlacement* p)
    {
        return Plan
        {
            (CCPiece)p->piece,
            (CCTspinStatus)p->tspin,
            ConvertCArray(p->expected_x),
            ConvertCArray(p->expected_y),
            ConvertCArray(p->cleared_lines)
        };
    }

    public value struct Move
    {
        /// <summary>
        /// Whether hold is required.
        /// </summary>
        bool Hold;

        /// <summary>
        /// Expected cell X coordinates of placement, (0, 0) being the bottom left.
        /// </summary>
        array<uint8_t>^ ExpectedX;
        /// <summary>
        /// Expected cell Y coordinates of placement, (0, 0) being the bottom left.
        /// </summary>
        array<uint8_t>^ ExpectedY;

        array<CCMovement>^ Movements;

        /// <summary>
        /// Bot info.
        /// </summary>
        uint32_t Nodes;

        /// <summary>
        /// Bot info.
        /// </summary>
        uint32_t Depth;

        /// <summary>
        /// Bot info.
        /// </summary>
        uint32_t OriginalRank;
    };
    Move ConvertMove(CCMove move)
    {
        return Move
        {
            move.hold,
            ConvertCArray(move.expected_x),
            ConvertCArray(move.expected_y),
            ConvertCArray(move.movements, move.movement_count),
            move.nodes,
            move.depth,
            move.original_rank
        };
    }

    public value struct MovePlans
    {
        Move Move;
        array<Plan>^ Plans;
    };

    public enum CCMovementMode
    {
        CC_0G,
        CC_20G,
        CC_HARD_DROP_ONLY
    };

    public value struct CCOptions
    {
        CCMovementMode mode;
        bool use_hold;
        bool speculate;
        bool pcloop;
        uint32_t min_nodes;
        uint32_t max_nodes;
        uint32_t threads;

        static CCOptions Default()
        {
            ::CCOptions options;
            cc_default_options(&options);
            return CCOptions
            {
                (CCMovementMode)options.mode,
                options.use_hold,
                options.speculate,
                options.pcloop,
                options.min_nodes,
                options.max_nodes,
                options.threads
            };
        }

        void ConvertToC(::CCOptions& options)
        {
            options.mode = (::CCMovementMode)mode;
            options.use_hold = use_hold;
            options.speculate = speculate;
            options.pcloop = pcloop;
            options.min_nodes = min_nodes;
            options.max_nodes = max_nodes;
            options.threads = threads;
        }
    };

    public value struct CCWeights
    {
        int32_t back_to_back;
        int32_t bumpiness;
        int32_t bumpiness_sq;
        int32_t height;
        int32_t top_half;
        int32_t top_quarter;
        int32_t jeopardy;
        int32_t cavity_cells;
        int32_t cavity_cells_sq;
        int32_t overhang_cells;
        int32_t overhang_cells_sq;
        int32_t covered_cells;
        int32_t covered_cells_sq;
        array<int32_t>^ tslot;
        int32_t well_depth;
        int32_t max_well_depth;
        array<int32_t>^ well_column;

        int32_t b2b_clear;
        int32_t clear1;
        int32_t clear2;
        int32_t clear3;
        int32_t clear4;
        int32_t tspin1;
        int32_t tspin2;
        int32_t tspin3;
        int32_t mini_tspin1;
        int32_t mini_tspin2;
        int32_t perfect_clear;
        int32_t combo_garbage;
        int32_t move_time;
        int32_t wasted_t;

        bool use_bag;

        static CCWeights Default()
        {
            ::CCWeights weights;
            cc_default_weights(&weights);
            return ConvertFromC(weights);
        }

        static CCWeights Fast()
        {
            ::CCWeights weights;
            cc_fast_weights(&weights);
            return ConvertFromC(weights);
        }

        static CCWeights ConvertFromC(::CCWeights weights)
        {
            return CCWeights
            {
                weights.back_to_back,
                weights.bumpiness,
                weights.bumpiness_sq,
                weights.height,
                weights.top_half,
                weights.top_quarter,
                weights.jeopardy,
                weights.cavity_cells,
                weights.cavity_cells_sq,
                weights.overhang_cells,
                weights.overhang_cells_sq,
                weights.covered_cells,
                weights.covered_cells_sq,
                ConvertCArray(weights.tslot),
                weights.well_depth,
                weights.max_well_depth,
                ConvertCArray(weights.well_column),

                weights.b2b_clear,
                weights.clear1,
                weights.clear2,
                weights.clear3,
                weights.clear4,
                weights.tspin1,
                weights.tspin2,
                weights.tspin3,
                weights.mini_tspin1,
                weights.mini_tspin2,
                weights.perfect_clear,
                weights.combo_garbage,
                weights.move_time,
                weights.wasted_t,
                weights.use_bag
            };
        }

        void ConvertToC(::CCWeights& weights)
        {
            weights.back_to_back = back_to_back;
            weights.bumpiness = bumpiness;
            weights.bumpiness_sq = bumpiness_sq;
            weights.height = height;
            weights.top_half = top_half;
            weights.top_quarter = top_quarter;
            weights.jeopardy = jeopardy;
            weights.cavity_cells = cavity_cells;
            weights.cavity_cells_sq = cavity_cells_sq;
            weights.overhang_cells = overhang_cells;
            weights.overhang_cells_sq = overhang_cells_sq;
            weights.covered_cells = covered_cells;
            weights.covered_cells_sq = covered_cells_sq;
            ConvertCSharpArray(tslot, weights.tslot);
            weights.well_depth = well_depth;
            weights.max_well_depth = max_well_depth;
            ConvertCSharpArray(well_column, weights.well_column);

            weights.b2b_clear = b2b_clear;
            weights.clear1 = clear1;
            weights.clear2 = clear2;
            weights.clear3 = clear3;
            weights.clear4 = clear4;
            weights.tspin1 = tspin1;
            weights.tspin2 = tspin2;
            weights.tspin3 = tspin3;
            weights.mini_tspin1 = mini_tspin1;
            weights.mini_tspin2 = mini_tspin2;
            weights.perfect_clear = perfect_clear;
            weights.combo_garbage = combo_garbage;
            weights.move_time = move_time;
            weights.wasted_t = wasted_t;

            weights.use_bag = use_bag;
        }
    };

    public ref class Bot
    {
    public:
        /// <summary>
        /// Launches a bot thread with a blank board, empty queue, and all seven pieces in the bag, using default
        /// options and weights.
        /// </summary>
        Bot()
        {
            ::CCOptions options;
            ::CCWeights weights;
            cc_default_options(&options);
            cc_default_weights(&weights);
            cc = cc_launch_async(&options, &weights);
        }

        /// <summary>
        /// Launches a bot thread with a blank board, empty queue, and all seven pieces in the bag, using the
        /// speicified options and weights.
        /// </summary>
        /// <param name="options">Options of CC.</param>
        /// <param name="weights">Weights of CC.</param>
        Bot(CCOptions options, CCWeights weights)
        {
            ::CCOptions coptions;
            ::CCWeights cweights;
            options.ConvertToC(coptions);
            weights.ConvertToC(cweights);
            cc = cc_launch_async(&coptions, &cweights);
        }

        /// <summary>
        /// Launches a bot thread with a predefined field, empty queue, remaining pieces in the bag, hold piece,
        /// back-to-back status, and combo count. This allows you to start CC from the middle of a game.
        /// </summary>
        /// <param name="options">Options of CC.</param>
        /// <param name="weights">Weights of CC.</param>
        /// <param name="field">An array of 400 booleans in row major order with index 0 being the bottom-left cell.</param>
        /// <param name="remain">Remaining pieces in the bag. This must match the next few pieces provided to CC via AddNextPiece.</param>
        /// <param name="hold">Hold piece. CC_None indicates no hold piece.</param>
        /// <param name="b2b">Back-to-Back status.</param>
        /// <param name="combo">The number of consecutive line clears achieved.</param>
        Bot(CCOptions options, CCWeights weights, array<bool>^ field, CCPieceBag remain, CCPiece hold, bool b2b, uint32_t combo)
        {
            ::CCOptions coptions;
            ::CCWeights cweights;
            options.ConvertToC(coptions);
            weights.ConvertToC(cweights);
            bool f[400];
            ConvertCSharpArray(field, f);

            if (hold == CCPiece::CC_None)
            {
                cc = cc_launch_with_board_async(&coptions, &cweights, f, remain.Get(), nullptr, b2b, combo);
            }
            else
            {
                ::CCPiece h = (::CCPiece)hold;
                cc = cc_launch_with_board_async(&coptions, &cweights, f, remain.Get(), &h, b2b, combo);
            }
        }

        ~Bot()
        {
            cc_destroy_async(cc);
        }

        /// <summary>
        /// Adds a new piece to the end of the queue.
        /// If speculation is enabled, the piece must be in the bag. For example, if you start a new
        /// game with starting sequence IJOZT, the first time you call this function you can only
        /// provide either an L or an S piece.
        /// </summary>
        /// <param name="p">Next piece</param>
        void AddNextPiece(CCPiece p)
        {
            if (p == CCPiece::CC_None)
                throw gcnew System::ArgumentException("CC_None is not allowed");
            cc_add_next_piece_async(cc, (::CCPiece)p);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="incoming">The number of lines of garbage the bot is expected to receive after placing the next piece.</param>
        /// <returns>The next move and the following plans.</returns>
        MovePlans GetNextMoveAndPlans(int incoming)
        {
            cc_request_next_move(cc, incoming);
            CCMove m;
            CCPlanPlacement plans[32];
            uint32_t len = 32;
            CCBotPollStatus status = cc_block_next_move(cc, &m, plans, &len);
            if (status == CC_BOT_DEAD)
                throw gcnew System::Exception("CC bot dead");

            MovePlans mp;
            mp.Move = ConvertMove(m);
            mp.Plans = gcnew array<Plan>(len);
            for (uint32_t i = 0; i < len; i++)
            {
                mp.Plans[i] = ConvertPlan(plans + i);
            }
            return mp;
        }

        /// <summary>
        /// Resets the playfield, back-to-back status, and combo count.
        /// This should only be used when garbage is received or when your client could not place the
        /// piece in the correct position for some reason (e.g. 15 move rule), since this forces the
        /// bot to throw away previous computations.
        /// </summary>
        /// <param name="field">An array of 400 booleans in row major order with index 0 being the bottom-left cell.</param>
        /// <param name="b2b">Back-to-Back status.</param>
        /// <param name="combo">The number of consecutive line clears achieved.</param>
        void Reset(array<bool>^ field, bool b2b, uint32_t combo)
        {
            bool f[400];
            ConvertCSharpArray(field, f);
            cc_reset_async(cc, f, b2b, combo);
        }
    private:
        CCAsyncBot* cc;
    };
}