#ifndef SHOP_NEXON_CASH_H
#define SHOP_NEXON_CASH_H

#include <cstdint>

// Nexon Cash Shop Integration Namespace
// Confidence: 75% - Complete class structure found
// Error count: 996
// Evidence: decl.h.backup_round2:13459-13492
namespace nexon {
namespace cash {

    // Binary buffer for cash shop protocol
    class ByteBuffer {
    public:
        ByteBuffer& operator<<(char *str);
        ByteBuffer& operator<<(wchar_t *wstr);
        ByteBuffer& operator<<(int16_t value);
        int length() const;

    private:
        char* buffer;
        int size;
        int capacity;
    };

    // Implementation class
    class NCashImpl {
    public:
        NCashImpl(char *host, int16_t port, uint8_t protocol, uint16_t flags, int16_t timeout);
        int reconnect();

        // Protocol helpers
        NCashImpl* set_data_header(int *data, char type, char flags);
        NCashImpl* get_data_header(uint8_t *type, uint8_t *flags, unsigned int *size, uint8_t *data);

        // Error handling
        int handle_cash_error(int errorCode);
        int handle_coupon_error(int errorCode);

        // Cash operations
        int checkEntrance(char *account, char *character);
        int getBalance(char *account, int *outBalance);
        int purchase(int itemId, int count, int price, int flags, char *account, char *character, uint8_t *result);
        int gift(int itemId, int count, int price, int flags, char *sender, char *receiver, char *message, char *title, uint8_t *result);
        int confirm(char type, int transactionId);
        int cancel(char type, int transactionId);
        int chargeCash(unsigned int amount, ...);

    private:
        char host[256];
        int16_t port;
        int socket;
        ByteBuffer sendBuf;
        ByteBuffer recvBuf;
    };

    // Public wrapper class
    class NexonCash {
    public:
        NexonCash(char *host, int16_t port, char protocol, uint16_t flags, int16_t timeout);
        ~NexonCash();

        int getBalance(char *account, int *outBalance);
        int purchase(int itemId, int count, int price, int flags, char *account, char *character, uint8_t *result);
        int gift(int itemId, int count, int price, int flags, char *sender, char *receiver, char *message, char *title, uint8_t *result);
        int confirm(int transactionId);
        int cancel(int transactionId);
        int checkCoupon();
        int useCoupon();
        int checkCafeCoupon();

    private:
        NCashImpl* impl;
    };

} // namespace cash
} // namespace nexon

#endif // SHOP_NEXON_CASH_H
