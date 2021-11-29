# Jump Directory

`jd` is a lightweight, powerful cd utility made to save time in jumping to common directories.

## Explanation

`jd` is a tool used to resolve file paths. Given a path, `jd` tries three different searches before declaring the directory cannot be found:

- Given an absolute path, `jd` checks whether it exists and terminates
- Given a relative path, `jd` simplifies the path and checks if the directory exists and terminates
- If the path is not either of those (i.e. does not start with "/", "./", "../", etc.), `jd` checks a config file containing a list of available directories. If the path describes all or the end of a given path in the file, `jd` terminates.

Each successful `jd` usage logs the directory entered. This allows for repeated or common directories to have aliases. The program itself is lightweight and simple enough that, if one were to replace it with `cd`, it would not feel jarringly different.

The `jd` binary itself only resolves and outputs file paths. To change to a directory, see the [shell functions](#shell-functions) section.

## Installation

### Building

To build the executable, cd into the project directory and run:

```sh
$ make install
```

This will put an executable called `jdir` in your `~/.local/bin` directory. If you prefer to put the executable somewhere run this instead:

```sh
$ make main
```

This will populate the `bin` directory in the project root with the `main` executable.

### Shell Functions

For security reasons, programs are not able to change their parent process's directory. This is the reason why the executable itself only outputs the directory. If this is the case then, in order to achieve this behavior, we must do something differently. The workaround chosen was to create a shell function that uses the executable. So, for example, you could put something like this your shell's startup file.

```sh
function jd() {
	cd $(jdir $1)
}
```

When the shell is reloaded, you will now be able to use `jd` at the commandline to change directories. This is only a simple example, however. For a much more competent and better looking function, use this:

```zsh
function jd() {
	dir=$(jdir $1)
	if [[ $? -eq 0 ]] then
		cd $dir
		echo -e "\e[1;93m$dir\e[0m"
		ls --group-directories-first
	else
		return $?
	fi
}
```

*Note that this is my personal function in `zsh`. You may have to change this function for your specific shell.*

### Completions

Currently, command-line completions have only been implemented for `zsh`.

#### ZSH

To use completions for zsh, copy `completions/_jd` to your preferred path in the listed completion directories, usually found in `$fpath` (ex. `~/.oh-my-zsh/completions`). This will then allow you to complete from all available directories in the current working directory (i.e. the same completion found with `cd`) as well the tails of any paths in the directory log.