/**
 * @file    MsgReader.cpp
 * @brief   Part of application responsible for reading input files and run tick of Order Book after each line
 * @author  Marcin Piwowar
 * @mail    m.piwowar2@gmail.com
 */

#include <arpa/inet.h>
#include <chrono>
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
    void processClear(OrderBook<bidHeap>& bidOrderBook, OrderBook<askHeap>& askOrderBook, Pattern& tick) {
        bidOrderBook.clearAll();
        askOrderBook.clearAll();
    }

    template<typename heap>
    void processAdd(OrderBook<heap>& orderBook, Pattern& tick) {
        orderBook.addOrderToGroup(tick.Price, tick.OrderId);
        orderBook.addOrder(tick.OrderId, Order{tick.Qty});
        orderBook.addPrice(tick.Price);
    }

    /// @comment From delivered instruction and implemented logic - modify process is exactly same like @fn processAdd
    template<typename heap>
    void processModify(OrderBook<heap>& orderBook, Pattern& tick) {
        orderBook.addOrderToGroup(tick.Price, tick.OrderId);
        orderBook.addOrder(tick.OrderId, Order{tick.Qty});
        orderBook.addPrice(tick.Price);
    }

    template<typename heap>
    void processRemove(OrderBook<heap>& orderBook, Pattern& tick) {
        orderBook.popOrderFromGroup(tick.Price, tick.OrderId);  // Removes price if it's needed
        orderBook.popOrder(tick.OrderId);
    }

    template<typename bidHeap, typename askHeap>
    void processTick(OrderBook<bidHeap>& bidOrderBook, OrderBook<askHeap>& askOrderBook, Pattern& tick) {
        if (bidOrderBook.isAnyPrice()) {
            uint32_t bestPrice = bidOrderBook.bestPrice();
            tick.B0 = std::to_string(bestPrice);
            tick.BQ0 = std::to_string(bidOrderBook.noShares(bestPrice));
            tick.BN0 = std::to_string(bidOrderBook.noOrders(bestPrice));
        }
        else {
            tick.B0 = "";
            tick.BQ0 = "";
            tick.BN0 = "";
        }
        if (askOrderBook.isAnyPrice()) {
            uint32_t bestPrice = askOrderBook.bestPrice();
            tick.A0 = std::to_string(bestPrice);
            tick.AQ0 = std::to_string(askOrderBook.noShares(bestPrice));
            tick.AN0 = std::to_string(askOrderBook.noOrders(bestPrice));
        }
        else {
            tick.A0 = "";
            tick.AQ0 = "";
            tick.AN0 = "";
        }
    }

    template<typename bidHeap, typename askHeap>
    static void runActions(OrderBook<bidHeap>& bidOrderBook, OrderBook<askHeap>& askOrderBook, Pattern& tick) {
        switch(tick.Action) {
            case Action::clear1:
                processClear<bidHeap, askHeap>(bidOrderBook, askOrderBook, tick);
                processTick<bidHeap, askHeap>(bidOrderBook, askOrderBook, tick);
                break;
            case Action::clear2:
                processClear<bidHeap, askHeap>(bidOrderBook, askOrderBook, tick);
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
        std::string csvStr = (std::to_string(tick.SourceTime) + ";");
        if (Side::ask != tick.Side && Side::bid != tick.Side) csvStr += ";";
        else {
            csvStr += tick.Side;
            csvStr += ";";
        }
        csvStr += tick.Action;
        csvStr += ";";
        csvStr += std::to_string(tick.OrderId) + ";";
        csvStr += std::to_string(tick.Price) + ";";
        csvStr += std::to_string(tick.Qty) + ";";
        csvStr += tick.B0 + ';';
        csvStr += tick.BQ0 + ';';
        csvStr += tick.BN0 + ';';
        csvStr += tick.A0 + ';';
        csvStr += tick.AQ0 + ';';
        csvStr += tick.AN0 + '\n';
        return csvStr;
    }

    void MsgReader::read() {
        std::ifstream file(INPUT_FILE, std::ios::out | std::ios::binary);
        std::ofstream csvFile(OUTPUT_FILE);

        OrderBook<bidHeap> bidOrderBook;
        OrderBook<askHeap> askOrderBook;

        // Write header to CSV file
        csvFile << "SourceTime;Side;Action;OrderId;Price;Qty;B0;BQ0;BN0;A0;AQ0;AN0\n";

        // Total duration of all ticks
        std::chrono::microseconds totalDuration(0);

        while (file.peek() != EOF) {
            Pattern tick;
            readBinaryFile(file, tick);

            // Run tick for build Order Book
            auto start = std::chrono::high_resolution_clock::now();
            runActions<bidHeap, askHeap>(bidOrderBook, askOrderBook, tick);
            auto end = std::chrono::high_resolution_clock::now();

            // Printing on console time of tick
            auto tickDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            std::cout << tickDuration.count() << " us" << std::endl;
            totalDuration += tickDuration;

            // Print OB to file
            csvFile << printCSV(tick);
        }
        file.close();
        csvFile.close();

        std::cout << "Total time of building OB: " << totalDuration.count() << " us" << std::endl;
    }
} // quant