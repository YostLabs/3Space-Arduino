/**
 * @ Description:
 * C++ base wrapper around the C TSS_Com_Class interface.
 *
 * Derive from tss::ComClass and implement the required pure-virtual methods
 * to create a communication backend. Every slot in TSS_Com_Class_API has a
 * corresponding virtual method here, so any of them can be overridden
 * directly for full manual control.
 *
 * For a convenient default implementation of peek/length/read_until/clear
 * and buffered writes (mirroring struct TSS_Managed_Com_Class), derive from
 * tss::ManagedComClass (see tss/cpp/com/managed_com_class.hpp) instead, which
 * itself derives from ComClass and only requires implementing a smaller set
 * of "*Impl" hooks for the actual hardware operations.
 *
 * Usage:
 * @code
 * class MyComClass : public tss::ComClass {
 * public:
 *     int open() override { ... }
 *     int close() override { ... }
 *     int read(size_t num_bytes, uint8_t *out) override { ... }
 *     int write(const uint8_t *bytes, size_t len) override { ... }
 *     void setTimeout(uint32_t timeout_ms) override { ... }
 *     uint32_t getTimeout() override { ... }
 * };
 *
 * MyComClass com;
 * tssCreateSensor(&sensor, com); // implicit conversion to TSS_Com_Class*
 * @endcode
 */

#ifndef __TSS_CPP_COM_CLASS_HPP__
#define __TSS_CPP_COM_CLASS_HPP__

#include "tss/com/com_class.h"
#include "tss/com/managed_com.h"
#include "tss/sys/config.h"

#include <stddef.h>
#include <stdint.h>

namespace tss {

class ComClass {
public:
    /**
     * @param supports_reenumeration Set to true if this class implements
     * reenumerate()/autoDetect(). Mirrors the `reenumerates` field on the
     * underlying TSS_Com_Class. Cannot be changed later; a derived class
     * must forward the desired value up through its own constructor since
     * virtual dispatch is not available while the base class is constructing.
     */
    explicit ComClass(bool supports_reenumeration = false);
    virtual ~ComClass() = default;

    // m_com stores a pointer back to `this`, so instances of this class (and
    // anything derived from it) cannot be copied or moved.
    ComClass(const ComClass &) = delete;
    ComClass &operator=(const ComClass &) = delete;
    ComClass(ComClass &&) = delete;
    ComClass &operator=(ComClass &&) = delete;

    /**
     * @brief Implicitly converts to a TSS_Com_Class* so this object can be
     * passed directly anywhere a com class is expected, e.g.
     * tssCreateSensor(&sensor, com).
     */
    operator struct TSS_Com_Class *();

    //-----------------------------------------------------------------
    // Required overrides - the transport primitives.
    //-----------------------------------------------------------------

    /// Opens/initializes the communication object. Return 0 on success.
    virtual int open() = 0;

    /// Closes/deinitializes the communication object. Return 0 on success.
    virtual int close() = 0;

    /**
     * @brief Reads up to num_bytes, blocking up to the timeout set via
     * setTimeout(). Return the number of bytes actually read (which may be
     * less than num_bytes on timeout), or a negative TSS error code on an
     * unrecoverable hardware error.
     */
    virtual int read(size_t num_bytes, uint8_t *out) = 0;

    /**
     * @brief Sends len bytes out. Return 0 on success, non-zero on error.
     * @note If len==0, bytes may be null; simply return success.
     */
    virtual int write(const uint8_t *bytes, size_t len) = 0;

    /// Sets the timeout, in milliseconds, used by read(). 0 = non-blocking.
    virtual void setTimeout(uint32_t timeout_ms) = 0;

    /// Returns the current timeout, in milliseconds, used by read().
    /// @note Not const: querying the timeout on the real API is just as
    /// free to mutate internal state as read()/peek()/length() are.
    virtual uint32_t getTimeout() = 0;

    //-----------------------------------------------------------------
    // Optional overrides. Default implementations reuse the exported
    // tssManagedComBase* helpers, which operate byte-at-a-time on top of
    // whatever read()/getTimeout()/setTimeout() this instance provides -
    // these are correct (if not maximally efficient) even without any
    // buffering, and automatically improve if read()/etc. are overridden
    // (e.g. by tss::ManagedComClass) to be buffering-aware.
    //-----------------------------------------------------------------

    virtual int readUntil(uint8_t value, uint8_t *out, size_t size);
    virtual void clearImmediate();
    virtual void clearTimeout(uint32_t timeout_ms);

    //-----------------------------------------------------------------
    // Optional overrides - peeking. No generic default is possible without
    // a buffer, so the base implementation simply reports "unsupported".
    //-----------------------------------------------------------------

#if !(TSS_MINIMAL_SENSOR)
    virtual int peek(size_t start, size_t num_bytes, uint8_t *out);
    virtual int peekUntil(size_t start, uint8_t value, uint8_t *out, size_t size);
    virtual size_t peekCapacity();
    virtual size_t length();
#endif

    //-----------------------------------------------------------------
    // Optional overrides - buffered writes. Default is a no-op, since the
    // default write() above sends immediately and needs no bracketing.
    //-----------------------------------------------------------------

#if TSS_BUFFERED_WRITES
    virtual int beginWrite();
    virtual int endWrite();
#endif

    //-----------------------------------------------------------------
    // Optional overrides - device (re)discovery.
    // Default implementation reports the operation as unsupported.
    //-----------------------------------------------------------------

    virtual int reenumerate(TssComAutoDetectCallback cb, void *detect_data);
    virtual int autoDetect(TssComAutoDetectCallback cb, void *detect_data);

private:
    // TSS_Com_Class_API vtable shared by every ComClass instance. Every
    // entry forwards 1:1 into the calling instance's matching virtual method.
    static const struct TSS_Com_Class_API s_api;

    static ComClass *owner(struct TSS_Com_Class *com);

    static int trampolineOpen(struct TSS_Com_Class *com);
    static int trampolineClose(struct TSS_Com_Class *com);
    static int trampolineRead(struct TSS_Com_Class *com, size_t num_bytes, uint8_t *out);
    static int trampolineWrite(struct TSS_Com_Class *com, const uint8_t *bytes, size_t len);
    static int trampolineReadUntil(struct TSS_Com_Class *com, uint8_t value, uint8_t *out, size_t size);
    static void trampolineSetTimeout(struct TSS_Com_Class *com, uint32_t timeout_ms);
    static uint32_t trampolineGetTimeout(struct TSS_Com_Class *com);
    static void trampolineClearImmediate(struct TSS_Com_Class *com);
    static void trampolineClearTimeout(struct TSS_Com_Class *com, uint32_t timeout_ms);
#if !(TSS_MINIMAL_SENSOR)
    static int trampolinePeek(struct TSS_Com_Class *com, size_t start, size_t num_bytes, uint8_t *out);
    static int trampolinePeekUntil(struct TSS_Com_Class *com, size_t start, uint8_t value, uint8_t *out, size_t size);
    static size_t trampolinePeekCapacity(struct TSS_Com_Class *com);
    static size_t trampolineLength(struct TSS_Com_Class *com);
#endif
#if TSS_BUFFERED_WRITES
    static int trampolineBeginWrite(struct TSS_Com_Class *com);
    static int trampolineEndWrite(struct TSS_Com_Class *com);
#endif
    static int trampolineReenumerate(struct TSS_Com_Class *com, TssComAutoDetectCallback cb, void *detect_data);
    static int trampolineAutoDetect(struct TSS_Com_Class *out, TssComAutoDetectCallback cb, void *detect_data);

    // The com object exposed to the rest of the API via
    // operator TSS_Com_Class*(). Its `.context` is `this`, and its `.api` is
    // s_api, whose trampolines forward to this instance's virtual methods.
    struct TSS_Context_Com_Class m_com;
};

} // namespace tss

#endif /* __TSS_CPP_COM_CLASS_HPP__ */
