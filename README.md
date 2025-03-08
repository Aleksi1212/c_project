# Repository manager (GitGobbler)

**University project.** <br> 
Keeps track of users repositories. Program recods aliases (short names) for long repository links and allows user to view and manage the aliases.

## Building the app
```
make
```
### Clean build
```
make clean
```

## Running the app

### Add a repository
```
build/gitgobbler --add <alias> <repository_link>
```

### Show repository with an alias or all repositories
```
build/gitgobbler --show <alias | all>
```

### List all aliases
```
build/gitgobbler --list
```

### Delete an alias
```
build/gitgobbler --delete <alias>
```

### Help me
```
build/gitgobbler --help
```