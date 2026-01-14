#ifndef SHOP_TAIWAN_CASH_H
#define SHOP_TAIWAN_CASH_H

#include <ctime>

// Taiwan-specific cash shop packets
// Confidence: 60%
// Error count: 1,392
// Internal packet for Taiwan region cash transactions
struct TaiwanInternalPack {
    int packetType;     // Transaction type (0=purchase, 1=gift, 2=refund)
    int accountId;      // Account UID (m_id from accounts table)
    int amount;         // Cera amount involved
    int itemId;         // Purchased item ID (ipg_no)
    int quantity;       // Item quantity
    int transactionId;  // Unique transaction ID
    /* ORM: taiwan_billing database - billing_cera_log table */
};

// Prepayment information
// Confidence: 65%
// Error count: 912
// Prepaid Cera balance information
struct PrePayInfo {
    int accountId;      // Account UID
    int prepaidAmount;  // Prepaid Cera balance
    time_t expiryDate;  // Balance expiry date (0 = no expiry)
    int bonusPoints;    // Bonus points from promotions
    int lastChargeTime; // Last charge timestamp
    /* ORM: taiwan_billing.cera_account table */
};

// Restriction categories
// Confidence: 50%
// Error count: 648
// Purchase restriction by item category
struct RestrictCategory {
    int categoryId;     // Category ID (avatar, consumable, etc.)
    int restrictType;   // 0=none, 1=daily, 2=weekly, 3=monthly
    int maxDaily;       // Maximum daily purchases
    int maxWeekly;      // Maximum weekly purchases
    int maxMonthly;     // Maximum monthly purchases
    /* Integrates with shop_restriction.h for enforcement */
};

#endif // SHOP_TAIWAN_CASH_H
