#!/bin/bash
#
# Installation script for Photo Scanner Kiosk on Raspberry Pi 4
# Supports Raspberry Pi OS (Debian-based)
#
# Usage: ./install_rpi.sh
#

set -e

echo "=========================================="
echo "Photo Scanner Kiosk - Raspberry Pi Setup"
echo "=========================================="
echo ""

# Check if running as root
if [ "$EUID" -eq 0 ]; then
    echo "ERROR: Please do not run this script as root"
    echo "Run as: ./install_rpi.sh"
    exit 1
fi

echo "This script will install all dependencies for the Photo Scanner Kiosk"
echo "including Qt6, SANE (scanner support), and required libraries."
echo ""
read -p "Continue? (y/n) " -n 1 -r
echo
if [[ ! $REPLY =~ ^[Yy]$ ]]; then
    exit 0
fi

echo ""
echo "Updating package list..."
sudo apt-get update

echo ""
echo "Installing build tools..."
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    pkg-config

echo ""
echo "Installing Qt6 and dependencies..."
sudo apt-get install -y \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-declarative-dev \
    qml6-module-qtquick \
    qml6-module-qtquick-controls \
    qml6-module-qtquick-layouts \
    qml6-module-qtquick-window \
    qml6-module-qtqml-workerscript \
    qml6-module-qtquick-templates \
    libqt6core6 \
    libqt6gui6 \
    libqt6quick6 \
    libqt6qml6 \
    libqt6network6

echo ""
echo "Installing SANE (scanner support) for Fujitsu fi-800R..."
sudo apt-get install -y \
    sane \
    sane-utils \
    libsane-dev \
    libsane-extras

echo ""
echo "Installing image processing libraries..."
sudo apt-get install -y \
    libjpeg-dev \
    libtiff-dev \
    libpng-dev

echo ""
echo "Installing networking libraries..."
sudo apt-get install -y \
    libcurl4-openssl-dev \
    libssl-dev

echo ""
echo "Installing QR code generation tool..."
sudo apt-get install -y \
    qrencode

echo ""
echo "Installing Python3 for email support..."
sudo apt-get install -y \
    python3 \
    python3-pip

echo ""
echo "Configuring SANE for Fujitsu fi-800R scanner..."

# Add user to scanner group
sudo usermod -a -G scanner $USER
echo "✓ Added user $USER to scanner group"

# Configure SANE for Fujitsu scanner
if [ ! -f /etc/sane.d/dll.conf.backup ]; then
    sudo cp /etc/sane.d/dll.conf /etc/sane.d/dll.conf.backup
fi

# Enable fujitsu backend
if ! grep -q "^fujitsu$" /etc/sane.d/dll.conf; then
    echo "fujitsu" | sudo tee -a /etc/sane.d/dll.conf > /dev/null
    echo "✓ Enabled fujitsu backend in SANE"
fi

# Enable epsonds backend (alternative for fi-800R)
if ! grep -q "^epsonds$" /etc/sane.d/dll.conf; then
    echo "epsonds" | sudo tee -a /etc/sane.d/dll.conf > /dev/null
    echo "✓ Enabled epsonds backend in SANE"
fi

echo ""
echo "Setting up scanner permissions..."
sudo tee /etc/udev/rules.d/65-fujitsu-scanner.rules > /dev/null <<EOF
# Fujitsu fi-800R Scanner
# PFU (Fujitsu) USB scanners
ATTRS{idVendor}=="04c5", MODE="0666", GROUP="scanner"

# Alternative vendor IDs for Fujitsu scanners
ATTRS{idVendor}=="0686", MODE="0666", GROUP="scanner"
EOF

sudo udevadm control --reload-rules
sudo udevadm trigger
echo "✓ Scanner udev rules configured"

echo ""
echo "Creating application directories..."
mkdir -p ~/kiosk_app/scans
mkdir -p ~/kiosk_app/logs
mkdir -p ~/kiosk_app/scans/failed_deliveries
echo "✓ Directories created"

echo ""
echo "=========================================="
echo "Installation complete!"
echo "=========================================="
echo ""
echo "IMPORTANT NOTES:"
echo "1. You must LOG OUT and LOG BACK IN for scanner group permissions to take effect"
echo "2. After logging back in, connect your Fujitsu fi-800R scanner"
echo "3. Test scanner detection with: scanimage -L"
echo "4. Build the application with:"
echo "   cd kiosk_app_cpp"
echo "   mkdir build && cd build"
echo "   cmake .."
echo "   make"
echo "5. Run the application with:"
echo "   ./PhotoScannerKiosk"
echo "   or in demo mode:"
echo "   DEMO_MODE=true ./PhotoScannerKiosk --demo"
echo ""
echo "For SSH access from your local machine:"
echo "   ssh erhan@192.168.1.233"
echo ""
