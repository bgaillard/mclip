# :clipboard: mclip

mclip (for "**M**emory **Clip**board") is a very simple Linux clipboard manager which stores its data inside a POSIX shared memory object.

Today several clipboad tools exist but almost all of them require a specific dependency (X11, Wayland, Tmux, SSH, etc.). See for example the [Clipboard integration](https://neovim.io/doc/user/provider.html#provider-clipboard) page in the Neovim documentation.

mclip is a tiny binary which only depends on low level Linux system functions like [`shm_open`](https://man7.org/linux/man-pages/man3/shm_open.3.html) and [`mmap`](https://man7.org/linux/man-pages/man2/mmap.2.html). So it should be usable in very basic environments.

## Install

## Usage

This section explains how to use mclip in several tools.

- [CLI](#cli)
- [Neovim](#neovim)
- [Zellij](#zellij)

### CLI

Display the help.

```bash
mclip --help
Usage: mclip [OPTION]

Clipboard which stores its data inside shared memory.

         h-, --help      show quick summary of options
         i-, --in        read text from standard input and write it into the clipboard shared memory (default)
         o-, --out       print the content of the clipboard shared memory
```

To save text into the clipboard just enter it through stdin.

```bash
# Using a pipe
$ echo "Hello world!" | mclip

# Entering characters with the keyboard (press Ctrl+D to finish your input)
$ mclip
I can
enter 
text 
on 
several 
lines
<Ctrl+D>
```

You can finally output the content of the clipboard.

```bash
$ mclip -o
Hello world!
```

### Neovim

[Neovim](https://neovim.io) provides a [Clipboard integration](https://neovim.io/doc/user/provider.html#provider-clipboard) which allows to provide any clipboard tool. 

You can configure mclip as the Neovim clipboard tool using the following Lua configuration object.

```lua
vim.g.clipboard = {
    name = "mclip",
    copy = {
        ["+"] = "mclip",
        ["*"] = "mclip",
    },
    paste = {
        ["+"] = "mclip -o",
        ["*"] = "mclip -o",
    },
    cache_enabled = 1,
}
```

## Zellij

TODO
