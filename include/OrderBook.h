#ifndef ORDER_BOOK_ORDERBOOK_H
#define ORDER_BOOK_ORDERBOOK_H

/**
 * @file    OrderBook.h
 * @brief   Header of application creating Order Book of any action type (bid or ask)
 * @author  Marcin Piwowar
 * @mail    m.piwowar2@gmail.com
 */

#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include "Order.h"

/// @namespace "quant" for Quant Sky dedicated solution
namespace quant {
    /**
     * @brief Two types of heap used as types to create Order Book got bid or ask
     * @param bidHeap -> heap for bids where the highest value has the highest priority
     * @param askHeap -> heap for asks where the lowest value has the highest priority
     */
    using bidHeap = std::priority_queue<uint32_t>;
    using askHeap = std::priority_queue<uint32_t, std::vector<uint32_t>, std::greater<>>;

    /// @brief Class creating Order Book with one of types of heap
    template<typename heap>
    class OrderBook {
    public:
        OrderBook() = default;
        OrderBook(const OrderBook&) = default;
        OrderBook& operator=(const OrderBook&) = default;
        OrderBook(OrderBook&&) noexcept = default;
        OrderBook& operator=(OrderBook&&) noexcept = default;
        ~OrderBook() = default;

        /**
         * @brief Add price - only unique values are stored
         * @param price - any value of type uint32_t (bid or ask)
         */
        void addPrice(uint32_t price);

        /**
         * @brief Method to remove price - once removed price is not available, because only uniques are stored
         * @param price - any value of type uint32_t (bid or ask)
         */
        void popPrice(uint32_t price);

        /// @brief Return top of heap
        uint32_t bestPrice();

        /// @brief Remove top of heap
        void popBestPrice();

        /**
         * @brief Add Order type to map with given key
         * @param orderID key to map
         * @param order value to key
         */
        void addOrder(uint64_t orderID, Order&& order);

        /**
         * @brief Remove Order from a map
         * @param orderID key to remove a value
         */
        void popOrder(uint64_t orderID);

        /**
         * @brief Add OrderID to the unique set of orders with same price
         * @param price key to map of sets
         * @param orderID value to store in unique set of orders with same price (key)
         */
        void addOrderToGroup(uint32_t price, uint64_t orderID);

        /**
         * @brief Remove OrderID from set
         * @param price key to set of OrderIDs
         * @param orderID value to remove
         */
        void popOrderFromGroup(uint32_t price, uint64_t orderID);

        /**
         * @brief Returning number of Orders in Order Book related to given price
         * @param price Given price to check (bid/ask)
         */
        uint32_t noOrders(uint32_t price);

        /**
         * @brief Returning number of all shares (Qty) in Order Book related to given price
         * @param price Given price to check (bid/ask)
         */
        uint32_t noShares(uint32_t price);

        /// @brief clear all poles in Order Book
        void clearAll();

        /// @brief Checking if there is any price in Order Book (we store only unique prices)
        bool isAnyPrice();

    private:
        /// @brief Heap of one of the given types - store prices with declared order
        heap _heap;
        /// @brief Set to store only unique prices, which are stored in _heap too
        std::unordered_set<uint32_t> _uniquePrices;
        /// @brief Map to store Orders where Order ID is a key
        std::unordered_map<uint64_t, Order> _orders;
        /// @brief Map to store set of OrderIDs with same price
        std::unordered_map<uint32_t, std::unordered_set<uint64_t>> _groupOrders;
    };
} // quant

#endif //ORDER_BOOK_ORDERBOOK_H
