#include "tss/sys/stdinc.h"
#include "tss/constants.h"
#include "internal.h"

int tssUtilStreamSlotStringToCommands(const char *str, const struct TSS_Command* out[TSS_NUM_STREAM_SLOTS+1])
{
    uint8_t num_slots_read = 0;
    struct TSS_Stream_Slot slot = {0};

    while(*str && num_slots_read < TSS_NUM_STREAM_SLOTS) {
        slot.cmd_num = (uint8_t)strtol(str, (char**)&str, 10);
        if(*str == ':') {
            str++;
            
            // cppcheck-suppress unreadVariable
            slot.param = (uint8_t)strtol(str, (char**)&str, 10);
            // cppcheck-suppress unreadVariable
            slot.has_param = true;
        }

        //Reached the end
        if(slot.cmd_num == 255) {
            break;
        }
        
        num_slots_read++;

        //Add to the output
        out[num_slots_read-1] = tssGetCommand(slot.cmd_num);
        
        //Validate and advance string
        if(*str != ',') {
            return -1;
        }
        str++;
    }

    //Null terminate the command list
    out[num_slots_read] = NULL;

    return 0;
}