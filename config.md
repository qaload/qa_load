How to find the large file extension:
```bash
du -a  | sort -n | grep -E ".*\.[^\.,\/]+$"
```

How to add the file extension to the LFS:
```bash
git lfs track "*.zip"
```