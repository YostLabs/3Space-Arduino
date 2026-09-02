/**
 * @ Description:
 * C++ convenience layer adding peek/length/read_until/clear support (and
 * buffered writes, when TSS_BUFFERED_WRITES=1) on top of tss::ComClass,
 * mirroring struct TSS_Managed_Com_Class.
 *
 * Derive from tss::ManagedComClass instead of tss::ComClass directly to get
 * all of that for free. Doing so only requires implementing the smaller set
 * of "*Impl" hooks below (openImpl/closeImpl/readImpl/writeImpl/
 * setTimeoutImpl/getTimeoutImpl) - the actual hardware operations - rather
 * than every slot of TSS_Com_Class_API.
 *
 * tss::ManagedComClass does NOT own its read/write buffers itself - its
 * constructor takes them (pointer + size) explicitly, so every concrete
 * subclass decides its own buffer sizes (no heap allocation, no templates).
 * TSS_DECLARE_MANAGED_COM_BUFFERS()/TSS_MANAGED_COM_INIT() below are small
 * helper macros that do the (mechanical, size-independent) work of
 * declaring those buffers and forwarding them to the base class, so a
 * subclass doesn't have to hand-write the TSS_MINIMAL_SENSOR/
 * TSS_BUFFERED_WRITES #if guards itself.
 *
 * Usage:
 * @code
 * class MySerial : public tss::ManagedComClass {
 * public:
 *     MySerial() : TSS_MANAGED_COM_INIT(ManagedComClass, false) {}
 *
 *     int openImpl() override { ... }
 *     int closeImpl() override { ... }
 *     int readImpl(size_t num_bytes, uint8_t *out) override { ... }
 *     int writeImpl(const uint8_t *bytes, size_t len) override { ... }
 *     void setTimeoutImpl(uint32_t timeout_ms) override { ... }
 *     uint32_t getTimeoutImpl() override { ... }
 *
 * private:
 *     // Default sizes; use any size(s) you like instead.
 *     TSS_DECLARE_MANAGED_COM_BUFFERS(tss::kManagedComDefaultReadBufferSize, tss::kManagedComDefaultWriteBufferSize)
 * };
 *
 * MySerial com;
 * tssCreateSensor(&sensor, com); // gets peek/length/read_until/clear/buffered writes for free
 * @endcode
 */

#ifndef __TSS_CPP_MANAGED_COM_CLASS_HPP__
#define __TSS_CPP_MANAGED_COM_CLASS_HPP__

#include "tss/cpp/com/com_class.hpp"
#include "tss/com/managed_com.h"
#include "tss/sys/config.h"

#include <stddef.h>
#include <stdint.h>

namespace tss {

/// Recommended default buffer sizes - not required, just what
/// TSS_DECLARE_MANAGED_COM_BUFFERS() is typically invoked with for a
/// "no special requirements" subclass. The read buffer size must be a power
/// of 2. See tss/sys/config.h for sizing guidance (4096 guarantees the
/// largest possible command response fits).
constexpr size_t kManagedComDefaultReadBufferSize = 4096;
constexpr size_t kManagedComDefaultWriteBufferSize = 512;

/**
 * @brief Derive from this (instead of tss::ComClass directly) to get
 * peek/length/read_until/clear support and buffered writes for free. Doing
 * so only requires implementing the smaller set of "*Impl" hooks below
 * (openImpl/closeImpl/readImpl/writeImpl/setTimeoutImpl/getTimeoutImpl) -
 * the actual hardware operations - rather than every slot of
 * TSS_Com_Class_API.
 *
 * Does NOT own a read/write buffer itself - its constructor takes them
 * (pointer + size) explicitly, so every subclass decides its own buffer
 * size(s) (no heap allocation anywhere, and no templates). See the
 * TSS_DECLARE_MANAGED_COM_BUFFERS()/TSS_MANAGED_COM_INIT() macros below -
 * and the file-level doc comment above - for the easiest way to do that.
 */
class ManagedComClass : public ComClass {
public:
    /**
     * @param supports_reenumeration See tss::ComClass's constructor.
     * @param read_buf/read_buf_size Read buffer this instance should use
     * (nullptr/0 if TSS_MINIMAL_SENSOR=1, in which case no read buffer
     * exists and these are ignored).
     * @param write_buf/write_buf_size Write buffer this instance should use
     * (nullptr/0 if TSS_BUFFERED_WRITES=0, in which case no write buffer
     * exists and these are ignored).
     */
    ManagedComClass(bool supports_reenumeration, uint8_t *read_buf, size_t read_buf_size, uint8_t *write_buf, size_t write_buf_size);

    //-----------------------------------------------------------------
    // Required overrides - the actual hardware operations. Everything else
    // (peek/length/read_until/clear/buffered writes) is implemented in
    // terms of these by this class.
    //-----------------------------------------------------------------

    virtual int openImpl() = 0;
    virtual int closeImpl() = 0;
    virtual int readImpl(size_t num_bytes, uint8_t *out) = 0;
    virtual int writeImpl(const uint8_t *bytes, size_t len) = 0;
    virtual void setTimeoutImpl(uint32_t timeout_ms) = 0;
    virtual uint32_t getTimeoutImpl() = 0;

    //-----------------------------------------------------------------
    // Optional overrides - device (re)discovery. Default reports the
    // operation as unsupported, same as tss::ComClass's own default.
    //
    // Unlike the other *Impl hooks, these take \p com explicitly (mirroring
    // TSS_Com_Class_API::reenumerate/auto_detect) because an implementation
    // must pass it back to \p cb for each candidate device it finds - \p com
    // is m_hw (this class's private "child" object), which callers further
    // up the managed-com chain rely on getting back unchanged.
    //-----------------------------------------------------------------

    virtual int reenumerateImpl(struct TSS_Com_Class *com, TssComAutoDetectCallback cb, void *detect_data);
    virtual int autoDetectImpl(struct TSS_Com_Class *com, TssComAutoDetectCallback cb, void *detect_data);

    //-----------------------------------------------------------------
    // tss::ComClass overrides - all delegate to the embedded
    // TSS_Managed_Com_Class, which itself delegates to m_hw (and therefore
    // the *Impl hooks above) as needed.
    //-----------------------------------------------------------------

    int open() override;
    int close() override;
    int read(size_t num_bytes, uint8_t *out) override;
    int write(const uint8_t *bytes, size_t len) override;
    void setTimeout(uint32_t timeout_ms) override;
    uint32_t getTimeout() override;
    int readUntil(uint8_t value, uint8_t *out, size_t size) override;
    void clearImmediate() override;
    void clearTimeout(uint32_t timeout_ms) override;
#if !(TSS_MINIMAL_SENSOR)
    int peek(size_t start, size_t num_bytes, uint8_t *out) override;
    int peekUntil(size_t start, uint8_t value, uint8_t *out, size_t size) override;
    size_t peekCapacity() override;
    size_t length() override;
#endif
#if TSS_BUFFERED_WRITES
    int beginWrite() override;
    int endWrite() override;
#endif
    int reenumerate(TssComAutoDetectCallback cb, void *detect_data) override;
    int autoDetect(TssComAutoDetectCallback cb, void *detect_data) override;

private:
    // TSS_Com_Class_API vtable for m_hw (the "child" wrapped by m_managed).
    // Every entry forwards into the calling instance's matching *Impl
    // method, except read_until/clear_immediate/clear_timeout, which reuse
    // the exported tssManagedComBase* helpers directly (same as e.g.
    // communication/serial/serial_com_class.c does for its own raw child).
    static const struct TSS_Com_Class_API s_impl_api;

    // TSS_Com_Class_API vtable used as m_managed.base.api INSTEAD of the
    // real one tssCreateManagedComDynamic() would normally set up, when
    // that call fails (currently: read_buf_size wasn't a power of 2) - see
    // the constructor. Every entry just returns/no-ops immediately, so
    // ManagedComClass's own methods (open()/read()/write()/...) never need
    // to check for this themselves - they always unconditionally dispatch
    // through m_managed.base.api, exactly like the success path, and simply
    // get routed here instead of into potentially-uninitialized state.
    static const struct TSS_Com_Class_API s_failed_api;

    static ManagedComClass *implOwner(struct TSS_Com_Class *com);

    static int trampolineOpenImpl(struct TSS_Com_Class *com);
    static int trampolineCloseImpl(struct TSS_Com_Class *com);
    static int trampolineReadImpl(struct TSS_Com_Class *com, size_t num_bytes, uint8_t *out);
    static int trampolineWriteImpl(struct TSS_Com_Class *com, const uint8_t *bytes, size_t len);
    static void trampolineSetTimeoutImpl(struct TSS_Com_Class *com, uint32_t timeout_ms);
    static uint32_t trampolineGetTimeoutImpl(struct TSS_Com_Class *com);
    static int trampolineReenumerateImpl(struct TSS_Com_Class *com, TssComAutoDetectCallback cb, void *detect_data);
    static int trampolineAutoDetectImpl(struct TSS_Com_Class *out, TssComAutoDetectCallback cb, void *detect_data);

    // s_failed_api's entries - all ignore `com` and its other arguments,
    // simply reporting failure/nothing-to-report immediately.
    static int failedOpenOrClose(struct TSS_Com_Class *com);
    static int failedRead(struct TSS_Com_Class *com, size_t num_bytes, uint8_t *out);
    static int failedReadUntil(struct TSS_Com_Class *com, uint8_t value, uint8_t *out, size_t size);
    static int failedWrite(struct TSS_Com_Class *com, const uint8_t *bytes, size_t len);
    static void failedSetTimeout(struct TSS_Com_Class *com, uint32_t timeout_ms);
    static uint32_t failedGetTimeout(struct TSS_Com_Class *com);
    static void failedClear(struct TSS_Com_Class *com);
    static void failedClearTimeout(struct TSS_Com_Class *com, uint32_t timeout_ms);
    static int failedReenumerateOrAutoDetect(struct TSS_Com_Class *com, TssComAutoDetectCallback cb, void *detect_data);
#if !(TSS_MINIMAL_SENSOR)
    static int failedPeek(struct TSS_Com_Class *com, size_t start, size_t num_bytes, uint8_t *out);
    static int failedPeekUntil(struct TSS_Com_Class *com, size_t start, uint8_t value, uint8_t *out, size_t size);
    static size_t failedPeekCapacityOrLength(struct TSS_Com_Class *com);
#endif
#if TSS_BUFFERED_WRITES
    static int failedBeginOrEndWrite(struct TSS_Com_Class *com);
#endif

    // The "hardware" com object - what m_managed wraps as its child. Its
    // `.context` is `this`, and its `.api` is s_impl_api.
    struct TSS_Context_Com_Class m_hw;

    // Provides peek/length/read_until/clear support (and write buffering,
    // if TSS_BUFFERED_WRITES=1) on top of m_hw. If construction fails (see
    // the constructor), only `.base.api` is set (to s_failed_api) - nothing
    // else about this is safe to read directly, only through `.base.api`.
    struct TSS_Managed_Com_Class m_managed;
};


} // namespace tss

//---------------------------------------------------------------------------
// Buffer-declaration/forwarding helpers for classes deriving (directly or
// indirectly) from tss::ManagedComClass. Plain preprocessor macros (not tied
// to the tss namespace) so they're usable from any subclass, anywhere in the
// managed-com inheritance chain (e.g. TssSpiComClass, or a custom-sized
// subclass of it) without repeating the TSS_MINIMAL_SENSOR/
// TSS_BUFFERED_WRITES #if guards or the nullptr/sizeof() plumbing by hand.
//---------------------------------------------------------------------------

#if !(TSS_MINIMAL_SENSOR)
#define TSS_DECLARE_MANAGED_COM_READ_BUFFER(READ_SIZE) \
    static_assert(TSS_RING_POW_2(READ_SIZE), "TSS_DECLARE_MANAGED_COM_BUFFERS()/TSS_DECLARE_MANAGED_COM_READ_BUFFER(): read buffer size must be a power of 2"); \
    uint8_t m_read_buffer[READ_SIZE];
#define TSS_MANAGED_COM_READ_BUFFER_ARGS m_read_buffer, sizeof(m_read_buffer)
#else
#define TSS_DECLARE_MANAGED_COM_READ_BUFFER(READ_SIZE)
#define TSS_MANAGED_COM_READ_BUFFER_ARGS nullptr, (size_t)0
#endif

#if TSS_BUFFERED_WRITES
#define TSS_DECLARE_MANAGED_COM_WRITE_BUFFER(WRITE_SIZE) uint8_t m_write_buffer[WRITE_SIZE];
#define TSS_MANAGED_COM_WRITE_BUFFER_ARGS m_write_buffer, sizeof(m_write_buffer)
#else
#define TSS_DECLARE_MANAGED_COM_WRITE_BUFFER(WRITE_SIZE)
#define TSS_MANAGED_COM_WRITE_BUFFER_ARGS nullptr, (size_t)0
#endif

/**
 * @brief Declares this class's OWN m_read_buffer[READ_SIZE]/
 * m_write_buffer[WRITE_SIZE] member arrays - place in a private section of
 * any class deriving (directly or indirectly) from tss::ManagedComClass.
 * Automatically omits whichever buffer isn't needed (TSS_MINIMAL_SENSOR=1 /
 * TSS_BUFFERED_WRITES=0). Pair with TSS_MANAGED_COM_INIT(...) in your
 * constructor's mem-initializer-list to forward them to your base class.
 */
#define TSS_DECLARE_MANAGED_COM_BUFFERS(READ_SIZE, WRITE_SIZE) \
    TSS_DECLARE_MANAGED_COM_READ_BUFFER(READ_SIZE) \
    TSS_DECLARE_MANAGED_COM_WRITE_BUFFER(WRITE_SIZE)

/**
 * @brief Use in a constructor's mem-initializer-list to construct
 * BASE_CLASS - tss::ManagedComClass itself, or any class deriving from it
 * whose OWN constructor likewise takes its read/write buffer pointer+size
 * as its LAST 4 arguments (e.g. TssSpiComClassBase) - passing EXTRA_ARGS
 * through unchanged, followed by this class's own m_read_buffer/
 * m_write_buffer (as declared via TSS_DECLARE_MANAGED_COM_BUFFERS() above)
 * or nullptr/0 in place of whichever buffer is compiled out.
 *
 * @code
 * class MyCom : public tss::ManagedComClass {
 * public:
 *     MyCom() : TSS_MANAGED_COM_INIT(ManagedComClass, false) {}
 * private:
 *     TSS_DECLARE_MANAGED_COM_BUFFERS(1024, 64)
 * };
 * @endcode
 */
#define TSS_MANAGED_COM_INIT(BASE_CLASS, ...) BASE_CLASS(__VA_ARGS__, TSS_MANAGED_COM_READ_BUFFER_ARGS, TSS_MANAGED_COM_WRITE_BUFFER_ARGS)

#endif /* __TSS_CPP_MANAGED_COM_CLASS_HPP__ */
