#include "timing_mem.h"
#include "log.h"

namespace MemoryHierarchy
{
void *xmalloc(int size)
{
    void *x = (void *)malloc(size);
    if (x == 0)
        fatal_error("Out of memory at request for %d bytes.\n");
    return (x);
}

void Memory::Reset() {
    memcpy(this->text_seg, ::text_seg, mem_text_seg_size);
    memcpy(this->data_seg, ::data_seg, mem_data_seg_size);
    memcpy(this->stack_seg, ::stack_seg, mem_stack_seg_size);
    memcpy(this->k_text_seg, ::k_text_seg, mem_k_text_seg_size);
    memcpy(this->k_data_seg, ::k_data_seg, mem_k_data_seg_size);
    printf("reset memory\n");
}

void Memory::make_memory(int text_size, int data_size, int data_limit,
                         int stack_size, int stack_limit, int k_text_size,
                         int k_data_size, int k_data_limit)
{
    text_seg = (instruction **)xmalloc(BYTES_TO_INST(text_size));
    text_top = TEXT_BOT + text_size;
    mem_text_seg_size = BYTES_TO_INST(text_size);

    data_size = ROUND_UP(data_size, BYTES_PER_WORD); /* Keep word aligned */
    data_seg = (mem_word *)xmalloc(data_size);
    data_seg_b = (BYTE_TYPE *)data_seg;
    data_seg_h = (short *)data_seg;
    data_top = DATA_BOT + data_size;
    data_size_limit = data_limit;
    mem_data_seg_size = data_size;

    stack_size = ROUND_UP(stack_size, BYTES_PER_WORD); /* Keep word aligned */
    stack_seg = (mem_word *)xmalloc(stack_size);
    stack_seg_b = (BYTE_TYPE *)stack_seg;
    stack_seg_h = (short *)stack_seg;
    stack_bot = STACK_TOP - stack_size;
    stack_size_limit = stack_limit;
    mem_stack_seg_size = stack_size;

    k_text_seg = (instruction **)xmalloc(BYTES_TO_INST(k_text_size));
    k_text_top = K_TEXT_BOT + k_text_size;
    mem_k_text_seg_size = BYTES_TO_INST(k_text_size);

    k_data_size = ROUND_UP(k_data_size, BYTES_PER_WORD); /* Keep word aligned */
    k_data_seg = (mem_word *)xmalloc(k_data_size);
    k_data_seg_b = (BYTE_TYPE *)k_data_seg;
    k_data_seg_h = (short *)k_data_seg;
    k_data_top = K_DATA_BOT + k_data_size;
    k_data_size_limit = k_data_limit;
    mem_k_data_seg_size = k_data_size;
}

reg_word Memory::read_mem_word(mem_addr addr)
{
    if ((addr >= DATA_BOT) && (addr < data_top) && !(addr & 0x3))
        return data_seg[(addr - DATA_BOT) >> 2];
    else if ((addr >= stack_bot) && (addr < STACK_TOP) && !(addr & 0x3))
        return stack_seg[(addr - stack_bot) >> 2];
    else if ((addr >= K_DATA_BOT) && (addr < k_data_top) && !(addr & 0x3))
        return k_data_seg[(addr - K_DATA_BOT) >> 2];
    else
    {
        return bad_mem_read(addr, 0x3);
        // printf("mem addr:%x\n", addr);
        // assert_msg("Bad address in read_mem_word\n");
    }
}

instruction *Memory::read_mem_inst(mem_addr addr)
{
    if ((addr >= TEXT_BOT) && (addr < text_top) && !(addr & 0x3))
        return text_seg[(addr - TEXT_BOT) >> 2];
    else if ((addr >= K_TEXT_BOT) && (addr < k_text_top) && !(addr & 0x3))
        return k_text_seg[(addr - K_TEXT_BOT) >> 2];
    else
    {
        // return bad_text_read(addr);
        printf("mem addr:%x\n", addr);
        assert_msg("Bad address in read_mem_inst\n");
    }
}

void *Memory::mem_reference(mem_addr addr)
{
    if ((addr >= TEXT_BOT) && (addr < text_top))
        return addr - TEXT_BOT + (char *)text_seg;
    else if ((addr >= DATA_BOT) && (addr < data_top))
        return addr - DATA_BOT + (char *)data_seg;
    else if ((addr >= stack_bot) && (addr < STACK_TOP))
        return addr - stack_bot + (char *)stack_seg;
    else if ((addr >= K_TEXT_BOT) && (addr < k_text_top))
        return addr - K_TEXT_BOT + (char *)k_text_seg;
    else if ((addr >= K_DATA_BOT) && (addr < k_data_top))
        return addr - K_DATA_BOT + (char *)k_data_seg;
    else
    {
        printf("mem addr:%x\n", addr);
        assert_msg("Memory address out of bounds\n");
        return NULL;
    }
}

void Memory::set_mem_word(mem_addr addr, reg_word value)
{
    // printf("set mem addr:%x val:%x\n", addr, value);
    data_modified = true;
    if ((addr >= DATA_BOT) && (addr < data_top) && !(addr & 0x3))
        data_seg[(addr - DATA_BOT) >> 2] = (mem_word)value;
    else if ((addr >= stack_bot) && (addr < STACK_TOP) && !(addr & 0x3))
        stack_seg[(addr - stack_bot) >> 2] = (mem_word)value;
    else if ((addr >= K_DATA_BOT) && (addr < k_data_top) && !(addr & 0x3))
        k_data_seg[(addr - K_DATA_BOT) >> 2] = (mem_word)value;
    else
    {
        bad_mem_write(addr, value, 0x3);
        // printf("mem addr:%x val:%x\n", addr, value);
        // assert_msg("Bad address in set_mem_word\n");
    }
}

void Memory::expand_stack(int addl_bytes) {
    int delta = ROUND_UP(addl_bytes, BYTES_PER_WORD); /* Keep word aligned */
    int old_size = STACK_TOP - stack_bot;
    int new_size = old_size + MAX(delta, old_size);
    mem_word *new_seg;
    mem_word *po, *pn;

    if ((addl_bytes < 0) || (new_size > stack_size_limit))
    {
        run_error("Can't expand stack segment by %d bytes to %d bytes.\nUse -lstack # with # > %d\n",
                  addl_bytes, new_size, new_size);
    }

    new_seg = (mem_word *)xmalloc(new_size);
    memset(new_seg, 0, new_size);

    po = stack_seg + (old_size / BYTES_PER_WORD - 1);
    pn = new_seg + (new_size / BYTES_PER_WORD - 1);
    for (; po >= stack_seg;)
        *pn-- = *po--;

    free(stack_seg);
    stack_seg = new_seg;
    stack_seg_b = (BYTE_TYPE *)stack_seg;
    stack_seg_h = (short *)stack_seg;
    stack_bot -= (new_size - old_size);
}

mem_word Memory::bad_mem_read(mem_addr addr, int mask) {
    mem_word tmp;

    if ((addr & mask) != 0)
        assert_msg("Unaligned address fault\n")
    else if (addr >= TEXT_BOT && addr < text_top)
        switch (mask)
        {
        case 0x0:
            tmp = ENCODING(text_seg[(addr - TEXT_BOT) >> 2]);
#ifdef SPIM_BIGENDIAN
            tmp = (unsigned)tmp >> (8 * (3 - (addr & 0x3)));
#else
            tmp = (unsigned)tmp >> (8 * (addr & 0x3));
#endif
            return (0xff & tmp);

        case 0x1:
            tmp = ENCODING(text_seg[(addr - TEXT_BOT) >> 2]);
#ifdef SPIM_BIGENDIAN
            tmp = (unsigned)tmp >> (8 * (2 - (addr & 0x2)));
#else
            tmp = (unsigned)tmp >> (8 * (addr & 0x2));
#endif
            return (0xffff & tmp);

        case 0x3:
        {
            instruction *inst = text_seg[(addr - TEXT_BOT) >> 2];
            if (inst == NULL)
                return 0;
            else
                return (ENCODING(inst));
        }

        default:
            run_error("Bad mask (0x%x) in bad_mem_read\n", mask);
        }
    else
        assert_msg("Address out of range\n");
    return (0);
}

void Memory::bad_mem_write(mem_addr addr, mem_word value, int mask) {
    mem_word tmp;

    if ((addr & mask) != 0)
        assert_msg("Unaligned address fault\n")
    else if (addr >= TEXT_BOT && addr < text_top)
    {
        switch (mask)
        {
        case 0x0:
            tmp = ENCODING(text_seg[(addr - TEXT_BOT) >> 2]);
#ifdef SPIM_BIGENDIAN
            tmp = ((tmp & ~(0xff << (8 * (3 - (addr & 0x3))))) | (value & 0xff) << (8 * (3 - (addr & 0x3))));
#else
            tmp = ((tmp & ~(0xff << (8 * (addr & 0x3)))) | (value & 0xff) << (8 * (addr & 0x3)));
#endif
            break;

        case 0x1:
            tmp = ENCODING(text_seg[(addr - TEXT_BOT) >> 2]);
#ifdef SPIM_BIGENDIAN
            tmp = ((tmp & ~(0xffff << (8 * (2 - (addr & 0x2))))) | (value & 0xffff) << (8 * (2 - (addr & 0x2))));
#else
            tmp = ((tmp & ~(0xffff << (8 * (addr & 0x2)))) | (value & 0xffff) << (8 * (addr & 0x2)));
#endif
            break;

        case 0x3:
            tmp = value;
            break;

        default:
            tmp = 0;
            run_error("Bad mask (0x%x) in bad_mem_read\n", mask);
        }

        if (text_seg[(addr - TEXT_BOT) >> 2] != NULL)
        {
            free_inst(text_seg[(addr - TEXT_BOT) >> 2]);
        }
        text_seg[(addr - TEXT_BOT) >> 2] = inst_decode(tmp);

        text_modified = true;
    }
    else if (addr > data_top && addr < stack_bot
             /* If more than 16 MB below stack, probably is bad data ref */
             && addr > stack_bot - 16 * K * K)
    {
        /* Grow stack segment */
        expand_stack(stack_bot - addr + 4);
        if (addr >= stack_bot)
        {
            if (mask == 0)
                stack_seg_b[addr - stack_bot] = (char)value;
            else if (mask == 1)
                stack_seg_h[(addr - stack_bot) >> 1] = (short)value;
            else
                stack_seg[(addr - stack_bot) >> 2] = value;
        }
        else
            assert_msg("Bad CP0_BadVAddr\n");

        data_modified = true;
    }
    else
        assert_msg("Address out of range\n");
}

Memory::Memory()
{
    make_memory(TEXT_SIZE, ::data_top-DATA_BOT, DATA_LIMIT, STACK_SIZE, STACK_LIMIT, K_TEXT_SIZE, ::k_data_top-K_DATA_BOT, K_DATA_LIMIT);
}

Memory::~Memory()
{
    free(this->text_seg);
    free(this->data_seg);
    free(this->stack_seg);
    free(this->k_data_seg);
    free(this->k_text_seg);

}
} // namespace MemoryHierarchy