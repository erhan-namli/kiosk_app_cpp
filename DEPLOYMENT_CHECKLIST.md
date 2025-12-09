# Deployment Checklist - Photo Scanner Kiosk

Use this checklist when deploying the kiosk application to a Raspberry Pi.

## Pre-Deployment

### Hardware Preparation
- [ ] Raspberry Pi 4 (2GB+ RAM) obtained
- [ ] MicroSD card (32GB+) formatted with Raspberry Pi OS
- [ ] Fujitsu fi-800R scanner available
- [ ] USB cable for scanner
- [ ] Touchscreen display connected (1024x768+)
- [ ] Power supplies for Pi and scanner
- [ ] Network connection configured (Ethernet or WiFi)
- [ ] Keyboard/mouse for initial setup

### Software Preparation
- [ ] Raspberry Pi OS installed and updated
- [ ] SSH enabled on Pi
- [ ] Pi accessible on network (test with ping)
- [ ] Source code repository ready
- [ ] Environment variables documented
- [ ] Square API credentials obtained
- [ ] Email SMTP credentials obtained

## Installation Phase

### 1. File Transfer
- [ ] Files transferred to Pi (SCP/rsync)
```bash
scp -r kiosk_app_cpp erhan@192.168.1.233:~/
```
- [ ] File permissions correct
- [ ] All files present (check with `ls -R`)

### 2. Dependency Installation
- [ ] SSH into Pi
```bash
ssh erhan@192.168.1.233
```
- [ ] Make install script executable
```bash
chmod +x ~/kiosk_app_cpp/install_rpi.sh
```
- [ ] Run installation script
```bash
cd ~/kiosk_app_cpp
./install_rpi.sh
```
- [ ] Installation completed without errors
- [ ] Log out and log back in (IMPORTANT!)
- [ ] Verify scanner group membership
```bash
groups | grep scanner
```

### 3. Scanner Setup
- [ ] Scanner connected via USB
- [ ] Scanner powered on
- [ ] Scanner detected by system
```bash
lsusb | grep -i fujitsu
```
- [ ] SANE can detect scanner
```bash
scanimage -L
```
- [ ] Expected output shows fi-800R

### 4. Build Application
- [ ] Make build script executable
```bash
chmod +x ~/kiosk_app_cpp/build.sh
```
- [ ] Run build in release mode
```bash
./build.sh release
```
- [ ] Build completed without errors
- [ ] Executable created
```bash
ls -lh build/PhotoScannerKiosk
```

## Testing Phase

### 5. Demo Mode Testing
- [ ] Run in demo mode
```bash
cd build
DEMO_MODE=true ./PhotoScannerKiosk --demo
```
- [ ] Logo screen appears
- [ ] Tap logo advances to phone input
- [ ] Phone number entry works (try: 5551234567)
- [ ] Credit selection displays correctly
- [ ] Tap to pay screen shows
- [ ] Simulated payment completes
- [ ] Scanner screen shows alignment guide
- [ ] Scan simulation works
- [ ] Confirmation screen displays
- [ ] Returns to logo after timeout (60s)
- [ ] Close application (Ctrl+C or close window)

### 6. Scanner Hardware Testing
- [ ] Test scanner independently
```bash
scanimage --format=tiff --resolution=600 > test.tiff
```
- [ ] TIFF file created successfully
- [ ] Open and verify test.tiff
- [ ] Delete test file
```bash
rm test.tiff
```

### 7. Configuration
- [ ] Create environment file
```bash
nano ~/.bashrc
```
- [ ] Add configuration (see below)
- [ ] Reload configuration
```bash
source ~/.bashrc
```
- [ ] Verify variables set
```bash
env | grep -E "EMAIL|SQUARE|KIOSK"
```

#### Required Environment Variables
```bash
# Email Configuration
export EMAIL_USER=your_email@gmail.com
export EMAIL_PASSWORD=your_app_password
export SMTP_SERVER=smtp.gmail.com
export SMTP_PORT=587
export SENDER_EMAIL=your_email@gmail.com

# Square Payment Configuration
export SQUARE_ACCESS_TOKEN=your_access_token
export SQUARE_LOCATION_ID=your_location_id
export SQUARE_API_BASE=https://connect.squareupsandbox.com

# Application Settings
export KIOSK_FULLSCREEN=true
export DEMO_MODE=false
```

### 8. Production Testing
- [ ] Run application in normal mode
```bash
cd ~/kiosk_app_cpp/build
./PhotoScannerKiosk
```
- [ ] Application starts
- [ ] Scanner detected (check logs)
- [ ] Complete full workflow:
  - [ ] Enter phone number
  - [ ] Select credits
  - [ ] Process payment
  - [ ] Perform real scan
  - [ ] Image processed correctly
  - [ ] Email sent successfully (or saved locally)
  - [ ] Confirmation displayed
- [ ] Close application

### 9. Fullscreen Testing
- [ ] Run in fullscreen mode
```bash
./PhotoScannerKiosk --fullscreen
```
- [ ] Application fills entire screen
- [ ] UI elements scaled correctly
- [ ] Touch interface responsive
- [ ] Exit fullscreen (Alt+F4 or configured exit method)

## Production Deployment

### 10. Auto-Start Configuration
- [ ] Create systemd service file
```bash
sudo nano /etc/systemd/system/photo-kiosk.service
```
- [ ] Add service configuration (see template below)
- [ ] Reload systemd
```bash
sudo systemctl daemon-reload
```
- [ ] Enable service
```bash
sudo systemctl enable photo-kiosk.service
```
- [ ] Start service
```bash
sudo systemctl start photo-kiosk.service
```
- [ ] Check service status
```bash
sudo systemctl status photo-kiosk.service
```
- [ ] Verify application running

#### Systemd Service Template
```ini
[Unit]
Description=Photo Scanner Kiosk
After=graphical.target network.target

[Service]
Type=simple
User=erhan
Environment=DISPLAY=:0
Environment=KIOSK_FULLSCREEN=true
Environment=DEMO_MODE=false
WorkingDirectory=/home/erhan/kiosk_app_cpp/build
ExecStart=/home/erhan/kiosk_app_cpp/build/PhotoScannerKiosk --fullscreen
Restart=always
RestartSec=10

[Install]
WantedBy=graphical.target
```

### 11. System Configuration
- [ ] Disable screen blanking
```bash
sudo nano /etc/lightdm/lightdm.conf
# Add: xserver-command=X -s 0 -dpms
```
- [ ] Configure touchscreen calibration (if needed)
- [ ] Set display resolution
- [ ] Configure auto-login (if desired)
- [ ] Set timezone
```bash
sudo raspi-config  # 5 Localisation Options > L2 Timezone
```

### 12. Security Hardening
- [ ] Change default password
```bash
passwd
```
- [ ] Disable unused services
- [ ] Configure firewall (if needed)
```bash
sudo apt-get install ufw
sudo ufw allow 22/tcp  # SSH
sudo ufw enable
```
- [ ] Set up SSH key authentication (recommended)
- [ ] Remove unnecessary packages
- [ ] Update system
```bash
sudo apt-get update && sudo apt-get upgrade -y
```

### 13. Monitoring Setup
- [ ] Create log rotation
```bash
sudo nano /etc/logrotate.d/photo-kiosk
```
- [ ] Set up disk space monitoring
- [ ] Configure email alerts (optional)
- [ ] Document monitoring procedures

### 14. Backup Configuration
- [ ] Backup configuration files
```bash
tar -czf kiosk-config-backup.tar.gz ~/.bashrc /etc/systemd/system/photo-kiosk.service
```
- [ ] Document all settings
- [ ] Store credentials securely
- [ ] Test restore procedure

## Verification

### 15. Final Testing
- [ ] Reboot Raspberry Pi
```bash
sudo reboot
```
- [ ] Wait for system to boot
- [ ] Application starts automatically
- [ ] Scanner operational
- [ ] Network connectivity confirmed
- [ ] Complete test transaction
- [ ] Monitor for 1 hour (stability test)
- [ ] Check logs for errors
```bash
tail -f ~/kiosk_app/logs/kiosk.log
```

### 16. Performance Verification
- [ ] Application responds within 2 seconds
- [ ] Scanning completes in under 30 seconds
- [ ] Image processing under 5 seconds
- [ ] Email delivery successful
- [ ] Memory usage acceptable
```bash
top  # Look for PhotoScannerKiosk process
```
- [ ] No memory leaks after 10 transactions
- [ ] Inactivity timeout works (60s)

### 17. User Acceptance
- [ ] UI elements clearly visible
- [ ] Touch targets easily tappable
- [ ] Instructions clear and understandable
- [ ] Error messages helpful
- [ ] Scanner alignment guide effective
- [ ] Overall user experience smooth

## Documentation

### 18. Final Documentation
- [ ] Document all configuration settings
- [ ] Create operator manual
- [ ] List troubleshooting procedures
- [ ] Document backup/restore process
- [ ] Create maintenance schedule
- [ ] Prepare support contact information

### 19. Training
- [ ] Train staff on normal operation
- [ ] Show how to restart application
- [ ] Demonstrate troubleshooting steps
- [ ] Explain maintenance tasks
- [ ] Practice recovery procedures

## Handoff

### 20. Deployment Sign-Off
- [ ] All checklist items completed
- [ ] System running stably for 24 hours
- [ ] Backup created and tested
- [ ] Documentation provided
- [ ] Training completed
- [ ] Support contact established
- [ ] Final acceptance obtained

## Post-Deployment

### 21. Monitoring (First Week)
- [ ] Check daily for errors
- [ ] Monitor transaction success rate
- [ ] Verify email delivery
- [ ] Check scanner performance
- [ ] Review system logs
- [ ] Track user issues

### 22. Ongoing Maintenance
- [ ] Weekly log review
- [ ] Monthly system updates
- [ ] Quarterly security review
- [ ] Annual hardware inspection
- [ ] Regular backup verification

---

## Emergency Contacts

**Technical Support**: _________________
**Scanner Support**: _________________
**Square Support**: _________________
**System Administrator**: _________________

## Important Commands Reference

```bash
# Start application manually
cd ~/kiosk_app_cpp/build && ./PhotoScannerKiosk

# Check service status
sudo systemctl status photo-kiosk.service

# Restart service
sudo systemctl restart photo-kiosk.service

# View logs
tail -f ~/kiosk_app/logs/kiosk.log

# Test scanner
scanimage -L

# Check system resources
top
df -h
free -h
```

---

**Deployment Date**: ___________
**Deployed By**: ___________
**Location**: ___________
**Pi Hostname/IP**: ___________
**Notes**: ___________

