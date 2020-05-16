#!/usr/bin/env python3

# Serve ".wasm" files as "application/wasm".

from http.server import SimpleHTTPRequestHandler, test


if __name__ == '__main__':
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument('--bind', '-b', default='', metavar='ADDRESS',
                        help='Specify alternate bind address '
                             '[default: all interfaces]')
    parser.add_argument('port', action='store',
                        default=8000, type=int,
                        nargs='?',
                        help='Specify alternate port [default: 8000]')
    args = parser.parse_args()
    handler_class = SimpleHTTPRequestHandler
    handler_class.extensions_map[".wasm"] = "application/wasm"
    test(HandlerClass=handler_class, port=args.port, bind=args.bind)
