/**
 * @file    Order.cpp
 * @brief   Source code of class storing dedicated values of Order in Order Book
 * @author  Marcin Piwowar
 * @mail    m.piwowar2@gmail.com
 */

#include "Order.h"

namespace quant {
    Order::Order(uint32_t qty) : qty{qty} {};

    uint32_t Order::getQty() const {
        return qty;
    };
} // quant