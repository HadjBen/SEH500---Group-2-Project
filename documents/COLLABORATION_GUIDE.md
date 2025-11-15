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

### Option 2: Direct Merge (If You Have Access)

```bash
# Switch to main
git checkout main

# Merge your branch
git merge audio-implementation

# Push to main
git push origin main
```

---

## Common Git Commands

### Check Status
```bash
git status                    # See what files changed
git log --oneline            # See commit history
git diff                     # See changes not yet staged
```

### Undo Changes
```bash
git checkout -- filename     # Discard changes to file
git reset HEAD filename      # Unstage file (keep changes)
git reset --hard HEAD        # Discard ALL uncommitted changes (careful!)
```

### Branch Management
```bash
git branch                   # List all branches
git branch -d branch-name    # Delete local branch
git branch -a                # List all branches (local + remote)
```

---

## Best Practices

1. **Commit Often**: Make small, logical commits rather than one big commit
2. **Test Before Committing**: Make sure code compiles and works
3. **Write Good Commit Messages**: Describe what and why, not just what
4. **Keep Branch Updated**: Regularly merge main into your branch
5. **Don't Commit Build Files**: `.gitignore` handles this, but double-check
6. **Communicate**: Let team know when you push major changes

---

## Troubleshooting

### "Your branch is behind 'origin/main'"
```bash
git checkout main
git pull origin main
git checkout your-branch
git merge main
```

### "Merge Conflicts"
1. Git will mark conflicted files
2. Open files and look for `<<<<<<<`, `=======`, `>>>>>>>` markers
3. Edit to resolve conflicts
4. `git add` the resolved files
5. `git commit` to complete merge

### "Can't Push - Permission Denied"
- Make sure you have write access to the repository
- If not, fork the repo and create pull request from your fork

### "Accidentally Committed to Main"
```bash
# Create branch from current main
git checkout -b fix-branch

# Reset main to previous commit
git checkout main
git reset --hard HEAD~1

# Push your fix branch
git push origin fix-branch
```

---

## Quick Reference Workflow

```bash
# 1. Start working
git checkout -b audio-implementation

# 2. Make changes, test, then commit
git add .
git commit -m "Your commit message"
git push origin audio-implementation

# 3. When done, create pull request on GitHub
# Or merge directly if you have access:
git checkout main
git merge audio-implementation
git push origin main
```

---

## Questions?

- Check `documents/PARTNER_HANDOFF.md` for implementation details
- Check `documents/PROJECT_REQUIREMENTS.md` for project requirements
- Contact team members for Git/collaboration questions

