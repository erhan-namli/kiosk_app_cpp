# Testing Modes

The Photo Scanner Kiosk supports multiple testing modes for development and deployment.

## Available Modes

### 1. Production Mode (Default)
Full application with real hardware and payment processing.

```bash
cd build
./PhotoScannerKiosk
```

**Features:**
- Real Fujitsu fi-800R scanner
- Real Square payment processing
- Email delivery
- Full workflow

---

### 2. Demo Mode
Mock everything - no hardware or payment needed. Perfect for UI testing.

```bash
DEMO_MODE=true ./PhotoScannerKiosk --demo
```

**Features:**
- ✅ Mock scanner (generates fake scans)
- ✅ Mock payment (auto-approves after 3 seconds)
- ✅ Full UI workflow
- ✅ No hardware required
- ✅ No payment API calls

**Use cases:**
- UI/UX testing
- Development without hardware
- Demo presentations

---

### 3. Scanner-Only Mode
Real scanner testing without payment processing. Perfect for scanner hardware testing.

```bash
SCANNER_ONLY=true ./PhotoScannerKiosk --scanner-only
```

**Features:**
- ✅ Real Fujitsu fi-800R scanner
- ✅ Skips payment (auto-approves immediately)
- ✅ Full scanning workflow
- ✅ Email delivery (if configured)
- ❌ No payment API calls

**Use cases:**
- Testing scanner hardware
- Verifying scanner configuration
- Testing scan quality and cropping
- GUI testing with real scanner

---

## Command Line Options

### `--demo`
Enables demo mode (requires `DEMO_MODE=true` environment variable)

### `--scanner-only`
Enables scanner-only mode (requires `SCANNER_ONLY=true` environment variable)

### `--fullscreen`
Runs application in fullscreen mode (for kiosk deployment)

---

## Environment Variables

### `DEMO_MODE=true`
Activates demo mode

### `SCANNER_ONLY=true`
Activates scanner-only mode

### `KIOSK_FULLSCREEN=true`
Enables fullscreen mode by default

### Scanner Configuration
- `SCANNER_DEVICE` - Override scanner device name

### Square Payment API
- `SQUARE_ACCESS_TOKEN` - Square API access token
- `SQUARE_LOCATION_ID` - Square location ID
- `SQUARE_API_BASE` - Square API base URL

### Email Configuration
- `SMTP_SERVER` - SMTP server (default: smtp.gmail.com)
- `SMTP_PORT` - SMTP port (default: 587)
- `EMAIL_USER` - Email username
- `EMAIL_PASSWORD` - Email password
- `SENDER_EMAIL` - Sender email address

---

## Examples

### Test UI without any hardware
```bash
cd build
DEMO_MODE=true ./PhotoScannerKiosk --demo
```

### Test scanner hardware only
```bash
cd build
SCANNER_ONLY=true ./PhotoScannerKiosk --scanner-only
```

### Production deployment
```bash
cd build
./PhotoScannerKiosk --fullscreen
```

### Production with custom scanner device
```bash
cd build
SCANNER_DEVICE='fujitsu:fi-800R:1700265' ./PhotoScannerKiosk --fullscreen
```

---

## Troubleshooting

### Scanner not detected
1. Check scanner is connected: `lsusb | grep Fujitsu`
2. Test SANE detection: `scanimage -L`
3. Check udev rules: `/etc/udev/rules.d/65-fujitsu-scanner.rules`
4. Verify user in scanner group: `groups`

### Payment errors
- In demo mode: Payment is mocked, no API calls
- In scanner-only mode: Payment is skipped
- In production mode: Check Square API credentials

### Display issues
- Run with `DISPLAY=:0` if using SSH
- Or use VNC to access Pi desktop directly
