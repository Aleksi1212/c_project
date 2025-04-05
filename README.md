# Repository manager (GitGobbler)

**University project.** <br> 
Keeps track of users repositories. Program recods aliases (short names) for long repository links and allows user to view and manage the aliases.

## Building the app
```
mkdir build
cd build
cmake ..
make -j$(nproc)
```
### Clean build
```
rm -rf build
```

## Running the app

### Add a repository
```
build/gitgobbler --add <alias> <repository_link>
```

### Get repository with an alias or all repositories
```
build/gitgobbler --get <alias> | --get
```

### List all aliases
```
build/gitgobbler --list
```

### Delete an alias or all aliases
```
build/gitgobbler --delete <alias> | --delete
```

### Help me
```
build/gitgobbler --help
```