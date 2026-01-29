# logger

Log numeric data to a CSV file via a singleton.

## Usage

- Include the header: `logger/CsvLogger.h`
- Get the global instance and set the CSV path once.
- Append data with `update(key, value)`.
- Call `save()` before the program exits.

```cpp
#include "logger/CsvLogger.h"

int main() {
    CsvLogger& saver = CsvLogger::getInstance();
    saver.setCsvPath("plot/controller_data.csv");

    // ...
    saver.update("data", data);
    // ...

    saver.save();  // ensure data is flushed to disk
    return 0;
}
```

You can call `update` from anywhere after including the header:

```cpp
#include "logger/CsvLogger.h"

// ...
CsvLogger& saver = CsvLogger::getInstance();
saver.update("data", data);
```