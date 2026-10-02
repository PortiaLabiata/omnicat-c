# omnicat

omnicat is a simple utility akin to socat, but supporting multiple endpoints instead of two. It currently works on Linux and Windows with the help of Cygwin.

## Obtaining

To obtain omnicat, simply clone this repository with command:

```sh
    git clone --recurse-submodules https://github.com/PortiaLabiata/omnicat-c.git
```

It doesn't have external dependencies other than [cJSON](https://github.com/davegamble/cjson). To build and install this software, simply run 

```sh
    make
    make install
```

## Usage

Configuration is done in a single JSON file. This program is ran as follows:

```sh
    omnicat config.json
```

Configuration file should have this kind of structure:

```json
    {
        "configs": [
            { config-1 },
            { config-2 },
            ...
            { config-n }
        ]
    }
```

where configs are configurations for endpoints. Currently, theese types of endpoints are supported:

 - Stdio ("stdio")
 - UDP ("udp")
 - TCP ("tcp")
 - File ("file")
 - Serial port ("serial")

Endpoint type should be specified in "kind" field of config object. Mandatory fields are also "name" (a human-readable name by which endpoint will be identified) and "to", which is a list of destination endpoints' names. Such a structure allows building relatively robust endpoints' nets. It is also possible to specify internal buffer size (to account for MTU i. e.) through parameter "rxbuf_size".

Some endpoint types require additional parameters, like:

 - UDP: "addr", "server", "port", "reuseaddr", "so_rcvbuf", "so_sndbuf"
 - TCP: like UDP
 - Serial: "addr" (path to file), "echo", "canon", "raw" (for now, more will be added later, theese are just options, that I needed most)
 - File: "addr"

# Licensing

This code is distributed under the MIT License, see "LICENSE.md".
