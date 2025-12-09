# Quick Start Guide - Photo Scanner Kiosk

This guide will help you get the kiosk application running on your Raspberry Pi 4.

## Prerequisites

- Raspberry Pi 4 with Raspberry Pi OS installed
- Fujitsu fi-800R scanner
- Internet connection
- SSH access configured (optional for remote setup)

## Step-by-Step Setup

### 1. Transfer Files to Raspberry Pi

From your local machine:

```bash
# Option 1: Using SCP
scp -r kiosk_app_cpp erhan@192.168.1.233:~/

# Option 2: Using Git (if you have a repository)
ssh erhan@192.168.1.233
git clone <your-repo-url> kiosk_app_cpp
```

### 2. SSH into Raspberry Pi

```bash
ssh erhan@192.168.1.233
```

### 3. Run Installation Script

```bash
cd ~/kiosk_app_cpp
chmod +x install_rpi.sh
./install_rpi.sh
```

This will install all necessary dependencies. **Important**: After this completes, you must log out and log back in.

### 4. Log Out and Back In

```bash
exit  # Log out
ssh erhan@192.168.1.233  # Log back in
```

This activates the scanner group permissions.

### 5. Connect Scanner

1. Connect your Fujitsu fi-800R scanner via USB
2. Power on the scanner
3. Verify detection:

```bash
scanimage -L
```

You should see output like:
```
device `fujitsu:ScanSnap fi-800R:xxxxx' is a FUJITSU fi-800R scanner
```

### 6. Build the Application

```bash
cd ~/kiosk_app_cpp
chmod +x build.sh
./build.sh release
```

### 7. Test in Demo Mode (Without Hardware)

```bash
cd build
DEMO_MODE=true ./PhotoScannerKiosk --demo
```

This will run the application with simulated scanner and payment processing.

### 8. Run with Real Hardware

```bash
cd build
./PhotoScannerKiosk
```

For fullscreen:
```bash
./PhotoScannerKiosk --fullscreen
```

## Environment Configuration

### Email Settings

For email delivery to work, set these environment variables:

```bash
export EMAIL_USER=your_gmail@gmail.com
export EMAIL_PASSWORD=your_app_specific_password
export SMTP_SERVER=smtp.gmail.com
export SMTP_PORT=587
```

**Note**: For Gmail, you need to generate an [App Password](https://support.google.com/accounts/answer/185833).

### Square Payment Settings

For payment processing:

```bash
export SQUARE_ACCESS_TOKEN=your_square_access_token
export SQUARE_LOCATION_ID=your_location_id

# For production:
export SQUARE_API_BASE=https://connect.squareup.com

# For sandbox/testing:
export SQUARE_API_BASE=https://connect.squareupsandbox.com
```

### Persistent Configuration

To make these permanent, add them to `~/.bashrc`:

```bash
nano ~/.bashrc
```

Add at the end:
```bash
# Kiosk Application Settings
export EMAIL_USER=your_gmail@gmail.com
export EMAIL_PASSWORD=your_app_password
export SQUARE_ACCESS_TOKEN=your_token
export SQUARE_LOCATION_ID=your_location
export SQUARE_API_BASE=https://connect.squareupsandbox.com
export KIOSK_FULLSCREEN=true
```

Then reload:
```bash
source ~/.bashrc
```

## Testing Checklist

- [ ] Scanner detected with `scanimage -L`
- [ ] Application builds without errors
- [ ] Demo mode runs and shows all screens
- [ ] Logo screen appears and responds to tap
- [ ] Phone number input works
- [ ] Credit selection displays correctly
- [ ] Payment screen appears
- [ ] Scanner executes scan (real mode)
- [ ] Images are processed and saved
- [ ] Email delivery works (or saves locally)
- [ ] Confirmation screen shows
- [ ] Inactivity timeout resets after 60 seconds

## Troubleshooting

### Scanner Not Detected

```bash
# Check if scanner is connected
lsusb | grep -i fujitsu

# Check SANE configuration
cat /etc/sane.d/dll.conf | grep fujitsu

# Check user permissions
groups | grep scanner

# If not in scanner group:
sudo usermod -a -G scanner $USER
# Then log out and back in
```

### Build Fails

```bash
# Check Qt6 installation
dpkg -l | grep qt6

# If missing, reinstall:
sudo apt-get install qt6-base-dev
```

### Display Issues

```bash
# Check X server
echo $DISPLAY  # Should show :0 or :0.0

# If not set:
export DISPLAY=:0
```

### Application Crashes

```bash
# Run with debug output
QT_DEBUG_PLUGINS=1 ./PhotoScannerKiosk
```

## Auto-Start on Boot

To automatically launch the kiosk when the Pi boots:

1. Create systemd service:
```bash
sudo nano /etc/systemd/system/photo-kiosk.service
```

2. Add content:
```ini
[Unit]
Description=Photo Scanner Kiosk
After=graphical.target

[Service]
Type=simple
User=erhan
Environment=DISPLAY=:0
Environment=KIOSK_FULLSCREEN=true
WorkingDirectory=/home/erhan/kiosk_app_cpp/build
ExecStart=/home/erhan/kiosk_app_cpp/build/PhotoScannerKiosk --fullscreen
Restart=on-failure

[Install]
WantedBy=graphical.target
```

3. Enable and start:
```bash
sudo systemctl enable photo-kiosk.service
sudo systemctl start photo-kiosk.service
```

4. Check status:
```bash
sudo systemctl status photo-kiosk.service
```

## Development Workflow

### Making Changes

1. Edit code on your local machine
2. Transfer to Pi:
```bash
rsync -avz kiosk_app_cpp/ erhan@192.168.1.233:~/kiosk_app_cpp/
```

3. SSH in and rebuild:
```bash
ssh erhan@192.168.1.233
cd ~/kiosk_app_cpp/build
make -j4
```

4. Test:
```bash
./PhotoScannerKiosk
```

### Debugging

Enable verbose logging:
```bash
export QT_LOGGING_RULES='*.debug=true'
./PhotoScannerKiosk 2>&1 | tee kiosk.log
```

View logs:
```bash
tail -f ~/kiosk_app/logs/kiosk.log
```

## Next Steps

1. Configure production Square credentials
2. Set up email with your domain
3. Test all hardware integrations
4. Set up auto-start service
5. Configure touchscreen if needed
6. Test full user workflow
7. Train staff on troubleshooting

## Support

If you encounter issues:
1. Check the main README.md for detailed documentation
2. Review logs in `~/kiosk_app/logs/`
3. Test scanner separately with `scanimage`
4. Run in demo mode to isolate hardware issues

---

**Important**: Always test in demo mode first before attempting to use real scanner and payment hardware!
