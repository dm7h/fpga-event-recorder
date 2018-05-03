
def generate_c_code(icosoc_h, icosoc_c, mod):
    code = """
static inline void icosoc_@name@_set_config(uint32_t bitmask) {
    *(volatile uint32_t*)(0x20000004 + @addr@ * 0x10000) = bitmask;
}

static inline uint32_t icosoc_@name@_get_config() {
    return *(volatile uint32_t*)(0x20000004 + @addr@ * 0x10000);
}

static inline uint32_t icosoc_@name@_get_trig() {
    return *(volatile uint32_t*)(0x20000000 + @addr@ * 0x10000);
}

static inline uint32_t icosoc_@name@_get_cnt1() {
    return *(volatile uint32_t*)(0x20000008 + @addr@ * 0x10000);
}

static inline uint32_t icosoc_@name@_get_cnt2() {
    return *(volatile uint32_t*)(0x200000012 + @addr@ * 0x10000);
}
"""

    code = code.replace("@name@", mod["name"])
    code = code.replace("@addr@", mod["addr"])
    icosoc_h.append(code)

