# Failing tests archive

This directory stores source codes which our compiler fails to compile correctly.

## Navigating through this archive

Each test file contains the source code that fails to compile and a testing output which describes where and why the compilation failed.

Report any issues and incorrect descriptions to @JurikMatej at lednica4ever@gmail.com.

* `./exe` - Generation errors:
  * Invalid assembly structure
  * Returning function's params
  * Invalid operations
* `./sem` - Semantic errors:
  * Duplicit assembly symbols
  * Throwing wrong errors
  * Empty expr operand stack
* `./syn` - Last two syntax errors:  
  * Empty expr operand stack whilst using GTS instruction
  * Multiple commands on a single line (expected EOL after the first one)

Some of the tests in the directories specified above are marked as **_CURRENTLY PASSING_**.  
These remain in failing tests so that they will still be tested after future bugfixes.
