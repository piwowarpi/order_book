/**
 * @file    OrderBook.cpp
 * @brief   Source code of application creating Order Book of any action type (bid or ask)
 * @author  Marcin Piwowar
 * @mail    m.piwowar2@gmail.com
 */

#include "OrderBook.h"

namespace quant {
    /// @comment Only unique prices are stored
    template<typename heap>
    void OrderBook<heap>::addPrice(uint32_t price) {
        if (_uniquePrices.find(price) == _uniquePrices.end()) {
            _heap.push(price);
            _uniquePrices.insert(price);
        }
    }

    template<typename heap>
    void OrderBook<heap>::popPrice(uint32_t price) {
        if (price != _heap.top()) {
            _uniquePrices.erase(price);
            _heap = heap(_uniquePrices.begin(), _uniquePrices.end());
        }
        else {
            popBestPrice();
        }
    }

    template<typename heap>
    uint32_t OrderBook<heap>::bestPrice() {
        return _heap.top();
    }

    template<typename heap>
    void OrderBook<heap>::popBestPrice() {
        _uniquePrices.erase(_heap.top());
        _heap.pop();
    }

    /// @comment If orderID already exist -> will be replace with new one
    template<typename heap>
    void OrderBook<heap>::addOrder(uint64_t oderID, Order&& order) {
        _orders[oderID] = order;
    }

    /// @comment If OrderID doesn't exist, removing will be ignore without exception
    template<typename heap>
    void OrderBook<heap>::popOrder(uint64_t oderID) {
        _orders.erase(oderID);
    }

    /// @comment If orderID already exist, will be ignore as only unique values are stored
    template<typename heap>
    void OrderBook<heap>::addOrderToGroup(uint32_t price, uint64_t orderID) {
        _groupOrders[price].insert(orderID);
    }

    /// @comment If OrderID doesn't exist, removing will be ignore without exception
    template<typename heap>
    void OrderBook<heap>::popOrderFromGroup(uint32_t price, uint64_t orderID) {
        _groupOrders[price].erase(orderID);
    }

    template<typename heap>
    uint32_t OrderBook<heap>::noOrders(uint32_t price) {
        return static_cast<uint32_t>(_groupOrders[price].size());
    }

    template<typename heap>
    uint32_t OrderBook<heap>::noShares(uint32_t price) {
        uint32_t shares = 0;
        for (const auto& item : _groupOrders[price]) {
            shares += _orders[item].getQty();
        }
        return shares;
    }

    template<typename heap>
    void OrderBook<heap>::clearAll() {
        _heap = heap();
        _uniquePrices.clear();
        _orders.clear();
        _groupOrders.clear();
    }

    template<typename heap>
    bool OrderBook<heap>::isAnyPrice() {
        return !_uniquePrices.empty();
    }

    /// @comment belows for correctness of linker process
    template class OrderBook<bidHeap>;
    template class OrderBook<askHeap>;
} // quant