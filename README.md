# ECE/MAE 148 — UCSD Robocar Documentation Website

**Repository:** `ECEMAE148_RPI_Documentation.github.io`  
**Hosted via:** GitHub Pages  
**Maintained by:** UCSD Jacobs School of Engineering — ECE/MAE 148 Course Staff  
**Purpose:** A multi-page, fully static HTML reference guide for students, TAs, and instructors working with the UCSD Robocar platform in ECE/MAE 148: Introduction to Autonomous Vehicles.

---

## Table of Contents

1. [Project Overview](#1-project-overview)
2. [Repository File Structure](#2-repository-file-structure)
3. [Page-by-Page Breakdown](#3-page-by-page-breakdown)
   - [rpi_setup.html — Raspberry Pi Setup](#rpi_setuphtml--raspberry-pi-setup)
   - [vesc_setup.html — VESC Motor Controller Setup](#vesc_setuphtml--vesc-motor-controller-setup)
   - [donkeycar.html — DonkeyCar Framework](#donkeycarhtml--donkeycar-framework)
   - [gps_laps.html — GPS Lap Recording](#gps_lapshtml--gps-lap-recording)
   - [index.html — ROS2 Setup & Calibration Guide](#indexhtml--ros2-setup--calibration-guide)
   - [genai_setup.html — GenAI / Copilot Setup Guide](#genai_setuphtml--genai--copilot-setup-guide)
4. [Recommended Reading Order for Students](#4-recommended-reading-order-for-students)
5. [Site Architecture & Design System](#5-site-architecture--design-system)
   - [CSS Design Tokens (Variables)](#css-design-tokens-variables)
   - [Shared Layout Components](#shared-layout-components)
   - [Interactive JavaScript Features](#interactive-javascript-features)
6. [How to Make Changes](#6-how-to-make-changes)
   - [Editing Existing Content](#editing-existing-content)
   - [Adding a New Section to an Existing Page](#adding-a-new-section-to-an-existing-page)
   - [Adding a New Page](#adding-a-new-page)
   - [Updating the Navigation Between Pages](#updating-the-navigation-between-pages)
7. [Content Component Reference](#7-content-component-reference)
   - [Code Blocks (`.cb`)](#code-blocks-cb)
   - [Callout Boxes (`.callout`)](#callout-boxes-callout)
   - [Tables (`.tw`)](#tables-tw)
   - [Step Cards (`.steps` / `.step-card`)](#step-cards-steps--step-card)
   - [Info Cards and Grid Layouts](#info-cards-and-grid-layouts)
   - [Section Dividers](#section-dividers)
8. [Deploying Changes via GitHub Pages](#8-deploying-changes-via-github-pages)
9. [Style and Tone Guidelines](#9-style-and-tone-guidelines)
10. [Common Mistakes to Avoid](#10-common-mistakes-to-avoid)
11. [Versioning and Change Log Conventions](#11-versioning-and-change-log-conventions)

---

## 1. Project Overview

This is a **fully static, no-build-system website** — there is no JavaScript framework, no Node.js, no bundler, and no backend. Every page is a single self-contained `.html` file that includes its own CSS (in a `<style>` block in the `<head>`) and its own JavaScript (in a `<script>` block before `</body>`).

The site is hosted on **GitHub Pages** directly from the `main` branch. Any change pushed to `main` is automatically live within a few minutes — there is no separate deployment pipeline to manage.

The site covers the full hardware and software stack used in ECE/MAE 148, organized into six logical guides that a student should work through in sequence. The visual design uses the UCSD brand color palette (navy, gold, and a dark blue-grey background) and is optimized for readability during lab sessions on a laptop or secondary monitor.

---

## 2. Repository File Structure

```
ECEMAE148_RPI_Documentation.github.io/
│
├── index.html          ← ROS2 Setup & Calibration Guide (the main/landing page)
├── genai_setup.html    ← GenAI / Copilot workflow guide for ROS2 debugging and development
├── rpi_setup.html      ← Raspberry Pi OS flashing and configuration
├── vesc_setup.html     ← VESC motor controller firmware and calibration
├── donkeycar.html      ← DonkeyCar framework install, patching, training
├── gps_laps.html       ← GPS RTK corrections, path recording, PID tuning
│
└── README.md           ← This file
```

> **Important naming note:** The file currently named `index__1_.html` in some local copies is the canonical `index.html` that lives at the root of the repository and serves as the GitHub Pages landing page. Always keep the main ROS2 guide named exactly `index.html`. GitHub Pages will serve this file automatically at the root URL.

All six HTML files are flat at the root level of the repository. There are no subdirectories, no asset folders, no external CSS or JS files stored in the repo. All fonts are loaded from Google Fonts via CDN and all styling is embedded in each file's `<style>` block.

---

## 3. Page-by-Page Breakdown

### `rpi_setup.html` — Raspberry Pi Setup

**Tab label in the top navigation:** `RPI Setup`  
**Breadcrumb shown in the top bar:** `Raspberry Pi OS Setup Guide`  
**Audience:** Students who have a brand-new Raspberry Pi 5 and need to flash and configure the OS before any robotics software can be installed.

**What this page covers (in order):**

| Section # | Title | Description |
|-----------|-------|-------------|
| 01 | Install Raspberry Pi Imager | Where to download the imager tool on macOS, Windows, or Linux |
| 02 | Flash the OS — Choose the Correct Image | How to select the correct Legacy 64-bit OS for DonkeyCar 5.2.x compatibility |
| 03 | Configure Hostname | Setting the robot's hostname (e.g., `robotcar01`) before flashing |
| 04 | Set Username, Password & WiFi | Pre-configuring credentials and network SSID in the imager |
| 05 | Enable SSH & Write the Image | Enabling SSH in the imager and writing to the 128GB microSD card |
| 06 | Cooling & Hardware Assembly | Fan/heatsink installation, mounting the Pi to the car chassis |
| 07 | First Boot & SSH Connection | Initial SSH login, verifying the Pi is on the network |
| 08 | WiFi Configuration | Editing `wpa_supplicant.conf` for additional networks (lab, home, hotspot) |
| 09 | System Settings | `raspi-config` settings: expand filesystem, I2C, camera, VNC |
| 10 | Date & Time Configuration | Ensuring correct timezone and NTP sync to prevent build errors |
| 11 | GUI Settings & Filesystem Expansion | VNC server configuration, expanding the SD card partition |
| 12 | Passwordless SSH (Key-Based Auth) | Generating and copying SSH keys for convenience |
| 13 | Backup & Troubleshooting | Imaging the SD card as a backup, common first-boot failures |

**Key hardware chips shown in the hero section:** Raspberry Pi 5, 128GB microSD, Python 3.11, DonkeyCar 5.2.x

---

### `vesc_setup.html` — VESC Motor Controller Setup

**Tab label in the top navigation:** `VESC Setup`  
**Breadcrumb shown in the top bar:** `VESC Setup & Motor Calibration Guide`  
**Audience:** Students who have received a VESC (Vedder Electronic Speed Controller) and need to flash firmware, run motor detection, and configure the servo output for steering before the car can move.

**What this page covers (in order):**

| Section # | Title | Description |
|-----------|-------|-------------|
| 01 | Overview & Safety | Critical safety warnings (motor spins during detection — secure the car) |
| 02 | Required Hardware | VESC unit, USB-A cable, servo, motor — hardware checklist |
| 03 | Install VESC Tool | Downloading VESC Tool from vesc-project.com for host PC |
| 04 | Connect the VESC to Your Host PC | USB connection, identifying the serial port on macOS/Linux/Windows |
| 05 | Firmware Update — VESC 6.x | Step-by-step firmware flashing for modern VESC 6.x hardware |
| 06 | Firmware Update — VESC 4.x Legacy | Alternate firmware path for older VESC 4.x hardware |
| 07 | Cable Connections | Motor phase wires, hall sensor connector, servo PWM pin-out |
| 08 | FOC Motor Detection Wizard | Running the automatic Field-Oriented Control detection wizard |
| 09 | Battery Configuration | Setting battery type, cell count, and voltage cutoff thresholds |
| 10 | Motor Parameters | Manual parameter review after auto-detection |
| 11 | Run Detection & Motor Direction | Confirming motor spins in the correct direction; polarity correction |
| 12 | Hall Sensor Detection | Running hall sensor detection for accurate low-speed position feedback |
| 13 | Enable Servo Steering Output | Enabling PPM output on the VESC for servo/steering control |
| 14 | Troubleshooting & Known Issues | Common VESC errors, fault codes, and recovery steps |

---

### `donkeycar.html` — DonkeyCar Framework

**Tab label in the top navigation:** `DonkeyCar`  
**Breadcrumb shown in the top bar:** `DonkeyCar AI Driving Framework`  
**Audience:** Students using the DonkeyCar imitation-learning pipeline — driving the car manually to collect training data, training a neural network on a laptop, and deploying it back to the Pi for autonomous driving.

**What this page covers (in order):**

| Section # | Title | Description |
|-----------|-------|-------------|
| 01 | Raspberry Pi System Prep | Verifying the Pi is ready and updating packages before DonkeyCar install |
| 02 | DonkeyCar Installation | Installing DonkeyCar 5.2.x, creating the `mycar` application |
| 03 | Camera Setup — Oak-D Lite | Configuring the Luxonis OAK-D Lite depth camera as the primary input |
| 04 | Motor Controller — VESC | Configuring DonkeyCar's VESC driver in `myconfig.py` |
| 05 | Joystick — Logitech F710 | Pairing the F710 joystick for manual RC driving and data collection |
| 06 | TensorFlow Verification | Confirming TF is installed and accessible inside the DonkeyCar environment |
| 07 | Patch VESC.py — Firmware Compatibility | Applying a required patch to fix VESC firmware version string parsing |
| 08 | Patch tub_v2.py — Image File Naming | Patching the data-recording module to fix image filename generation |
| 09 | Patch oak_d.py — Separate RGB and Depth Queues | Patching the OAK-D driver to correctly separate color and depth streams |
| 10 | myconfig.py — Full Hardware Configuration | Complete annotated reference for all `myconfig.py` settings |
| 11 | Driving & Data Collection | How to start the car, drive with the joystick, and record tub data |
| 12 | Training the Model | Running `donkey train` on a laptop with GPU or on the Pi |
| 13 | Autonomous Driving | Loading the trained model and switching to autopilot mode |
| 14 | Troubleshooting & Known Issues | Camera errors, VESC disconnects, joystick pairing, training failures |

> **Note for future TAs:** The three patch sections (07, 08, 09) are particularly important. These patches address bugs in specific versions of DonkeyCar and may need to be updated or removed if the upstream DonkeyCar project fixes them in a future release. Always verify whether a patch is still needed by checking against the current DonkeyCar version at the start of each term.

---

### `gps_laps.html` — GPS Lap Recording

**Tab label in the top navigation:** `GPS Laps`  
**Breadcrumb shown in the top bar:** `GPS RTK Lap Recording & Path Playback`  
**Audience:** Students working on the GPS-guided path recording project — recording a driven route with RTK-corrected GPS and playing it back autonomously.

**What this page covers (in order):**

| Section # | Title | Description |
|-----------|-------|-------------|
| 01 | Hardware Setup & Location | GPS module placement on the car (roof-mounted for clear sky view) |
| 02 | User Permissions & Miniforge Install | Adding the user to the `dialout` group; installing Miniforge (conda) |
| 03 | Python Environment & GPS Configuration | Creating the conda environment; configuring the GPS serial port |
| 04 | Running GPS RTK Corrections | Connecting to UCSD's RTK correction service for centimeter-level accuracy |
| 05 | DonkeyCar Setup | Configuring DonkeyCar specifically for GPS-mode operation |
| 06 | Driving & Recording a Path | Using the joystick to manually drive and record a GPS path/tub |
| 07 | Autonomous Path Playback | Starting the GPS follower to replay the recorded route autonomously |
| 08 | PID Tuning Guide | Tuning the steering and throttle PID controllers for smooth path following |
| 09 | Troubleshooting & Known Issues | GPS fix failures, serial port errors, RTK connection issues |

> **Note for future TAs:** The RTK correction service endpoint (Section 04) is specific to UCSD's network infrastructure. If the campus NTRIP server address or credentials change, this section must be updated promptly or students will be unable to obtain RTK corrections and will fall back to lower-accuracy standalone GPS.

---

### `index.html` — ROS2 Setup & Calibration Guide

**Tab label in the top navigation:** `ROS Setup` *(marked `active`)*  
**Breadcrumb shown in the top bar:** `Setup & Calibration Guide`  
**Audience:** Students who have completed the Raspberry Pi and VESC setup and are now working within the UCSD Robocar ROS2 Docker environment for lane detection and autonomous driving.

**This is the primary and most comprehensive page of the site.** It covers the full ROS2 Docker workflow and calibration pipeline.

**What this page covers (in order):**

| Section # | Title | Description |
|-----------|-------|-------------|
| 01 | Docker Setup | Installing Docker on the Raspberry Pi, managing permissions |
| 02 | Pull Robocar Image | Pulling the `djnighti/ucsd_robocar` Docker image from Docker Hub |
| 03 | X11 Forwarding | Configuring SSH X11 forwarding (`ssh -Y`) so GUI apps run inside Docker over SSH |
| 04 | Run Container | The full `docker run` command with all required flags and volume mounts |
| 05 | Python Error Fix | Resolving the common `externally-managed-environment` Python pip error inside Docker |
| 06 | Build Workspace | Running `build_ros2` to compile all ROS2 packages after cloning or modifying code |
| 07 | Shell Aliases | Explanation of the custom bash aliases (`build_ros2`, `source_ros2`, etc.) baked into the container |
| 08 | Camera Power Fix | Using a powered USB hub to resolve OAK-D Lite brownout/disconnect issues |
| 09 | Camera Isolation (ROS_DOMAIN_ID) | Setting `ROS_DOMAIN_ID` to prevent interference between teams on the same network |
| 10 | Config Overview | Overview of the `car_config.yaml` and `ros_racer_calibration.yaml` configuration files |
| 11 | Step 1 · Calibration Mode | Launching the car in calibration mode before running autonomous navigation |
| 12 | Step 2 · HSV Tuning (Track) | Using the HSV slider GUI to tune color thresholds for lane line detection |
| 13 | Step 3 · Lane Calibration | Setting `camera_centerline`, `camera_width`, and `error_threshold` |
| 14 | Step 4 · Navigation & PID | Configuring `Kp_steering`, `Ki_steering`, `Kd_steering`, and `max_throttle` for the PID controller |
| 15 | ROS2 Node Development | How to write custom ROS2 Python nodes and integrate them into the existing package structure |
| 16 | Troubleshooting & Known Issues | Categorized reference table covering Docker, X11, camera, build, and PID issues |

---

### `genai_setup.html` — GenAI / Copilot Setup Guide

**Tab label in the top navigation:** `GenAI Setup`  
**Breadcrumb shown in the top bar:** `GenAI Setup Guide`  
**Audience:** Students using AI assistants to accelerate ROS2 debugging, code scaffolding, and project documentation while keeping the robot and build workflow as their final validation source.

**What this page covers (in order):**

| Section # | Title | Description |
|-----------|-------|-------------|
| 01 | Overview | Best practices for using GenAI as a debugging accelerator rather than a replacement for validation |
| 02 | Recommended Workflow | Short, testable iteration loop for ROS2 and Python troubleshooting |
| 03 | Prompting Guide | A reusable prompt template for ROS2 debugging and node scaffolding |
| 04 | Guardrails | Rules for safe AI use, including validation, privacy, and hardware verification |

---

## 4. Recommended Reading Order for Students

Students should work through the six guides in this order:

```
1. rpi_setup.html     → Flash and configure the Raspberry Pi OS
2. vesc_setup.html    → Flash VESC firmware and calibrate the motor
3. donkeycar.html     → Install DonkeyCar, patch, collect data, train, and drive autonomously
       — OR —
3. index.html         → Set up ROS2 Docker, calibrate lane detection, develop ROS2 nodes
4. gps_laps.html      → (Advanced / optional) GPS RTK path recording and playback
5. genai_setup.html   → Use AI effectively for debugging, prompting, and validation workflows
```

DonkeyCar (`donkeycar.html`) and ROS2 (`index.html`) are parallel tracks — different project teams will use one or the other depending on their assigned project direction. The GPS Laps page (`gps_laps.html`) builds on top of the DonkeyCar framework and is used by teams specifically working on GPS-guided navigation. The GenAI page (`genai_setup.html`) is positioned after ROS Setup in the navigation order and is intended to support debugging and iteration without replacing the hardware and robot validation loop.

---

## 5. Site Architecture & Design System

Each HTML file is entirely self-contained. There is no shared CSS file or shared JavaScript file. This means that **any change to the visual design must be applied to all six files individually.** This is a deliberate trade-off that makes each page portable and deployable independently, at the cost of some duplication.

### CSS Design Tokens (Variables)

All six pages define an identical `:root` block at the top of their `<style>` section. These CSS custom properties control every color, spacing, and font value used across the site:

```css
:root {
  /* UCSD Brand Colors */
  --ucsd-navy:    #002E6D;   /* Dark navy — sidebar header background */
  --ucsd-blue:    #00629B;   /* Medium blue — used in gradients */
  --ucsd-gold:    #C69214;   /* Primary gold — active states, headings */
  --ucsd-gold-lt: #E8B931;   /* Lighter gold — progress bar gradient end */

  /* Background Layers */
  --bg:           #09111E;   /* Page background (darkest) */
  --bg2:          #0D1A2D;   /* Sidebar background */
  --surface:      #111F35;   /* Card/code block background */
  --surface2:     #162643;   /* Slightly lighter surface */
  --surface3:     #1C2F50;   /* Hover surfaces */

  /* Borders */
  --border:       #1E3355;   /* Standard border */
  --border-lt:    #2A4670;   /* Hover/active border */

  /* Text Colors */
  --text:         #DCE8F5;   /* Primary text */
  --text-muted:   #7A9EC4;   /* Secondary text, nav links */
  --text-dim:     #4D6E96;   /* Tertiary text, section numbers */

  /* Accent Colors */
  --accent:       #00A8E8;   /* Cyan — inline code, highlights */
  --accent-glow:  rgba(0,168,232,0.15);
  --gold-glow:    rgba(198,146,20,0.18);
  --green:        #00C896;   /* Tip callout icon */
  --yellow:       #F0B429;   /* Warning callout icon */
  --red:          #F04E37;   /* Danger/error callout icon */
  --purple:       #9B7FE8;   /* Note callout icon */

  /* Code Block */
  --code-bg:      #060E1A;
  --code-border:  #1A2E4A;

  /* Layout */
  --sidebar-w:    290px;     /* Fixed sidebar width */
  --topbar-h:     60px;      /* Fixed top bar height */
  --r:            6px;       /* Standard border radius */

  /* Typography */
  --font-body:    'Sora', sans-serif;
  --font-mono:    'IBM Plex Mono', monospace;
  --ease:         cubic-bezier(0.4,0,0.2,1);
}
```

> **If you ever need to update the brand colors or fonts, you must update this `:root` block in all six HTML files.** A simple find-and-replace across the repository will handle this efficiently.

### Shared Layout Components

Every page uses the same three-panel layout:

```
┌─────────────────────────────────────────────────────────────────┐
│                         TOP BAR (fixed, 60px)                   │
│  [Sidebar Brand] | Breadcrumb        [Page Switcher Tabs]       │
├──────────────┬──────────────────────────────────────────────────┤
│              │                                                   │
│   SIDEBAR    │                   MAIN CONTENT                   │
│  (fixed,     │                                                   │
│   290px)     │   Hero Section                                   │
│              │   ─────────────                                  │
│  Section     │   Section 01                                     │
│  Navigation  │   Section 02                                     │
│  Links       │   ...                                            │
│              │   Section N                                       │
│  Progress    │                                                   │
│  Bar         │                                                   │
│              │                                                   │
└──────────────┴──────────────────────────────────────────────────┘
```

**Top Bar (`.topbar`):** Fixed at the top. Contains the brand emblem on the left, a breadcrumb text label, and the page-switcher pill tabs on the right that link to all six pages.

**Sidebar (`.sidebar`):** Fixed on the left. Contains the course logo/title, a search input (client-side text filter of nav items), and the numbered section navigation links for the current page. A reading progress bar sits at the bottom.

**Main Content (`.main` → `.content`):** Scrollable area to the right of the sidebar. Starts with a `.hero` section and then contains numbered `<section>` elements.

### Interactive JavaScript Features

Each page includes a small `<script>` block at the bottom that powers:

1. **Copy buttons on code blocks** — `doCopy(btn)` reads the `<pre>` text within `.cb` and writes it to the clipboard using `navigator.clipboard.writeText()`, with a fallback to `execCommand('copy')`.

2. **Reading progress bar** — Listens to `window.scroll` and updates both `#read-progress-fill` (the thin bar at the top of the sidebar) and `#np-fill` / `#np-pct` (the progress bar at the bottom of the sidebar).

3. **Active section highlighting** — An `IntersectionObserver` watches `.section-anchor` elements and adds the `.active` class to the corresponding `.nav-link` in the sidebar as the user scrolls.

4. **Sidebar search filter** — The search input in the sidebar filters `.nav-link` elements in real time based on the typed text.

5. **Mobile menu toggle** — A `#menu-toggle` button appears on small screens to show/hide the sidebar.

6. **Back-to-top button** — `#btt` appears after scrolling 500px and smooth-scrolls back to the top.

---

## 6. How to Make Changes

### Editing Existing Content

1. Open the relevant `.html` file in any text editor (VS Code is recommended).
2. Find the section you want to modify. Each section is wrapped in:
   ```html
   <div class="section-anchor" id="sN"></div>
   <section class="section">
     <div class="sec-header">
       <span class="sec-num">N</span>
       <div class="sec-title-block">
         <h2>Section Title</h2>
         <div class="sec-subtitle">Brief subtitle text</div>
       </div>
     </div>
     <!-- Section content goes here -->
   </section>
   ```
   Replace `N` with the section number. The `id="sN"` on the anchor div is what the sidebar navigation links reference (e.g., `href="#s3"`).
3. Edit the content inside `<section>` and save the file.
4. Test locally by opening the file directly in a browser (no server required — it's all static).
5. Commit and push to `main`. GitHub Pages will update automatically.

---

### Adding a New Section to an Existing Page

To add a new section at the end of an existing page:

**Step 1 — Add the section anchor and `<section>` block.** Place this before the closing `</div><!-- /content -->` tag:

```html
<!-- §N — Your New Section Title -->
<div class="section-anchor" id="sN"></div>
<section class="section">
  <div class="sec-header">
    <span class="sec-num">N</span>
    <div class="sec-title-block">
      <h2>Your New Section Title</h2>
      <div class="sec-subtitle">A one-line description of what this section covers</div>
    </div>
  </div>

  <!-- Add your content here using the component patterns described in Section 7 -->

</section>
```

Replace `N` with the next sequential number.

**Step 2 — Add the nav link in the sidebar.** Find the `<div class="nav-scroll">` block and add a new link in the appropriate nav group:

```html
<a class="nav-link" href="#sN"><span class="nl-num">0N</span>Your New Section Title</a>
```

The `nl-num` badge should always be zero-padded to two digits (e.g., `01`, `02`, ... `09`, `10`, `11`).

**Step 3 — Test locally** by opening the file in a browser. Verify that clicking the new sidebar link scrolls to the correct section and that the active highlighting works.

---

### Adding a New Page

To add a completely new page (e.g., a new hardware guide):

**Step 1 — Copy an existing page as your starting template.** The best template to copy is `rpi_setup.html` because it has the cleanest structure with no unusual components.

```bash
cp rpi_setup.html your_new_page.html
```

**Step 2 — Update the `<title>` and `<meta name="description">` tags** in the `<head>`:

```html
<title>ECE/MAE 148 — Your New Page Title</title>
<meta name="description" content="Description of your new page.">
```

**Step 3 — Update the `.topbar-breadcrumb` text:**

```html
<span class="topbar-breadcrumb">Your New Page Subtitle</span>
```

**Step 4 — Update the page-switcher tabs.** Add your new page to the `.page-switcher` div and update the `active` class to point to your new page. You must also add this link to **every other page's** page-switcher div so users can navigate to your new page from anywhere on the site:

```html
<div class="page-switcher">
  <a href="./rpi_setup.html" class="ps-link">RPI Setup</a>
  <a href="./vesc_setup.html" class="ps-link">VESC Setup</a>
  <a href="./donkeycar.html" class="ps-link">DonkeyCar</a>
  <a href="./gps_laps.html" class="ps-link">GPS Laps</a>
  <a href="./index.html" class="ps-link">ROS Setup</a>
  <a href="./your_new_page.html" class="ps-link active">New Page</a>  <!-- active on this page only -->
</div>
```

**Step 5 — Update the sidebar header** to reflect the new page topic:

```html
<div class="sidebar-title">ECE/MAE 148</div>
<div class="sidebar-sub">Your New Page Title</div>
```

**Step 6 — Clear out all the old section content** and replace the hero section and `<section>` blocks with your new content.

**Step 7 — Add your new page to the README** (this document) under both the file structure and the page-by-page breakdown.

**Step 8 — Commit and push** both the new file and the updated existing files (all other pages need the new tab in their page-switcher).

---

### Updating the Navigation Between Pages

The top-bar page switcher is defined **independently in each file.** There is no shared nav component. This means:

- To **rename a tab label**, update the link text in all six (or more) files.
- To **rename a file**, update all `href` references pointing to that file across all pages and update `index.html` if the main page filename ever changes.
- The `active` class on a `.ps-link` should **only appear on the link that corresponds to the current page.** Every other link on that page should have no `active` class.

---

## 7. Content Component Reference

These are the HTML patterns used throughout the site. Copy these templates directly when adding new content.

### Code Blocks (`.cb`)

Used for terminal commands, file contents, and configuration values. The copy button is wired automatically by the JavaScript at the bottom of each file.

```html
<div class="cb">
  <div class="cb-header">
    <span class="cb-label">Optional label text (e.g., filename or description)</span>
    <button class="copy-btn" onclick="doCopy(this)">COPY</button>
  </div>
  <pre>your command or code here</pre>
</div>
```

For syntax-highlighted code, wrap tokens in `<span>` tags with the following classes:
- `.k` — keywords / command names (cyan)
- `.s` — strings (yellow-green)
- `.c` — comments (grey)
- `.n` — numbers (purple)

Example:
```html
<pre><span class="k">docker run</span> --name MY_CONTAINER <span class="s">"djnighti/ucsd_robocar:latest"</span></pre>
```

---

### Callout Boxes (`.callout`)

Used for tips, warnings, and important notes that need to stand out from the body text.

```html
<!-- Tip (green) -->
<div class="callout tip">
  <div class="callout-icon">✓</div>
  <div class="callout-body">
    <div class="callout-title">Tip Title</div>
    <div class="callout-text">Tip body text here. Can include <code>inline code</code>.</div>
  </div>
</div>

<!-- Warning (yellow) -->
<div class="callout warn">
  <div class="callout-icon">⚠</div>
  <div class="callout-body">
    <div class="callout-title">Warning Title</div>
    <div class="callout-text">Warning body text here.</div>
  </div>
</div>

<!-- Danger (red) -->
<div class="callout danger">
  <div class="callout-icon">✕</div>
  <div class="callout-body">
    <div class="callout-title">Danger Title</div>
    <div class="callout-text">Critical warning text here.</div>
  </div>
</div>

<!-- Note (purple) -->
<div class="callout note">
  <div class="callout-icon">ℹ</div>
  <div class="callout-body">
    <div class="callout-title">Note Title</div>
    <div class="callout-text">Note body text here.</div>
  </div>
</div>
```

---

### Tables (`.tw`)

All tables should be wrapped in a `.tw` div for horizontal scroll on small screens.

```html
<div class="tw">
  <table>
    <thead>
      <tr>
        <th>Column A</th>
        <th>Column B</th>
        <th>Column C</th>
      </tr>
    </thead>
    <tbody>
      <tr>
        <td><code>some_value</code></td>
        <td>Description of what it does</td>
        <td>Additional detail</td>
      </tr>
    </tbody>
  </table>
</div>
```

---

### Step Cards (`.steps` / `.step-card`)

Used when a procedure must be followed in a strict sequence (e.g., flashing firmware, running a wizard).

```html
<div class="steps">
  <div class="step-card">
    <div class="step-num">1</div>
    <div class="step-content">
      <div class="step-title">Step Title</div>
      <p>Step description. Explain what to do and what to expect.</p>
      <!-- Optional: add a .cb code block inside the step -->
    </div>
  </div>

  <div class="step-card">
    <div class="step-num">2</div>
    <div class="step-content">
      <div class="step-title">Next Step</div>
      <p>Next step description.</p>
    </div>
  </div>
</div>
```

---

### Info Cards and Grid Layouts

Used on the index page for overview cards (e.g., hardware requirements, config file breakdowns).

```html
<!-- Two-column card grid -->
<div class="card-grid">
  <div class="card">
    <div class="card-icon">🔧</div>
    <div class="card-title">Card Title</div>
    <div class="card-text">Card description text.</div>
  </div>
  <div class="card">
    <div class="card-icon">📁</div>
    <div class="card-title">Another Card</div>
    <div class="card-text">Another description.</div>
  </div>
</div>
```

For YAML/config parameter tables with inline code and type annotations, use the standard `.tw` table pattern with `<code>` tags in the first column.

---

### Section Dividers

Use a `.divider` to add visual breathing room between major topic groups within a single section:

```html
<div class="divider"></div>
```

Use `<h3>` tags for sub-headings within a section (e.g., grouping troubleshooting tables by category). Do not use `<h2>` inside a `<section>` as these are reserved for section titles in the `.sec-header` block.

---

## 8. Deploying Changes via GitHub Pages

This site uses GitHub Pages with the following configuration:

- **Branch:** `main`
- **Source directory:** Root (`/`)
- **Entry point:** `index.html` at the root

**Deployment workflow:**

1. Make your changes locally in the HTML files.
2. Test by opening the file(s) directly in a browser (Chrome or Firefox). Because this is pure static HTML, no local server is required.
3. Commit your changes with a descriptive commit message:
   ```
   git add index.html rpi_setup.html
   git commit -m "docs: update Docker run command flags in ROS setup (Section 04)"
   git push origin main
   ```
4. GitHub Pages will rebuild and deploy within 1–3 minutes.
5. Verify the live site at `https://josecv04.github.io/ECEMAE148_RPI_Documentation.github.io/` (or the configured custom domain if one is set up).

> **There is no build step, no CI pipeline, and no npm/yarn.** What you push is exactly what is served. If you see an old version of the page after pushing, do a hard refresh in your browser (`Ctrl+Shift+R` / `Cmd+Shift+R`) to bypass the browser cache.

---

## 9. Style and Tone Guidelines

Consistency in voice and formatting is important for a course reference that multiple TAs and a professor may edit over time. Follow these conventions:

**Voice:** Use the second person ("you") and the imperative mood for instructions. Write as if you are standing next to the student during lab.
- ✅ "Run the following command to build the workspace."
- ❌ "The following command should be run by the student."

**Command formatting:** All terminal commands must be in a `.cb` code block with a copy button. Never put a command only in inline `<code>` tags — students will be copy-pasting these into a terminal.

**Path and filename formatting:** Use `<code>` inline tags for file paths, filenames, YAML keys, Python identifiers, and ROS topic names (e.g., `myconfig.py`, `/cmd_vel`, `Kp_steering`).

**Callout usage:**
- Use `.callout.tip` (green) for shortcuts, best practices, and time-saving advice.
- Use `.callout.warn` (yellow) for things that commonly cause confusion or wasted time.
- Use `.callout.danger` (red) for actions that can damage hardware or permanently corrupt data (e.g., "Do not run motor detection without securing the car").
- Use `.callout.note` (purple) for informational context that isn't critical to following the steps.

**Section subtitles:** Keep them to one short sentence. They appear below the section heading in smaller muted text and give a quick preview of what the section covers.

**Section numbering:** Sections are numbered sequentially starting from 01 within each page. They are independent across pages — each page has its own numbering starting from 01.

**Images:** There are currently no images in any of the HTML files. If you want to add a screenshot, either host it in a dedicated `assets/` folder in the repository and reference it with a relative path (`<img src="./assets/my_screenshot.png" alt="Description">`), or upload it as a GitHub release asset and use the absolute URL. Do not use base64-encoded images inline — they will bloat the HTML file significantly.

---

## 10. Common Mistakes to Avoid

**Forgetting to update all six page-switcher blocks.** When you add or rename a page, you must update the `.page-switcher` nav in every file. Missing one file means users clicking that page's tab will see a broken/missing link or navigate to the wrong page.

**Breaking section anchor IDs.** The sidebar nav links use `href="#sN"` and the JavaScript `IntersectionObserver` watches `div.section-anchor[id]`. If you add or reorder sections and the IDs get out of sync with the nav links, the active-highlighting will break. Always verify IDs match between the `.section-anchor` div and the corresponding `.nav-link` href.

**Not testing locally before pushing.** Open the file in a browser before committing. Check that copy buttons work, that sidebar links scroll to the right location, and that new callout boxes or tables render correctly.

**Putting commands only in inline `<code>` tags.** Students are expected to copy-paste commands. Always put commands that need to be run in a `.cb` block with the copy button.

**Forgetting `build_ros2` vs `source_ros2`.** This is the most common student confusion in the ROS2 guide. Any time you update documentation about the ROS2 workflow, be explicit about when each alias is needed. YAML changes require `build_ros2`. Just sourcing the workspace with `source_ros2` alone is not sufficient.

**Removing safety callouts in the VESC guide.** The `.callout.danger` block in Section 01 of `vesc_setup.html` warns students to physically secure the car before running motor detection. Do not remove this — unsecured cars have flipped off tables during motor detection in past quarters.

---

## 11. Versioning and Change Log Conventions

There is currently no formal versioning system in this repository. It is recommended that future maintainers adopt the following practices:

**Commit message conventions:** Use a short prefix to categorize changes:
- `docs:` — Content additions, corrections, or reorganization
- `style:` — Visual/CSS changes with no content effect
- `fix:` — Correcting a broken command, outdated step, or bad link
- `feat:` — Adding a new page or major new section
- `refactor:` — Restructuring HTML without changing visible content

**Start-of-quarter checklist:** At the beginning of each academic quarter, a TA should:
1. Verify all software version numbers mentioned in the guides (DonkeyCar, VESC firmware, ROS2 image tag, Python version).
2. Verify the UCSD RTK NTRIP server credentials in `gps_laps.html` Section 04.
3. Check whether the DonkeyCar patches in `donkeycar.html` Sections 07–09 are still required against the current DonkeyCar release.
4. Verify the Docker image tag in `index.html` Section 02 points to the currently recommended image version.
5. Test the live site URL to confirm GitHub Pages is serving the correct content.

**Adding a changelog section (recommended for future TAs):** Consider adding a short HTML comment block at the top of each file's `<body>` documenting major changes:

```html
<!-- 
  CHANGELOG
  2026-01 (J. Cervantes): Initial release. All six pages created.
  2026-04 (Future TA): Updated Docker image tag to v2.x. Added Section 17 on LiDAR.
-->
```

This provides a lightweight audit trail without requiring a separate file.

---

*This README was written to be sufficient for a new TA with no prior exposure to this repository to understand the site, make confident edits, and hand it off to the next person in equally good shape. If you find anything unclear or out of date, please update this document as part of the same commit where you make your changes.*

---

**UCSD Jacobs School of Engineering — ECE/MAE 148: Introduction to Autonomous Vehicles**
