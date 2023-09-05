### goals
1. The fundamental law of debugging, and bug taxonomies
2. Tips for avoiding bugs
3. How to plan for bugs
4. The different kinds of memory errors
5. How to use a debugger to pinpoint code causing a bug

### rules
1. Techniques for planning for bugs include `error logging`, `debug traces`, `assertions`, and `crash dumps`. reproducing bugs, debugging reproducible bugs, debugging nonreproducible bugs, debugging memory errors, and debugging multithreaded programs.
2. The first rule of debugging is to be honest with yourself and admit that your code will contain bugs!
3. The fundamental law of debugging states that you should avoid bugs when you’re coding, but plan for bugs in your code.
4. A bug in a computer program is incorrect run-time behavior.

### avoiding bugs
1. Read this book from cover to cover (Professional C++, Fifth Edition. Marc Gregoire.)
2. Design before you code
3. Do code reviews
4. Test, test, and test again
5. Write automated unit tests
6. Expect error conditions, and handle them appropriately
7. Use smart pointers to avoid memory leaks
8. Don’t ignore compiler warnings
9. Use static code analysis
10. Use good coding style


### planning for bugs

#### error logging
1. Error logging is the process of writing error messages to persistent storage so that they will be available following an application, or even machine, death. 
2. log messages every few lines in your code so that, in the event of any bug, you’ll be able to trace the code path that was executing. These types of log messages are appropriately called traces.
3. logging that much information will slow down your program; information in your traces is not appropriate for the end user to see. 

#### debug traces
1. useful infomations: The thread ID, if it’s a multithreaded program; The name of the function that generates the trace; The source filename in which the code that generates the trace lives
2. Trace files can be written in text format, but if you do, be careful with logging too much detail. You don’t want to leak intellectual property through your log files! An alternative is to write the files in a binary format that only you can read.
3. start-time debug mode allows your application to run with or without debug mode depending on a command-line argument.
4. Compile-Time Debug Mode: use macro or something else
5. One way to provide Run-Time Debug Mode is to supply an asynchronous interface that controls debug mode on the fly.

##### ring buffers
1. enable tracing in your program at all times. You usually need only the most recent traces to debug a program, so you should store only the most recent traces
2. store logging information in memory and provide a mechanism to dump all the trace messages to standard error or to a log file if the need arises.

#### assertion
1. `<assert>` : assert(bool expression) static_assert -- compile-time

#### crash dumps
1. also called `memory dumps` or `core dumps`
2. make sure you set up a `symbol server` and a `source code control server`. The symbol server is used to store debugging symbols of released binaries of your software. 

### debugging techniques
1. If a feature contains a regression bug, it means that the feature used to work correctly, but stopped working due to the introduction of a bug.


