# FF13Fix-4GB

[![Build](https://github.com/a-ki-yoshi/FF13Fix-4GB/actions/workflows/build.yml/badge.svg)](https://github.com/a-ki-yoshi/FF13Fix-4GB/actions/workflows/build.yml)
![Twitch Status](https://img.shields.io/twitch/status/a_ki_yoshi)
![X (formerly Twitter) URL](https://img.shields.io/twitter/url?url=https%3A%2F%2Fx.com%2FOribeAkiyoshi)

4GB (Large Address Aware) patch for Final Fantasy XIII on PC.

A minimal fork of [FF13Fix](https://github.com/rebtd7/FF13Fix) that applies only the Large Address Aware patch, with no other gameplay modifications. Intended for use in speedruns where the full FF13Fix is not permitted.

# What this does
Allows Final Fantasy XIII to access more than 2GB of RAM by applying the Large Address Aware flag at runtime. This helps prevent out-of-memory crashes.

No framerate changes, no gameplay modifications.

# Installation
1. Download the latest release from the [Releases page](../../releases/latest) and extract the zip file. You will get `d3d9.dll` and `FF13Fix4GB.ini`.
2. Move both files into the following folder inside your Final Fantasy XIII installation:
   ```
   white_data\prog\win\bin
   ```
   Example (Steam default):
   ```
   C:\Program Files (x86)\Steam\steamapps\common\FINAL FANTASY XIII\white_data\prog\win\bin
   ```
3. Create a copy of `ffxiiiimg.exe` in the same folder and name it `untouched.exe`.
   - This is **required**. The game's integrity check will fail without it.
   - Use the **original, unmodified** file from a clean Steam installation. If you have already applied the 4GB patch separately, the patcher will have created `ffxiiiimg.exe.Backup` — you can rename that file to `untouched.exe` instead.

# Configuration
`FF13Fix4GB.ini` is optional. Default settings work out of the box.

| Setting | Default | Description |
|---|---|---|
| `AutoFix` | `true` | Enable the LAA patch |
| `LogLevel` | `2` (info) | Log verbosity. Leave at default unless asked to change it for troubleshooting. |
| `LogFlush` | `false` | Flush log every line (slow) |

# Reporting issues
Please include `FF13Fix4GB.log` and your system specs.

`FF13Fix4GB.log` is created automatically in the same folder as `d3d9.dll` when the game runs.
