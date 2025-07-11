// This is the header file for the system's shell

#ifndef SH_H
#define SH_H

extern "C" void sh_loop();

// Initialize shell
void sh_init();

void convert_init();

#ifndef __cplusplus
extern "C"
{
    #endif
    typedef void (*sh_cmd_f)(const char* args);
    extern "C" void reg_sh_cmd(const char* name, sh_cmd_f handler);

#ifndef __cplusplus
}
#endif

#endif
