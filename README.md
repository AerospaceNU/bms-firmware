# BMS Firmware

Firmware project for a Raspberry Pi Pico-based battery management system.

## Getting Started

### 1. Clone the repository

```bash
git clone <your-repo-url>
cd bms-firmware
```

### 2. Open in VS Code

- Open VS Code.
- Select **File > Open Folder...** and choose this repository folder.

### 3. Import as a Pico project

Use the Raspberry Pi Pico extension in VS Code:

- Run **Raspberry Pi Pico: Import Project** from the Command Palette.
- Select this project folder.
- Keep the extension's **default settings** selected during import.

### 4. Build the project

◊

- In VS Code, run the **Compile Project** task/button.
- This builds the firmware in the `build/` directory.

## Build Output

After a successful build, generated artifacts are available in `build/`, including:

- `bms-firmware.elf`
- `bms-firmware.uf2`

## Notes

- This repository already contains Pico project/task configuration, so importing with default settings should work out of the box.
