/**
 * @file    MsgReader.cpp
 * @brief   Part of application responsible for reading input files and run tick of Order Book after each line
 * @author  Marcin Piwowar
 * @mail    m.piwowar2@gmail.com
 */

#include <arpa/inet.h>
#include <chrono>
#include <deque>
#include <fstream>
#include <iostream>
#include <string>

#include "MsgReader.h"

namespace quant {
    void readBinaryFile(std::ifstream& inputFile, Pattern& tick) {
        inputFile.read(reinterpret_cast<char *>(&tick.SourceTime), sizeof(tick.SourceTime));
        tick.SourceTime = be64toh(tick.SourceTime);
        inputFile.read(reinterpret_cast<char *>(&tick.Side), sizeof(tick.Side));
        inputFile.read(reinterpret_cast<char *>(&tick.Action), sizeof(tick.Action));
        inputFile.read(reinterpret_cast<char *>(&tick.OrderId), sizeof(tick.OrderId));
        tick.OrderId = be64toh(tick.OrderId);
        inputFile.read(reinterpret_cast<char *>(&tick.Price), sizeof(tick.Price));
        tick.Price = ntohl(tick.Price);
        inputFile.read(reinterpret_cast<char *>(&tick.Qty), sizeof(tick.Qty));
        tick.Qty = ntohl(tick.Qty);
    }

    template<typename bidHeap, typename askHeap>
    void processClear(OrderBook<bidHeap>& bidOrderBook, OrderBook<askHeap>& askOrderBook) {
        bidOrderBook.clearAll();
        askOrderBook.clearAll();
    }

    template<typename heap>
    void processAdd(OrderBook<heap>& orderBook, Pattern& tick) {
        orderBook.addOrder(tick.OrderId, tick.Qty);
        orderBook.addOrderToGroup(tick.Price, tick.OrderId);    // Adding unique Price
    }

    /// @comment From delivered instruction and implemented logic - modify process is exactly same like @fn processAdd
    template<typename heap>
    void processModify(OrderBook<heap>& orderBook, Pattern& tick) {
        orderBook.addOrder(tick.OrderId, tick.Qty);
        orderBook.addOrderToGroup(tick.Price, tick.OrderId);    // Adding unique Price
    }

    template<typename heap>
    void processRemove(OrderBook<heap>& orderBook, Pattern& tick) {
        orderBook.popOrder(tick.OrderId);
        orderBook.popOrderFromGroup(tick.Price, tick.OrderId);  // Removes price if it's needed
    }

    template<typename bidHeap, typename askHeap>
    void processTick(OrderBook<bidHeap>& bidOrderBook, OrderBook<askHeap>& askOrderBook, Pattern& tick) {
        if (bidOrderBook.isAnyPrice()) {
            uint32_t bestPrice = bidOrderBook.bestPrice();
            tick.B0 = bestPrice;
            tick.BQ0 = bidOrderBook.getBestShares();
            tick.BN0 = bidOrderBook.getBestOrders();
        }
        if (askOrderBook.isAnyPrice()) {
            uint32_t bestPrice = askOrderBook.bestPrice();
            tick.A0 = bestPrice;
            tick.AQ0 = askOrderBook.getBestShares();
            tick.AN0 = askOrderBook.getBestOrders();
        }
    }

    template<typename bidHeap, typename askHeap>
    static void runActions(OrderBook<bidHeap>& bidOrderBook, OrderBook<askHeap>& askOrderBook, Pattern& tick) {
        switch(tick.Action) {
            case Action::clear1:
                processClear<bidHeap, askHeap>(bidOrderBook, askOrderBook);
                processTick<bidHeap, askHeap>(bidOrderBook, askOrderBook, tick);
                break;
            case Action::clear2:
                processClear<bidHeap, askHeap>(bidOrderBook, askOrderBook);
                processTick<bidHeap, askHeap>(bidOrderBook, askOrderBook, tick);
                break;
            case Action::add:
                if (Side::bid == tick.Side) processAdd<bidHeap>(bidOrderBook, tick);
                if (Side::ask == tick.Side) processAdd<askHeap>(askOrderBook, tick);
                processTick<bidHeap, askHeap>(bidOrderBook, askOrderBook, tick);
                break;
            case Action::modify:
                if (Side::bid == tick.Side) processModify<bidHeap>(bidOrderBook, tick);
                if (Side::ask == tick.Side) processModify<askHeap>(askOrderBook, tick);
                processTick<bidHeap, askHeap>(bidOrderBook, askOrderBook, tick);
                break;
            case Action::remove:
                if (Side::bid == tick.Side) processRemove<bidHeap>(bidOrderBook, tick);
                if (Side::ask == tick.Side) processRemove<askHeap>(askOrderBook, tick);
                processTick<bidHeap, askHeap>(bidOrderBook, askOrderBook, tick);
                break;
            default:
                break;
        }
    }

    std::string printCSV(Pattern& tick) {
        std::string csvStr = (std::to_string(tick.SourceTime) + ';');
        if (Side::ask != tick.Side && Side::bid != tick.Side) csvStr += ';';
        else {
            csvStr += tick.Side;
            csvStr += ';';
        }
        csvStr += tick.Action;
        csvStr += ';';
        csvStr += std::to_string(tick.OrderId) + ';';
        csvStr += std::to_string(tick.Price) + ';';
        csvStr += std::to_string(tick.Qty) + ';';
        if (tick.B0 != UINT32_MAX) csvStr += std::to_string(tick.B0) + ';';
        else csvStr += ';';
        if (tick.BQ0 != UINT32_MAX) csvStr += std::to_string(tick.BQ0) + ';';
        else csvStr += ';';
        if (tick.BN0 != UINT32_MAX) csvStr += std::to_string(tick.BN0) + ';';
        else csvStr += ';';
        if (tick.A0 != UINT32_MAX) csvStr += std::to_string(tick.A0) + ';';
        else csvStr += ';';
        if (tick.AQ0 != UINT32_MAX) csvStr += std::to_string(tick.AQ0) + ';';
        else csvStr += ';';
        if (tick.AN0 != UINT32_MAX) csvStr += std::to_string(tick.AN0) + '\n';
        else csvStr += '\n';
        return csvStr;
    }

    void MsgReader::read() {
        // Phase I - read file
        std::deque<Pattern> ticks;
        std::ifstream file(INPUT_FILE, std::ios::out | std::ios::binary);
        while (file.peek() != EOF) {
            Pattern tick;
            readBinaryFile(file, tick);
            ticks.push_back(std::move(tick));
        }
        file.close();

        // Phase II - create OB
        OrderBook<bidHeap> bidOrderBook;
        OrderBook<askHeap> askOrderBook;
        auto start = std::chrono::high_resolution_clock::now();
        for (Pattern& tick : ticks) {
            runActions<bidHeap, askHeap>(bidOrderBook, askOrderBook, tick);
        }
        auto end = std::chrono::high_resolution_clock::now();

        // Phase III - write output to file
        std::ofstream csvFile(OUTPUT_FILE);
        csvFile << "SourceTime;Side;Action;OrderId;Price;Qty;B0;BQ0;BN0;A0;AQ0;AN0\n";
        for (Pattern& tick: ticks) {
            csvFile << printCSV(tick);
        }
        csvFile.close();

        // Printing on console time of building OB
        auto tickDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Total time of building OB: " << tickDuration.count() << " us" << std::endl;
        std::cout << "Avg time per tick: "
        << (static_cast<double_t>(tickDuration.count()) / static_cast<double_t>(ticks.size()))
        << " us" << std::endl;
    }
} // quant