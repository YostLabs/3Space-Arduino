#include "tss/com/managed_com.h"
#include "tss/errors.h"
#include "tss/sys/time.h"

static int open(struct TSS_Com_Class *com);
static int close(struct TSS_Com_Class *com);

static int read(struct TSS_Com_Class *com, size_t num_bytes, uint8_t *out);
static int peek(struct TSS_Com_Class *com, size_t start, size_t num_bytes, uint8_t *out);

static int read_until(struct TSS_Com_Class *com, uint8_t value, uint8_t *out, size_t size);
static int peek_until(struct TSS_Com_Class *com, size_t start, uint8_t value, uint8_t *out, size_t size);

static size_t length(struct TSS_Com_Class *com);  
static size_t peek_capacity(struct TSS_Com_Class *com);
static void set_timeout(struct TSS_Com_Class *com, uint32_t timeout_ms);
static uint32_t get_timeout(struct TSS_Com_Class *com);
static void clear_immediate(struct TSS_Com_Class *com);
static void clear_timeout(struct TSS_Com_Class *com, uint32_t timeout_ms);

static int write(struct TSS_Com_Class *com, const uint8_t *bytes, size_t len);

#if TSS_BUFFERED_WRITES
static int begin_write(struct TSS_Com_Class *com);
static int end_write(struct TSS_Com_Class *com);
#endif

static int reenumerate(struct TSS_Com_Class *com, TssComAutoDetectCallback cb, void *detect_data);
static int auto_detect(struct TSS_Com_Class *out, TssComAutoDetectCallback cb, void *detect_data);

struct TSS_Com_Class_API m_tss_manage_com_api = {
    .open = open,
    .close = close,
    .reenumerate = reenumerate,
    .auto_detect = auto_detect,    
    .in = {
        .set_timeout = set_timeout,
        .get_timeout = get_timeout,
        .clear_immediate = clear_immediate,
        .clear_timeout = clear_timeout,
        .read = read,
        .read_until = read_until,
#if !(TSS_MINIMAL_SENSOR)
        .peek = peek,
        .peek_until = peek_until,
        .peek_capacity = peek_capacity,
        .length = length
#endif
    },
    .out = {
        .write = write,
#if TSS_BUFFERED_WRITES                
        .begin_write = begin_write,
        .end_write = end_write
#endif
    }
};

int tssCreateManagedCom(struct TSS_Com_Class *child, struct TSS_Com_Class *child_container, 
    uint8_t *read_buf, size_t read_size, uint8_t *write_buf, size_t write_size, struct TSS_Managed_Com_Class *out)
{
    if(!TSS_RING_POW_2(read_size)) {
        return TSS_ERR_INVALID_SIZE;
    }

    *out = (struct TSS_Managed_Com_Class) {
        //Attributes
        .child = child,
        .child_container = child_container,
        .read_ring = {
            .capacity = read_size,
            .data = read_buf
        },
        .write_buffer = write_buf,
        .write_buffer_size = write_size,

        //Functions
        .base = {
            .api = &m_tss_manage_com_api,
            .reenumerates = child->reenumerates,
        }
    };

    return TSS_SUCCESS;
}

int tssCreateManagedComDynamic(struct TSS_Com_Class *child, uint8_t *read_buf, size_t read_size, 
    uint8_t *write_buf, size_t write_size, struct TSS_Managed_Com_Class *out)
{
    return tssCreateManagedCom(child, child, read_buf, read_size, write_buf, write_size, out);
}

//--------------------------CUSTOM WRITE BEHAVIOR----------------------------------
#if TSS_BUFFERED_WRITES
static int begin_write(struct TSS_Com_Class *com)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    self->write_buffer_index = 0;
    return TSS_SUCCESS;
}

static int end_write(struct TSS_Com_Class *com)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    return self->child->api->out.write(self->child_container, self->write_buffer, self->write_buffer_index);
}
#endif

static int write(struct TSS_Com_Class *com, const uint8_t *bytes, size_t len)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
#if TSS_BUFFERED_WRITES
    uint32_t i = 0;
    while(i < len) {
        while(self->write_buffer_index < self->write_buffer_size && i < len) {
            self->write_buffer[self->write_buffer_index++] = bytes[i++];
        }
        
        //Previous loop ended because buffer would overflow, 
        //so need to send now and start buffering again.
        if(i < len) {
            end_write(com);
            begin_write(com); //Start again to finish writing
        }
    }

    return self->write_buffer_index >= self->write_buffer_size;
#else
    return self->child->api->out.write(self->child_container, bytes, len);
#endif    
}

#if !(TSS_MINIMAL_SENSOR)
//--------------------------CUSTOM READ BEHAVIOR----------------------------------
static int read(struct TSS_Com_Class *com, size_t num_bytes, uint8_t *out)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    size_t peek_len, i;
    peek_len = ring_size(&self->read_ring);
    for(i = 0; i < peek_len && i < num_bytes; i++) {
        out[i] = ring_pop(&self->read_ring);
    }

    int result = self->child->api->in.read(self->child_container, num_bytes - i, out + i);
    if(result < 0) {
        return result;
    }
    return (int)i + result;
}

static int read_until(struct TSS_Com_Class *com, uint8_t value, uint8_t *out, size_t size)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    size_t peek_len, num_read;
    int result;

    peek_len = ring_size(&self->read_ring);
    num_read = 0;
    while(num_read < peek_len && num_read < size) {
        *out = ring_pop(&self->read_ring);
        num_read++;
        if(*out == value) {
            return (int)num_read;
        }
        out++;
    }

    result = self->child->api->in.read_until(self->child_container, value, out, size - num_read);
    if(result < 0) {
        return result;
    }
    num_read += (size_t)result;
    return (int)num_read;
}

static int peek(struct TSS_Com_Class *com, size_t start, size_t num_bytes, uint8_t *out)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    size_t required_length, len, i;
    tss_time_t start_time;
    uint32_t timeout;

    required_length = start + num_bytes;

    if(required_length > self->read_ring.capacity) {
        return TSS_ERR_INSUFFICIENT_BUFFER;
    }

    timeout = self->child->api->in.get_timeout(self->child_container);
    start_time = tssTimeGet();
    while((len = length(com)) < required_length && tssTimeDiff(start_time) < timeout);

    //Read out as much as can
    for(i = 0; i < num_bytes && i + start < len; i++) {
        out[i] = ring_read(&self->read_ring, i + start);
    }

    return (int)i;
}

static int peek_until(struct TSS_Com_Class *com, size_t start, uint8_t value, uint8_t *out, size_t size)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    size_t num_read;
    tss_time_t start_time;
    uint32_t timeout;
    bool done;

    num_read = 0;
    done = false;
    timeout = self->child->api->in.get_timeout(self->child_container);
    start_time = tssTimeGet();
    while(!done && num_read < size && num_read + start < self->read_ring.capacity && tssTimeDiff(start_time) < timeout) {
        size_t len = length(com);
        while(num_read + start < len) {
            *out = ring_read(&self->read_ring, num_read + start);
            num_read++;
            if(*out == value) {
                done = true;
                break;
            }
            out++;
        }
    }

    //Stopped reading because can't peek further, not because no room to fill.
    if(!done && num_read < size && num_read + start == self->read_ring.capacity) {
        return TSS_ERR_INSUFFICIENT_BUFFER;
    }

    return (int)num_read;
}

inline static void fill_in_buffer(struct TSS_Managed_Com_Class *com)
{
    size_t space, start_index, start_len, read_len;
    int result;
    uint32_t timeout;

    space = ring_space(&com->read_ring);
    if(space == 0) return;

    //Need to do immediate reads, so cache the timeout and set to instant
    timeout = com->child->api->in.get_timeout(com->child_container);
    com->child->api->in.set_timeout(com->child_container, 0);

    //Read filling write index up to either capacity or the read index.
    start_index = ring_index(&com->read_ring, com->read_ring.w_index);
    start_len = com->read_ring.capacity - start_index;
    if(space < start_len) start_len = space; //Can't reach the end, the read index is in front of it
    result = com->child->api->in.read(com->child_container, start_len, com->read_ring.data + start_index);
    read_len = (size_t)result;
    if(result < 0) {
        read_len = 0;
    }

    //Update state variables
    com->read_ring.w_index += read_len;
    space -= read_len;

    //Check if more to read
    if(read_len == start_len && space != 0) {
        //Read filling start of buffer up to the read index.
        //More to read possibly, fill from the start of the buffer now
        size_t end_index = ring_index(&com->read_ring, com->read_ring.r_index);
        result = com->child->api->in.read(com->child_container, end_index, com->read_ring.data);
        read_len = (size_t) result;
        if(result < 0) {
            read_len = 0;
        }

        com->read_ring.w_index += read_len;
    }

    //Restore timeout
    com->child->api->in.set_timeout(com->child_container, timeout);
}

static size_t length(struct TSS_Com_Class *com)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    fill_in_buffer(self);
    return ring_size(&self->read_ring);
}

static size_t peek_capacity(struct TSS_Com_Class *com)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    return self->read_ring.capacity;
}
#else
//---------------------------Just basic wrapping-------------------------
static int read(struct TSS_Com_Class *com, size_t num_bytes, uint8_t *out)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    return self->child->api->in.read(self->child_container, num_bytes, out);
}
static int read_until(struct TSS_Com_Class *com, uint8_t value, uint8_t *out, size_t size)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    return self->child->api->in.read_until(self->child_container, value, out, size);
}
#endif

//--------------------------DIRECT WRAPPING-------------------------------

static int open(struct TSS_Com_Class *com)
{ 
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    return self->child->api->open(self->child_container);
}

static int close(struct TSS_Com_Class *com)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    self->write_buffer_index = 0;
    ring_clear(&self->read_ring);
    return self->child->api->close(self->child_container);
}

static void set_timeout(struct TSS_Com_Class *com, uint32_t timeout_ms)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    self->child->api->in.set_timeout(self->child_container, timeout_ms);
}

static uint32_t get_timeout(struct TSS_Com_Class *com)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    return self->child->api->in.get_timeout(self->child_container);
}

static void clear_immediate(struct TSS_Com_Class *com)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    ring_clear(&self->read_ring);
    self->child->api->in.clear_immediate(self->child_container);
}

static void clear_timeout(struct TSS_Com_Class *com, uint32_t timeout_ms)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;
    ring_clear(&self->read_ring);
    self->child->api->in.clear_timeout(self->child_container, timeout_ms);
}

//--------------------------------------DISCOVERY---------------------------------------------

struct PortEnumerate {
    struct TSS_Managed_Com_Class *self;
    TssComAutoDetectCallback cb;
    void *detect_data;
};


//When reenumerating, need to wrap the child class with this type before sending to the actual callback
static int reenumerate_callback(struct TSS_Com_Class *com, void *user_data)
{
    struct PortEnumerate *info = user_data;

    struct TSS_Managed_Com_Class *managed = info->self;
    if(&managed->base != com) {
        //This gets entered if the TSS_Managed_Com_Class is not included in the struct of the child com class
        //and therefore we need to wrap the child class with the managed class before sending it to the callback.

        //In the case where the TSS_Managed_Com_Class is included in the struct of the child com class,
        //we don't want to wrap the managed class with itself. The child class reenumerate should have already handled the wrapping
        //and therefore this should only be done if com != managed
        tssCreateManagedComDynamic(com, managed->read_ring.data, managed->read_ring.capacity, managed->write_buffer, managed->write_buffer_size, managed);
    }

    return info->cb(&managed->base, info->detect_data);
}

static int reenumerate(struct TSS_Com_Class *com, TssComAutoDetectCallback cb, void *detect_data)
{
    struct TSS_Managed_Com_Class *self = (struct TSS_Managed_Com_Class *)com;

    struct PortEnumerate info = {
        .self = self,
        .cb = cb,
        .detect_data = detect_data
    };

    //Detect based on the actual child type since this is just a wrapper
    return self->child->api->reenumerate(self->child_container, reenumerate_callback, &info);
}

static int auto_detect(struct TSS_Com_Class *out, TssComAutoDetectCallback cb, void *detect_data)
{
    struct TSS_Managed_Com_Class *com = (struct TSS_Managed_Com_Class *)out;
    if(com->child->api->auto_detect == NULL) {
        return TSS_ERR_UNIMPLEMENTED_DETECTION;
    }
    return com->child->api->auto_detect(com->child_container, cb, detect_data);
}

//------------------------------------BASE FUNCTION VERSIONS--------------------------------------

int tssManagedComBaseReadUntil(struct TSS_Com_Class *com, uint8_t value, uint8_t *out, size_t size)
{
    size_t num_read;
    tss_time_t start_time;
    uint32_t timeout;

    timeout = tss_com_get_timeout(com);
    start_time = tssTimeGet();
    num_read = 0;

    //Want to be able to poll instantly, will change back after
    tss_com_set_timeout(com, 0);
    while(num_read < size && tssTimeDiff(start_time) < timeout) {
        int result = tss_com_read(com, 1, out);
        if(result == 1) {
            num_read++;
            if(*out == value) {
                break;
            }
            out++;
        }
        else if(result < 0) {
            //Error reading, return error
            tss_com_set_timeout(com, timeout);
            return result;
        }
    }

    //Restore timeout back to what it was
    tss_com_set_timeout(com, timeout);
    return (int)num_read;
}

void tssManagedComBaseClear(struct TSS_Com_Class *com)
{
    uint8_t buffer[40];
    uint32_t timeout;
    int len;

    timeout = tss_com_get_timeout(com);
    tss_com_set_timeout(com, 0);
    do {
        len = tss_com_read(com, sizeof(buffer), buffer);
    } while(len > 0);  
    tss_com_set_timeout(com, timeout);
}

void tssManagedComBaseClearTimeout(struct TSS_Com_Class *com, uint32_t timeout_ms)
{
    uint8_t buffer[40];
    tss_time_t start, interval_start;
    uint32_t cached_timeout;

    cached_timeout = tss_com_get_timeout(com);
    tss_com_set_timeout(com, 0);

    start = tssTimeGet();
    interval_start = start;
    do {
        int len = tss_com_read(com, sizeof(buffer), buffer);
        if(len > 0) {
            interval_start = tssTimeGet();
        }
        else if(len < 0) {
            //Error reading, return error
            break;
        }
    } while(tssTimeDiff(interval_start) < timeout_ms && tssTimeDiff(start) < cached_timeout);

    tss_com_set_timeout(com, cached_timeout);
}