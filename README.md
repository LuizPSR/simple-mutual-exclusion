CS Assignment to handle threads and a simple mutual exclusion mechanism. Here the resources are unique, and threads acquire all required resources at once or wait for all resources to become available.

# Limitations

Program support only 8 resources numbered 0 to 7. It also support only thread ids between 0 and 999. This can be expanded or reduced in the constants in the main file.

# Usage

Program reads the following format from standard input:

tid f_time c_time [each] [resources] [used]

tid: the thread id that will be created for this line, integer seconds

f_time: time that should be spend before locking the required resources, integer seconds

c_time: time that should be spend with resources locked, integer seconds

each resources used: the resources numbered 0 to 7 that will be locked by this specific thread.
