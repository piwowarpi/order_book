#ifndef ORDER_BOOK_MSGREADER_H
#define ORDER_BOOK_MSGREADER_H

/**
 * @file    MsgReader.h
 * @brief   Part of application responsible for reading input files and run tick of Order Book after each line
 * @author  Marcin Piwowar
 * @mail    m.piwowar2@gmail.com
 */

#include <cstdint>
#include <string>

#include "OrderBook.h"

/// @namespace "quant" for Quant Sky dedicated solution
namespace quant {
    /// @enum To cover pure digits 1 and 2 and better understanding if we check bid or ask
    enum Side : uint8_t { bid = '1', ask = '2' };

    /// @enum To cover pure characters and better understanding which action is processed
    enum Action : uint8_t { clear1 = 'Y', clear2 = 'F', add = 'A', modify = 'M', remove = 'D' };

    /// @struct Pattern for correct reading input binary files and store data per tick
    struct Pattern {
        uint64_t SourceTime;
        uint8_t Side;
        uint8_t Action;
        uint64_t OrderId;
        uint32_t Price;
        uint32_t Qty;
        std::string B0;
        std::string BQ0;
        std::string BN0;
        std::string A0;
        std::string AQ0;
        std::string AN0;
    };

     /**
      * @brief Function to reading binary file and store input data to Order (tick)
      * @param inputFile Binary file already open
      */
     void readBinaryFile(std::ifstream& inputFile, Pattern& tick);

    /**
     * @brief Dedicated steps for clearing Order Book action
     * @tparam bidHeap heap for bids where the highest value has the highest priority
     * @tparam askHeap heap for asks where the lowest value has the highest priority
     * @param bidOrderBook Order Book dedicated to bid prices and logic
     * @param askOrderBook Order Book dedicated to ask prices and logic
     * @param tick Struct to store data per tick
     */
    template<typename bidHeap, typename askHeap>
    static void processClear(OrderBook<bidHeap>& bidOrderBook, OrderBook<askHeap>& askOrderBook);

    /**
     * @brief Dedicated steps for adding Order (tick) to Order Book action
     * @tparam heap bidHeap or askHeap
     * @param orderBook Dedicated Order Book
     * @param tick Struct to store data per tick
     */
    template<typename heap>
    static void processAdd(OrderBook<heap>& orderBook, Pattern& tick);

    /**
     * @brief Dedicated steps for modifying Order (tick) inside Order Book action
     * @tparam heap bidHeap or askHeap
     * @param orderBook Dedicated Order Book
     * @param tick Struct to store data per tick
     */
    template<typename heap>
    static void processModify(OrderBook<heap>& orderBook, Pattern& tick);

    /**
     * @brief Dedicated steps for removing Order (tick) from Order Book action
     * @tparam heap bidHeap or askHeap
     * @param orderBook Dedicated Order Book
     * @param tick Struct to store data per tick
     */
    template<typename heap>
    static void processRemove(OrderBook<heap>& orderBook, Pattern& tick);

    /**
     * @brief Process to write data to given tick
     * @tparam bidHeap heap for bids where the highest value has the highest priority
     * @tparam askHeap heap for asks where the lowest value has the highest priority
     * @param bidOrderBook Order Book dedicated to bid prices and logic
     * @param askOrderBook Order Book dedicated to ask prices and logic
     * @param tick Struct to store data per tick
     */
    template<typename bidHeap, typename askHeap>
    static void processTick(OrderBook<bidHeap>& bidOrderBook, OrderBook<askHeap>& askOrderBook, Pattern& tick);

    /**
     * @brief Part of code responsible for run correct actions and run ticks for Order Book
     * @tparam bidHeap heap for bids where the highest value has the highest priority
     * @tparam askHeap heap for asks where the lowest value has the highest priority
     * @param bidOrderBook Order Book dedicated to bid prices and logic
     * @param askOrderBook Order Book dedicated to ask prices and logic
     * @param tick Struct to store data per tick
     */
    template<typename bidHeap, typename askHeap>
    static void runActions(OrderBook<bidHeap>& bidOrderBook, OrderBook<askHeap>& askOrderBook, Pattern& tick);

    /**
     * @brief Function responsible for return tick data to CSV file
     * @param tick Struct to store data per tick
     */
    std::string printCSV(Pattern& tick);

    /// @brief Class responsible for reading input files, processing ticks, and writing to output file
    class MsgReader {
    public:
        MsgReader() = default;
        MsgReader(const MsgReader&) = default;
        MsgReader& operator=(const MsgReader&) = default;
        MsgReader(MsgReader&&) noexcept = default;
        MsgReader& operator=(MsgReader&&) noexcept = default;
        ~MsgReader() = default;

        /// @brief Function realising task of class
        static void read();
    };
} // quant

#endif //ORDER_BOOK_MSGREADER_H
