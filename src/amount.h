// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2016 The Bitcoin Core developers
// Copyright (c) 2017-2019 The Gemma Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef RAVEN_AMOUNT_H
#define RAVEN_AMOUNT_H

#include <stdint.h>

/** Amount in corbies (Can be negative) */
typedef int64_t CAmount;

// 6 decimals
static const CAmount COIN = 1000000;
static const CAmount CENT = 10000;

/** No amount larger than this (in satoshi) is valid.
 *
 * Note that this constant is *not* the total money supply, which in Gemma
 * currently happens to be less than 840,000,000,000 GEMMA for various reasons, but
 * rather a sanity check. As this sanity check is used by consensus-critical
 * validation code, the exact value of the MAX_MONEY constant is consensus
 * critical; in unusual circumstances like a(nother) overflow bug that allowed
 * for the creation of coins out of thin air modification could lead to a fork.
 * */
static const CAmount MAX_MONEY = 840000000000LL * COIN; // 840 billion
inline bool MoneyRange(const CAmount& nValue) { return (nValue >= 0 && nValue <= MAX_MONEY); }

#endif //  RAVEN_AMOUNT_H
