// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2017 The Bitcoin Core developers
// Copyright (c) 2017-2019 The Gemma Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef RAVEN_WALLET_FEES_H
#define RAVEN_WALLET_FEES_H

#include "amount.h"

class CBlockPolicyEstimator;
class CCoinControl;
class CFeeRate;
class CTxMemPool;
struct FeeCalculation;

/**
 * Return the minimum required fee taking into account the
 * floating relay fee and user set minimum transaction fee
 */
CAmount GetRequiredFee(unsigned int nTxBytes);

/**
 * Estimate the minimum fee considering user set parameters
 * and the required fee
 */
CAmount GetMinimumFee(unsigned int nTxBytes, const CCoinControl& coin_control, const CTxMemPool& pool, const CBlockPolicyEstimator& estimator, FeeCalculation *feeCalc);

/**
 * Return the maximum feerate for discarding change.
 */
CFeeRate GetDiscardRate(const CBlockPolicyEstimator& estimator);

#endif // RAVEN_WALLET_FEES_H
