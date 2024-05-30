/* date = September 25th 2023 10:37 am */

#ifndef BASE_PROJECT_H
#define BASE_PROJECT_H

//- rjf: main thread entry point
internal void BaseMainThreadEntry(void (*entry)(struct CmdLine *cmdln), U64 argument_count, char **arguments);

//- rjf: non-main-thread entry point
internal void BaseThreadEntry(void (*entry)(void *p), void *params);

#endif //BASE_PROJECT_H
