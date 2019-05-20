# qtcreator-plugin-tabs

Tabs for [Qt Creator](http://qt-project.org/wiki/Category:Tools::QtCreator).

## Copyright / License

Copyright (C) 2019 Andreas Schulz. All rights reserved.

Licensed under the [MIT](https://github.com/Longhanks/qtcreator-plugin-tabs/blob/master/LICENSE) License.


## Build instructions

Provide the following variables to cmake:

| Variable            | Value                                            |
| ------------------- | ------------------------------------------------ |
| `QTCREATOR_SRC`     | Path to directory containing Qt Creator source   |
| `QTCREATOR_BIN`     | Path to Qt Creator executable binary             |
| `QTCREATOR_VERSION` | Target version of Qt Creator (defaults to 4.9.0) |

For example (macOS):

```
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DQTCREATOR_SRC="~/Downloads/qt-creator-opensource-src-4.9.0" -DQTCREATOR_BIN="/Applications/Qt Creator.app/Contents/MacOS/Qt Creator"
make
sudo make install
```
