# Visual Studio Code Setup Guide for SEH500 Project

Complete step-by-step guide for setting up VS Code, connecting to GitHub, cloning the repository, and working with MCUXpresso IDE.

---

## Prerequisites

- Visual Studio Code installed
- Git installed (usually comes with VS Code)
- GitHub account
- MCUXpresso IDE installed
- FRDM-K66F board and USB cable

---

## Step 1: Install Visual Studio Code Extensions

1. Open Visual Studio Code
2. Click the Extensions icon (or press `Ctrl+Shift+X`)
3. Install these extensions:
   - **GitLens** (by GitKraken) - Enhanced Git capabilities
   - **Git History** (by Don Jayamanne) - View git history
   - **C/C++** (by Microsoft) - C/C++ language support
   - **C/C++ Extension Pack** (by Microsoft) - Complete C/C++ tooling

---

## Step 2: Connect VS Code to GitHub

### Option A: Using GitHub Account (Recommended)

1. In VS Code, click the **Source Control** icon (or press `Ctrl+Shift+G`)
2. Click **"Sign in with GitHub"** or **"Clone Repository"**
3. A browser window will open
4. Sign in to your GitHub account
5. Authorize VS Code to access your GitHub account
6. Return to VS Code - you should now be connected

### Option B: Using Personal Access Token

1. Go to GitHub.com → Settings → Developer settings → Personal access tokens → Tokens (classic)
2. Click "Generate new token (classic)"
3. Name it "VS Code Access"
4. Select scopes: `repo`, `workflow`
5. Click "Generate token"
6. Copy the token (you won't see it again!)
7. In VS Code, when prompted for credentials, use:
   - Username: Your GitHub username
   - Password: The token you just created

---

## Step 3: Clone the Repository

### Method 1: Using VS Code Command Palette

1. Press `Ctrl+Shift+P` to open Command Palette
2. Type: `Git: Clone`
3. Enter repository URL: `https://github.com/HadjBen/SEH500---Group-2-Project.git`
4. Select a folder to clone into (e.g., `C:\Users\YourName\Documents\Projects\`)
5. Click "Select Repository Location"
6. VS Code will ask to open the cloned repository - click **"Open"**

### Method 2: Using Terminal in VS Code

1. Open terminal in VS Code: `View` → `Terminal` (or press `` Ctrl+` ``)
2. Navigate to where you want the project:
   ```bash
   cd C:\Users\YourName\Documents\Projects
   ```
3. Clone the repository:
   ```bash
   git clone https://github.com/HadjBen/SEH500---Group-2-Project.git
   ```
4. Open the folder in VS Code:
   ```bash
   code SEH500---Group-2-Project
   ```
   Or: `File` → `Open Folder` → Select `SEH500---Group-2-Project`

---

## Step 4: Create a New Branch

### Using VS Code GUI

1. Click the branch name in the bottom-left corner of VS Code (should say "main")
2. Click **"Create new branch..."**
3. Enter branch name: `audio-implementation` or `feature/sd-card-audio`
4. Press Enter
5. VS Code will switch to your new branch automatically

### Using Terminal

1. Open terminal in VS Code (`` Ctrl+` ``)
2. Create and switch to new branch:
   ```bash
   git checkout -b audio-implementation
   ```
3. Verify you're on the new branch:
   ```bash
   git branch
   ```
   You should see `* audio-implementation`

---

## Step 5: Open Project in MCUXpresso IDE

1. **Open MCUXpresso IDE**
2. **File** → **Import**
3. Select **"Existing Projects into Workspace"**
4. Click **Next**
5. Click **Browse** and navigate to your cloned repository folder:
   ```
   C:\Users\YourName\Documents\Projects\SEH500---Group-2-Project
   ```
6. Select the `SEH500_Project` folder (the project root)
7. Check **"Copy projects into workspace"** (optional - recommended)
8. Click **Finish**

**Important:** MCUXpresso IDE will now have the project open. Any changes you make in VS Code will automatically appear in MCUXpresso IDE because both editors are looking at the same files!

---

## Step 6: Verify File Sync Between VS Code and MCUXpresso IDE

1. In VS Code, open `source/SEH500_Project.c`
2. Add a comment like: `// Test comment from VS Code`
3. Save the file (`Ctrl+S`)
4. Switch to MCUXpresso IDE
5. The file should automatically refresh (or click the file to refresh)
6. You should see your comment!

**Note:** Both editors read from the same files, so changes sync automatically. However:
- **Always save files** before switching editors
- **MCUXpresso IDE may need a refresh** - click the file tab or press `F5`
- **Build in MCUXpresso IDE** - VS Code doesn't have the MCUXpresso build tools

---

## Step 7: Workflow for Implementing Audio

### Recommended Workflow

1. **Edit in VS Code** (better editor, Git integration)
2. **Save files** (`Ctrl+S`)
3. **Switch to MCUXpresso IDE** to build and test
4. **Build in MCUXpresso IDE**: Right-click project → `Build Project`
5. **Flash and test** on hardware
6. **Return to VS Code** to commit changes

### Step-by-Step Implementation

1. **Read the handoff document:**
   - Open `documents/PARTNER_HANDOFF.md` in VS Code
   - Follow the implementation steps

2. **Make code changes in VS Code:**
   - Edit `source/SEH500_Project.c`
   - Add new functions as described in handoff guide
   - Use VS Code's IntelliSense for code completion

3. **Save all files** (`Ctrl+Shift+S` saves all)

4. **Build in MCUXpresso IDE:**
   - Switch to MCUXpresso IDE
   - Right-click project → `Build Project`
   - Fix any compilation errors
   - Return to VS Code to fix errors if needed

5. **Test on hardware:**
   - Flash to board in MCUXpresso IDE
   - Test functionality
   - Debug if needed

6. **Commit changes in VS Code:**
   - See Step 8 below

---

## Step 8: Commit and Push Changes from VS Code

### Using VS Code Source Control Panel

1. **Make your changes** and save all files

2. **Open Source Control:**
   - Click the Source Control icon (or press `Ctrl+Shift+G`)
   - You'll see all modified files listed

3. **Stage files:**
   - Click the **"+"** icon next to each file you want to commit
   - Or click **"+"** next to "Changes" to stage all files
   - Staged files move to "Staged Changes"

4. **Write commit message:**
   - In the message box at the top, enter:
     ```
     Implement SD card audio playback
     
     - Added SD card initialization
     - Implemented audio hardware setup
     - Integrated WAV file playback
     - Added 10-second repeat functionality
     ```

5. **Commit:**
   - Press `Ctrl+Enter` or click the checkmark icon
   - Your changes are now committed locally

6. **Push to GitHub:**
   - Click the **"..."** menu (three dots) in Source Control panel
   - Select **"Push"**
   - Or use the sync icon (circular arrows) in the bottom status bar
   - VS Code will push your branch to GitHub

### Using Terminal in VS Code

1. Open terminal (`` Ctrl+` ``)

2. Check status:
   ```bash
   git status
   ```

3. Stage files:
   ```bash
   git add source/SEH500_Project.c
   # Or stage all changes:
   git add .
   ```

4. Commit:
   ```bash
   git commit -m "Implement SD card audio playback

   - Added SD card initialization
   - Implemented audio hardware setup
   - Integrated WAV file playback
   - Added 10-second repeat functionality"
   ```

5. Push:
   ```bash
   git push origin audio-implementation
   ```
   (Replace `audio-implementation` with your branch name)

---

## Step 9: View Changes and History

### View File Changes

1. Click on a modified file in Source Control panel
2. VS Code shows a diff view (green = added, red = deleted)
3. You can see exactly what changed

### View Commit History

1. Click the **"..."** menu in Source Control
2. Select **"View History"** or **"Show Git Output"**
3. Or use GitLens extension for enhanced history view

### Compare with Previous Version

1. Right-click a file in Source Control
2. Select **"Open Changes"** or **"Compare with Previous"**

---

## Step 10: When Audio Implementation is Complete

### Create Pull Request from VS Code

1. **Push your final changes:**
   - Follow Step 8 to commit and push

2. **VS Code will show a notification:**
   - "Create Pull Request" button appears
   - Click it to open GitHub in browser

3. **Or manually create PR:**
   - Go to: https://github.com/HadjBen/SEH500---Group-2-Project
   - Click "Pull Requests" → "New Pull Request"
   - Select your branch
   - Add description and create PR

### Merge Locally (If You Have Access)

1. Switch to main branch:
   ```bash
   git checkout main
   ```

2. Pull latest changes:
   ```bash
   git pull origin main
   ```

3. Merge your branch:
   ```bash
   git merge audio-implementation
   ```

4. Push to main:
   ```bash
   git push origin main
   ```

---

## VS Code Tips and Tricks

### Keyboard Shortcuts

- `` Ctrl+` `` - Toggle terminal
- `Ctrl+Shift+G` - Open Source Control
- `Ctrl+Shift+P` - Command Palette
- `Ctrl+P` - Quick file open
- `Ctrl+F` - Find in file
- `Ctrl+Shift+F` - Find in all files
- `Ctrl+/` - Toggle comment
- `Ctrl+S` - Save file
- `Ctrl+Shift+S` - Save all files
- `F5` - Refresh (in MCUXpresso IDE)

### Useful VS Code Features

1. **Multi-cursor editing:**
   - `Alt+Click` to add multiple cursors
   - `Ctrl+Alt+Up/Down` to add cursors above/below

2. **Code formatting:**
   - `Shift+Alt+F` - Format document
   - Right-click → "Format Document"

3. **Go to definition:**
   - `F12` - Go to definition
   - `Alt+F12` - Peek definition
   - `Ctrl+Click` - Go to definition

4. **Find references:**
   - `Shift+F12` - Find all references

5. **Rename symbol:**
   - `F2` - Rename symbol (renames all occurrences)

---

## Troubleshooting

### VS Code Not Showing Git Changes

1. Check if Git is installed:
   ```bash
   git --version
   ```

2. Reload VS Code window:
   - `Ctrl+Shift+P` → "Reload Window"

3. Check Git is enabled:
   - Settings → Search "git.enabled" → Should be checked

### Files Not Syncing Between Editors

1. **Save files** before switching (`Ctrl+S`)
2. **Refresh MCUXpresso IDE** - Click file tab or press `F5`
3. **Check file paths** - Make sure both editors are looking at the same folder

### Can't Push to GitHub

1. **Check authentication:**
   - `Ctrl+Shift+P` → "Git: Show Git Output"
   - Look for authentication errors

2. **Re-authenticate:**
   - `Ctrl+Shift+P` → "Git: Clone" → Will prompt for auth

3. **Check branch name:**
   ```bash
   git branch
   ```
   Make sure you're on your feature branch

### MCUXpresso IDE Build Errors

1. **Clean build:**
   - Right-click project → `Clean Project`
   - Then `Build Project`

2. **Refresh project:**
   - Right-click project → `Refresh`

3. **Check file encoding:**
   - Make sure files are saved as UTF-8 in VS Code

---

## Quick Reference Workflow

```
1. Clone repo in VS Code
2. Create branch: audio-implementation
3. Open project in MCUXpresso IDE
4. Edit code in VS Code
5. Save files (Ctrl+S)
6. Build in MCUXpresso IDE
7. Test on hardware
8. Commit in VS Code (Ctrl+Shift+G)
9. Push to GitHub
10. Create Pull Request when done
```

---

## Best Practices

1. **Commit often** - Small, logical commits are better than one big commit
2. **Test before committing** - Make sure code compiles and works
3. **Write good commit messages** - Describe what and why
4. **Pull before pushing** - Make sure you have latest changes
5. **Use descriptive branch names** - `feature/sd-card-audio` is better than `test`
6. **Keep VS Code and MCUXpresso IDE in sync** - Save files before switching
7. **Build in MCUXpresso IDE** - VS Code can't build MCUXpresso projects

---

## Getting Help

- **VS Code Git issues:** Check Git output (`Ctrl+Shift+P` → "Git: Show Git Output")
- **Implementation questions:** See `documents/PARTNER_HANDOFF.md`
- **Git workflow:** See `documents/COLLABORATION_GUIDE.md`
- **Project requirements:** See `documents/PROJECT_REQUIREMENTS.md`

---

*This guide assumes you're working on Windows. Mac/Linux users can adapt the paths and some keyboard shortcuts.*

