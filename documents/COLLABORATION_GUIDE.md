# Collaboration Guide - Git Workflow

## For Partners Working on Audio Implementation

This guide explains how to collaborate on this project using Git branches.

---

## Initial Setup

### 1. Clone the Repository

```bash
git clone https://github.com/HadjBen/SEH500---Group-2-Project.git
cd SEH500---Group-2-Project
```

### 2. Verify Current Branch

```bash
git branch
# Should show: * main
```

---

## Working on Audio Implementation

### Step 1: Create Your Feature Branch

Create a new branch for your audio work:

```bash
git checkout -b audio-implementation
```

Or use a more descriptive name:
```bash
git checkout -b feature/sd-card-audio
```

### Step 2: Work on Your Changes

- Make your code changes
- Test thoroughly
- Follow the guide in `documents/PARTNER_HANDOFF.md`

### Step 3: Commit Your Changes

When you have working code, commit it:

```bash
# Check what files changed
git status

# Add specific files
git add source/SEH500_Project.c
git add source/your_new_file.c

# Or add all changes
git add .

# Commit with descriptive message
git commit -m "Implement SD card audio playback functionality

- Added SD card initialization
- Implemented audio hardware setup
- Integrated WAV file playback
- Added 10-second repeat functionality"
```

**Commit Message Best Practices:**
- Use clear, descriptive messages
- First line should be a brief summary (50 chars or less)
- Add detailed description if needed
- Reference issues or tasks if applicable

### Step 4: Push Your Branch

Push your branch to GitHub:

```bash
git push -u origin audio-implementation
```

The `-u` flag sets up tracking so future pushes are simpler.

---

## Updating Your Branch

### If Main Branch Gets Updated

If the main branch is updated while you're working:

```bash
# Switch to main branch
git checkout main

# Pull latest changes
git pull origin main

# Switch back to your branch
git checkout audio-implementation

# Merge main into your branch
git merge main

# Resolve any conflicts if they occur
# Then push your updated branch
git push origin audio-implementation
```

---

## When Audio Implementation is Complete

### Option 1: Create Pull Request (Recommended)

1. Push your final changes:
   ```bash
   git push origin audio-implementation
   ```

2. Go to GitHub: https://github.com/HadjBen/SEH500---Group-2-Project

3. Click "Pull Requests" → "New Pull Request"

4. Select your branch (`audio-implementation`) to merge into `main`

5. Add description of changes and request review

6. Once approved, merge the pull request


---
