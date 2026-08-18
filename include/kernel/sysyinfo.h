#ifndef SYSINFO_H
#define SYSINFO_H

typedef struct {
    const char *os_name;
    const char *kernel_version;
    const char *hostname;
    const char *arch;
} sysinfo_t;

static const sysinfo_t sys_info = {
    .os_name        = "Long-OS",
    .kernel_version = "0.1.0-alpha",
    .hostname       = "@longos",
    .arch           = "x86_64"
};

#endif