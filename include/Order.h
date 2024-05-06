#ifndef ORDER_BOOK_ORDER_H
#define ORDER_BOOK_ORDER_H

/**
 * @file    Order.h
 * @brief   Header of class storing dedicated values of Order in Order Book
 * @author  Marcin Piwowar
 * @mail    m.piwowar2@gmail.com
 */

#include <cstdint>

/// @namespace "quant" for Quant Sky dedicated solution
namespace quant {
    /// @brief Class creating Order with dedicated values
    class Order {
    public:
        Order() = default;
        Order(const Order&) = default;
        Order& operator=(const Order&) = default;
        Order(Order&&) noexcept = default;
        Order& operator=(Order&&) noexcept = default;
        ~Order() = default;

        /// @brief Constructor storing necessary values of Order
        explicit Order(uint32_t qty);

        /// @brief Return Qty
        [[nodiscard]] uint32_t getQty() const;
    private:
        uint32_t qty;
    };
} // quant

#endif //ORDER_BOOK_ORDER_H
