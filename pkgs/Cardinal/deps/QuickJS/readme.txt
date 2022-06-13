The main documentation is in doc/quickjs.pdf or doc/quickjs.html.

Local changes:

* remove two stack checks so that running from a thread works - need to refactor this
* remove assertion that dies if context is destroyed with leaks, clean up instead
* added `-fPIC` for compilation
* changed `CONFIG_WIN32` to allow ming-gcc compilation on windows
* removed example compilation, it fails on native windows
