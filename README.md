# FF13Fix-4GB
4GB (Large Address Aware) patch for Final Fantasy XIII on PC.

A minimal fork of [FF13Fix](https://github.com/rebtd7/FF13Fix) that applies only the Large Address Aware patch, with no other gameplay modifications. Intended for use in speedruns where the full FF13Fix is not permitted.

# What this does
Allows Final Fantasy XIII to access more than 2GB of RAM by applying the Large Address Aware flag at runtime. This helps prevent out-of-memory crashes.

No framerate changes, no gameplay modifications.

# Installation
1. Add `d3d9.dll` and `FF13Fix4GB.ini` to the folder:
   ```
   FINAL FANTASY XIII\white_data\prog\win\bin
   ```
2. Create a copy of the **unpatched** `ffxiiiimg.exe` in the same folder and name it `untouched.exe`.
   - This is **required**. The game's integrity check will fail without it.
   - If you have already applied the 4GB patch, the patcher will have created `ffxiiiimg.exe.Backup` — you can rename that file to `untouched.exe` instead.

# Configuration
`FF13Fix4GB.ini` is optional. Default settings work out of the box.

| Setting | Default | Description |
|---|---|---|
| `AutoFix` | `true` | Enable the LAA patch |
| `LogLevel` | `2` (info) | Log verbosity (0=trace … 5=critical) |
| `LogFlush` | `false` | Flush log every line (slow) |

# Reporting issues
Please include `FF13Fix4GB.log` and your system specs.
