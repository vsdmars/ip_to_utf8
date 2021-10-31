# iuctl converts CIDR IPv4/IPv6 string to UTF8 format and vice versa.
```bash
$iuctl --help
```
```bash
iuctl converts CIDR IP string to UTF8 format and vice versa.
Usage: iuctl [OPTIONS] input

Positionals:
  input TEXT REQUIRED         string to be converted either to IP or UTF8

Options:
  -h,--help                   Print this help message and exit
```

## example
```
$ iuctl 192.168.1.1
\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u00ff\u00ff\u00c0\u00a8\u0001\u0001
```

```
$ iuctl "\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u00ff\u00ff\u00c0\u00a8\u0001\u0001"
192.168.1.1
```

```
$ echo "1.1.1.1" | xargs iuctl
\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u00ff\u00ff\u0001\u0001\u0001\u0001
```

```
$ iuctl "\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u0000\u00ff\u00ff\u0001\u0001\u0001\u0001"
1.1.1.1
```
