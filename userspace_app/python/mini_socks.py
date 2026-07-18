#!/usr/bin/env python3
import socket, threading

# on server:
# python3 mini_socks.py
# on client:
# ssh -N -L 1080:127.0.0.1:9080 proxy_server

def recv_n(sock, n):
    d = b''
    while len(d) < n:
        b = sock.recv(n - len(d))
        if not b: raise ConnectionError
        d += b
    return d

def pipe(a, b):
    try:
        while True:
            d = a.recv(8192)
            if not d: break
            b.sendall(d)
    except: pass
    finally:
        try: a.close(); b.close()
        except: pass

def handle(c):
    try:
        # === 握手 ===
        ver, nmethods = recv_n(c, 2)
        recv_n(c, nmethods)          # 吃掉 method 列表
        c.sendall(b'\x05\x00')       # 无验证

        # === CONNECT ===
        ver, cmd, rsv, atyp = recv_n(c, 4)
        if cmd != 1: return

        if atyp == 1:
            addr = socket.inet_ntoa(recv_n(c, 4))
        elif atyp == 3:
            ln = recv_n(c, 1)[0]
            addr = recv_n(c, ln).decode(errors='ignore')
        elif atyp == 4:
            recv_n(c, 16); addr = '::1'
        else: return

        port = int.from_bytes(recv_n(c, 2), 'big')
        remote = socket.create_connection((addr, port), timeout=15)

        # 回复成功
        c.sendall(b'\x05\x00\x00\x01' + socket.inet_aton('0.0.0.0') + b'\x00\x00')
        threading.Thread(target=pipe, args=(remote, c), daemon=True).start()
        pipe(c, remote)
    except: pass
    finally:
        try: c.close()
        except: pass

s = socket.socket()
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.bind(('127.0.0.1', 9080))
s.listen(64)
print('[*] SOCKS5 ready on 127.0.0.1:9080')

while True:
    t, _ = s.accept()
    threading.Thread(target=handle, args=(t,), daemon=True).start()
