# Photo Scanner Kiosk - Qt6 C++ Application

A native Qt6 C++ application for Raspberry Pi 4 that provides a self-service photo scanning kiosk with payment processing and digital delivery.

## Features

- **Phone Number Input**: Users enter their phone number for delivery
- **Credit Purchase**: Select scan packages (1 credit = $3, 4 credits = $10)
- **Tap to Pay**: Integrated Square payment processing
- **Scanner Support**: Fujitsu fi-800R digital scanner via SANE
- **Image Processing**: Automatic cropping and JPEG conversion
- **Digital Delivery**: SMS/Email delivery of scanned photos
- **Inactivity Timeout**: Automatic reset after 60 seconds of inactivity
- **Demo Mode**: Test without hardware

## System Requirements

- **Hardware**:
  - Raspberry Pi 4 (2GB+ RAM recommended)
  - Fujitsu fi-800R scanner
  - USB connection for scanner
  - Touchscreen display (1024x768 or higher)
  - Internet connection for payment processing

- **Software**:
  - Raspberry Pi OS (64-bit recommended)
  - Qt6 6.2+
  - SANE scanner drivers
  - CMake 3.16+
  - GCC/G++ with C++17 support

## Installation

### 1. Automatic Installation (Recommended)

Run the installation script on your Raspberry Pi:

```bash
cd kiosk_app_cpp
chmod +x install_rpi.sh
./install_rpi.sh
```

This will install all dependencies including:
- Qt6 development libraries
- SANE scanner drivers with Fujitsu support
- Image processing libraries (libjpeg, libtiff)
- CURL for API calls
- Python3 for email support
- QR code generation tools

**Important**: After installation, log out and log back in for scanner permissions to take effect.

### 2. Manual Installation

If you prefer manual installation:

```bash
# Update system
sudo apt-get update
sudo apt-get upgrade -y

# Build tools
sudo apt-get install -y build-essential cmake git pkg-config

# Qt6
sudo apt-get install -y qt6-base-dev qt6-base-dev-tools

# Scanner support (SANE)
sudo apt-get install -y sane sane-utils libsane-dev

# Image libraries
sudo apt-get install -y libjpeg-dev libtiff-dev libpng-dev

# Networking
sudo apt-get install -y libcurl4-openssl-dev libssl-dev

# QR codes
sudo apt-get install -y qrencode

# Python for email
sudo apt-get install -y python3 python3-pip

# Add user to scanner group
sudo usermod -a -G scanner $USER
```

## Building the Application

### On Raspberry Pi (Local Compilation)

```bash
cd kiosk_app_cpp
mkdir build && cd build
cmake ..
make -j4
```

### Cross-Compilation (Optional)

For faster compilation, you can cross-compile on your development machine and deploy to the Pi.

## Configuration

### Environment Variables

Create a `.env` file or export these variables:

```bash
# Scanner Settings
export DEMO_MODE=false              # Set to 'true' for demo mode

# SMTP Email Settings
export SMTP_SERVER=smtp.gmail.com
export SMTP_PORT=587
export EMAIL_USER=your_email@gmail.com
export EMAIL_PASSWORD=your_app_password
export SENDER_EMAIL=your_email@gmail.com

# Square API Settings
export SQUARE_ACCESS_TOKEN=your_square_token
export SQUARE_LOCATION_ID=your_location_id
export SQUARE_API_BASE=https://connect.squareupsandbox.com  # or production URL

# UI Settings
export KIOSK_FULLSCREEN=true        # Run in fullscreen mode
```

### Scanner Configuration

The application auto-detects Fujitsu fi-800R scanners. To test detection:

```bash
scanimage -L
```

Expected output:
```
device `fujitsu:ScanSnap fi-800R:xxxxx' is a FUJITSU fi-800R scanner
```

If your scanner isn't detected:
1. Check USB connection
2. Verify udev rules: `ls -la /etc/udev/rules.d/65-fujitsu-scanner.rules`
3. Check user is in scanner group: `groups`
4. Reload udev: `sudo udevadm control --reload && sudo udevadm trigger`

## Running the Application

### Normal Mode

```bash
cd build
./PhotoScannerKiosk
```

### Demo Mode (No Hardware Required)

```bash
cd build
DEMO_MODE=true ./PhotoScannerKiosk --demo
```

Demo mode simulates:
- Scanner detection and scanning
- Payment processing
- Image generation

### Fullscreen Mode

```bash
./PhotoScannerKiosk --fullscreen
```

## Application Workflow

1. **Logo Screen**: Tap to begin
2. **Phone Input**: Enter 10-digit phone number
3. **Credit Purchase**: Select 1 or 4 credits
4. **Tap to Pay**: Payment processing via Square
5. **Scanning**: Insert and scan photo strips
6. **Confirmation**: Success message with delivery confirmation

## Architecture

```
kiosk_app_cpp/
├── include/
│   ├── AppController.h          # Main workflow controller
│   ├── AppSettings.h            # Configuration management
│   ├── ScannerManager.h         # Fujitsu fi-800R scanner interface
│   ├── PaymentManager.h         # Square API integration
│   ├── EmailManager.h           # SMTP email delivery
│   ├── ImageProcessor.h         # TIFF to JPEG conversion
│   └── ui/
│       ├── MainWindow.h         # Main window with screen management
│       ├── LogoScreen.h         # Initial splash screen
│       ├── PhoneInputScreen.h   # Phone number entry
│       ├── CreditPurchaseScreen.h # Credit package selection
│       ├── TapToPayScreen.h     # Payment interface
│       ├── ScanPromptScreen.h   # Scanner guidance
│       └── ConfirmationScreen.h # Completion message
├── src/                         # Implementation files
├── CMakeLists.txt              # Build configuration
├── install_rpi.sh              # Raspberry Pi setup script
└── README.md                   # This file
```

## Deployment

### SSH to Raspberry Pi

From your development machine:

```bash
ssh erhan@192.168.1.233
```

### Transfer Files

```bash
# Using SCP
scp -r kiosk_app_cpp erhan@192.168.1.233:~/

# Or using rsync
rsync -avz kiosk_app_cpp/ erhan@192.168.1.233:~/kiosk_app_cpp/
```

### Auto-Start on Boot

To run the kiosk automatically on boot:

```bash
# Create systemd service
sudo nano /etc/systemd/system/photo-kiosk.service
```

Add:

```ini
[Unit]
Description=Photo Scanner Kiosk
After=graphical.target

[Service]
Type=simple
User=erhan
Environment=DISPLAY=:0
Environment=DEMO_MODE=false
WorkingDirectory=/home/erhan/kiosk_app_cpp/build
ExecStart=/home/erhan/kiosk_app_cpp/build/PhotoScannerKiosk --fullscreen
Restart=on-failure

[Install]
WantedBy=graphical.target
```

Enable and start:

```bash
sudo systemctl enable photo-kiosk.service
sudo systemctl start photo-kiosk.service
```

## Troubleshooting

### Scanner Not Detected

```bash
# Check USB connection
lsusb | grep -i fujitsu

# Test SANE detection
scanimage -L

# Check permissions
groups  # Should include 'scanner'

# Restart SANE
sudo systemctl restart saned
```

### Build Errors

```bash
# Check Qt6 installation
qmake6 --version
# or
qmake --version

# Check CMake can find Qt6
cmake --find-package Qt6
```

### Display Issues

```bash
# Check display resolution
xrandr

# Test Qt application
export DISPLAY=:0
./PhotoScannerKiosk
```

### Permission Errors

```bash
# Fix scanner permissions
sudo usermod -a -G scanner $USER

# Fix directory permissions
chmod -R 755 ~/kiosk_app
```

## Development

### Adding New Features

1. **New UI Screen**: Create in `include/ui/` and `src/ui/`
2. **New Manager**: Add to `include/` and `src/`
3. **Modify Workflow**: Edit `AppController.cpp`

### Testing

```bash
# Run in demo mode for testing
DEMO_MODE=true ./PhotoScannerKiosk --demo

# Test scanner separately
scanimage --format=tiff --resolution=600 > test.tiff
```

### Debugging

```bash
# Enable Qt debug output
export QT_DEBUG_PLUGINS=1
export QT_LOGGING_RULES='*.debug=true'
./PhotoScannerKiosk
```

## Credits

- **Original Python Version**: Based on PyQt6 implementation
- **New Design**: TypeScript/React prototype
- **Scanner**: Fujitsu fi-800R with SANE driver
- **Payment**: Square API
- **UI Framework**: Qt6 Widgets

## License

Proprietary - All rights reserved

## Support

For issues or questions, contact the development team.

---

**Note**: This application requires active Square API credentials and a configured Fujitsu fi-800R scanner for full operation. Use demo mode for testing without hardware.
